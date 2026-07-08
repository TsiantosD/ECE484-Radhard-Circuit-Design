#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "backend.h"
#include "parser.h"
#include "netlist.h"
#include "levelization.h"

static void freeParsedNetlist(NodesArray *nodes_array,
                              NodesArray *primary_inputs_array,
                              GatesArray *gates_array,
                              LevelsArray *levels_array) {
    if (gates_array != NULL) {
        for (int i = 0; i < gates_array->size; i++) {
            Gate *curr_gate = gates_array->data[i];
            if (curr_gate != NULL) {
                free(curr_gate->inputs);
                free(curr_gate->outputs);
                free(curr_gate);
            }
        }
        free(gates_array->data);
        free(gates_array);
    }

    if (primary_inputs_array != NULL) {
        for (int i = 0; i < primary_inputs_array->size; i++) {
            free(primary_inputs_array->data[i]);
        }
        free(primary_inputs_array->data);
        free(primary_inputs_array);
    }

    if (nodes_array != NULL) {
        for (int i = 0; i < nodes_array->size; i++) {
            free(nodes_array->data[i]);
        }
        free(nodes_array->data);
        free(nodes_array);
    }

    if (levels_array != NULL) {
        for (int i = 0; i < levels_array->size; i++) {
            GatesArray *curr_gates_array = levels_array->data[i];
            if (curr_gates_array != NULL) {
                free(curr_gates_array->data);
                free(curr_gates_array);
            }
        }
        free(levels_array->data);
        free(levels_array);
    }
}

static int isSupportedBackend(const char *backend) {
    return strcmp(backend, "legacy") == 0 ||
           strcmp(backend, "x86") == 0 ||
           strcmp(backend, "flat") == 0 ||
           strcmp(backend, "cuda") == 0;
}

static double elapsedSeconds(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

static const char *baseName(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash == NULL ? path : slash + 1;
}

static void testNameFromPath(const char *path, char *out, size_t out_size) {
    const char *name = baseName(path);
    snprintf(out, out_size, "%s", name);
    char *dot = strrchr(out, '.');
    if (dot != NULL) {
        *dot = '\0';
    }

    for (size_t i = 0; out[i] != '\0'; i++) {
        if (!((out[i] >= 'a' && out[i] <= 'z') ||
              (out[i] >= 'A' && out[i] <= 'Z') ||
              (out[i] >= '0' && out[i] <= '9') ||
              out[i] == '_' || out[i] == '-')) {
            out[i] = '_';
        }
    }
}

static void timestampString(char *out, size_t out_size) {
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    strftime(out, out_size, "%Y%m%d_%H%M%S", &tm_now);
}

static void ensureTimingLogDir(const char *dir) {
    if (mkdir(dir, 0775) != 0 && errno != EEXIST) {
        perror("Warning: failed to create timing log directory");
    }
}

static void writeTimingLogs(const char *backend,
                            const char *verilog_path,
                            SerResult result,
                            double backend_elapsed_seconds,
                            double total_elapsed_seconds) {
    const char *log_dir = "timing_logs";
    char test_name[128];
    char timestamp[32];
    char log_path[512];
    char csv_path[512];

    testNameFromPath(verilog_path, test_name, sizeof(test_name));
    timestampString(timestamp, sizeof(timestamp));
    ensureTimingLogDir(log_dir);

    snprintf(log_path, sizeof(log_path), "%s/%s_%s_%s.log", log_dir, test_name, backend, timestamp);
    FILE *log_fp = fopen(log_path, "w");
    if (log_fp != NULL) {
        fprintf(log_fp, "test=%s\n", test_name);
        fprintf(log_fp, "backend=%s\n", backend);
        fprintf(log_fp, "verilog=%s\n", verilog_path);
        fprintf(log_fp, "backend_elapsed_seconds=%.9f\n", backend_elapsed_seconds);
        fprintf(log_fp, "total_elapsed_seconds=%.9f\n", total_elapsed_seconds);
        fprintf(log_fp, "total_simulations=%lld\n", result.total_simulations);
        fprintf(log_fp, "hit_gates=%lld\n", result.hit_gates_counter);
        fprintf(log_fp, "soft_errors=%lld\n", result.soft_error_counter);
        fprintf(log_fp, "ser_percent=%.6f\n", result.soft_error_rate * 100.0);
        fclose(log_fp);
    } else {
        perror("Warning: failed to write per-run timing log");
    }

    snprintf(csv_path, sizeof(csv_path), "%s/timings.csv", log_dir);
    int csv_exists = 0;
    struct stat st;
    if (stat(csv_path, &st) == 0 && st.st_size > 0) {
        csv_exists = 1;
    }

    FILE *csv_fp = fopen(csv_path, "a");
    if (csv_fp != NULL) {
        if (!csv_exists) {
            fprintf(csv_fp, "backend,test,backend_elapsed_seconds,total_elapsed_seconds,total_simulations,hit_gates,soft_errors,ser_percent,timestamp\n");
        }
        fprintf(csv_fp, "%s,%s,%.9f,%.9f,%lld,%lld,%lld,%.6f,%s\n",
                backend,
                test_name,
                backend_elapsed_seconds,
                total_elapsed_seconds,
                result.total_simulations,
                result.hit_gates_counter,
                result.soft_error_counter,
                result.soft_error_rate * 100.0,
                timestamp);
        fclose(csv_fp);
    } else {
        perror("Warning: failed to append timing CSV");
    }

    printf("Backend walltime: %.9f seconds\n", backend_elapsed_seconds);
    printf("Total walltime: %.9f seconds\n", total_elapsed_seconds);
    printf("Timing log: %s\n", log_path);
    printf("Timing CSV: %s\n", csv_path);
}

int main(int argc, char *argv[]) {
    const char *backend = "x86";

    if (argc < 4) {
        printf("Usage: %s <path/to/file.v> <nodes.csv> <levels.csv> [--backend legacy|x86|flat|cuda]\n", argv[0]);
        return 1;
    }

    for (int i = 4; i < argc; i++) {
        if (!strcmp(argv[i], "--backend")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: --backend requires legacy, x86, flat, or cuda\n");
                return 1;
            }
            backend = argv[++i];
        } else if (!strncmp(argv[i], "--backend=", 10)) {
            backend = argv[i] + 10;
        } else {
            fprintf(stderr, "Warning: ignoring unrecognized program argument '%s'\n", argv[i]);
        }
    }

    if (!isSupportedBackend(backend)) {
        fprintf(stderr, "Error: unsupported backend '%s'. Use legacy, x86, flat, or cuda.\n", backend);
        return 1;
    }

    printf("Backend: %s\n", backend);

    struct timespec total_start_time;
    clock_gettime(CLOCK_MONOTONIC, &total_start_time);

    char *pathname = argv[1];

    FILE *nodes_fp = fopen(argv[2], "w");
    if (!nodes_fp) {
        perror("Warning: Failed to open nodes CSV file");
    }

    FILE *levels_fp = fopen(argv[3], "w");
    if (!levels_fp) {
        perror("Warning: Failed to open levels CSV file");
    }

    NodesArray *nodes_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    NodesArray *primary_inputs_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    GatesArray *gates_array = (GatesArray*)calloc(1, sizeof(GatesArray));
    LevelsArray *levels_array = (LevelsArray*)calloc(1, sizeof(LevelsArray));
    if (!nodes_array || !primary_inputs_array || !gates_array || !levels_array) {
        exit(1);
    }

    parseVerilogFile(pathname, nodes_array, primary_inputs_array, gates_array);
    levelizeGates(levels_array, gates_array);

    SerResult result;
    struct timespec start_time;
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (strcmp(backend, "flat") == 0) {
        result = runSerFlatCpu(levels_array, gates_array, nodes_array, primary_inputs_array, nodes_fp, levels_fp);
    } else if (strcmp(backend, "cuda") == 0) {
        result = runSerCuda(levels_array, gates_array, nodes_array, primary_inputs_array, nodes_fp, levels_fp);
    } else {
        result = runSerLegacy(levels_array, gates_array, nodes_array, primary_inputs_array, nodes_fp, levels_fp);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double backend_elapsed_seconds = elapsedSeconds(start_time, end_time);

    if (nodes_fp != NULL) fclose(nodes_fp);
    if (levels_fp != NULL) fclose(levels_fp);

    struct timespec total_end_time;
    clock_gettime(CLOCK_MONOTONIC, &total_end_time);
    double total_elapsed_seconds = elapsedSeconds(total_start_time, total_end_time);

    printSerResult(result);
    writeTimingLogs(backend, pathname, result, backend_elapsed_seconds, total_elapsed_seconds);
    freeParsedNetlist(nodes_array, primary_inputs_array, gates_array, levels_array);

    if (strcmp(backend, "cuda") == 0 && result.total_simulations == 0) {
        return 1;
    }

    return 0;
}
