#include <stdlib.h>
#include <string.h>
#include "backend.h"
#include "flat_netlist.h"
#include "parser.h"

static int simulateFlatGate(const FlatNetlist *flat, const int *values, int gate_index) {
    int offset = flat->gate_input_offset[gate_index];
    int count = flat->gate_input_count[gate_index];

    switch (flat->gate_type[gate_index]) {
        case TYPE_AND:
            for (int i = 0; i < count; i++) {
                if (values[flat->gate_inputs_flat[offset + i]] == 0) return 0;
            }
            return 1;
        case TYPE_OR:
            for (int i = 0; i < count; i++) {
                if (values[flat->gate_inputs_flat[offset + i]] == 1) return 1;
            }
            return 0;
        case TYPE_INV:
            return !values[flat->gate_inputs_flat[offset]];
        case TYPE_NAND:
            for (int i = 0; i < count; i++) {
                if (values[flat->gate_inputs_flat[offset + i]] == 0) return 1;
            }
            return 0;
        case TYPE_NOR:
            for (int i = 0; i < count; i++) {
                if (values[flat->gate_inputs_flat[offset + i]] == 1) return 0;
            }
            return 1;
        default:
            return 0;
    }
}

static void resetAndApplyInputVector(const FlatNetlist *flat, int *values, long long input_vector) {
    memcpy(values, flat->initial_node_value, flat->num_nodes * sizeof(int));

    long long tmp_input_vector = input_vector;
    for (int i = 0; i < flat->num_primary_inputs; i++) {
        values[flat->primary_input_nodes[i]] = tmp_input_vector & 1LL;
        tmp_input_vector = tmp_input_vector >> 1;
    }
}

static void simulateFlatCircuit(const FlatNetlist *flat, int *values, int hit_gate_index) {
    for (int level = 0; level < flat->num_levels; level++) {
        int offset = flat->level_gate_offset[level];
        int count = flat->level_gate_count[level];

        for (int j = 0; j < count; j++) {
            int gate_index = flat->level_gates_flat[offset + j];
            int output_node = flat->gate_output_node[gate_index];
            values[output_node] = simulateFlatGate(flat, values, gate_index);

            if (gate_index == hit_gate_index) {
                values[output_node] = !values[output_node];
            }
        }
    }
}

static void printFlatNodesCurrentState(FILE *fp, const FlatNetlist *flat, const int *values, long long input_vector) {
    if (fp == NULL) return;

    if (input_vector == 0) {
        fprintf(fp, "VEC");
        for (int i = 0; i < flat->num_primary_inputs; i++) {
            fprintf(fp, ",%s", flat->node_names[flat->primary_input_nodes[i]]);
        }
        for (int i = 0; i < flat->num_non_primary_nodes; i++) {
            fprintf(fp, ",%s", flat->node_names[flat->non_primary_nodes[i]]);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "%lld", input_vector);
    for (int i = 0; i < flat->num_primary_inputs; i++) {
        fprintf(fp, ",%d", values[flat->primary_input_nodes[i]]);
    }
    for (int i = 0; i < flat->num_non_primary_nodes; i++) {
        fprintf(fp, ",%d", values[flat->non_primary_nodes[i]]);
    }
    fprintf(fp, "\n");
}

static void printFlatLevelsHeader(FILE *fp) {
    if (fp != NULL) {
        fprintf(fp, "VECTOR,LEVEL,GATE_INDEX,GATE_TYPE,OUTPUT_NODE,OUTPUT_VALUE\n");
    }
}

SerResult runSerFlatCpu(LevelsArray *levels_array,
                        GatesArray *gates_array,
                        NodesArray *nodes_array,
                        NodesArray *primary_inputs_array,
                        FILE *nodes_fp,
                        FILE *levels_fp) {
    (void)levels_array;
    SerResult result = {0};
    FlatNetlist *flat = flattenNetlist(nodes_array, primary_inputs_array, gates_array, levels_array);

    int *values = calloc(flat->num_nodes, sizeof(int));
    int *golden_dff_values = calloc(flat->num_dff_inputs, sizeof(int));
    if ((flat->num_nodes > 0 && values == NULL) ||
        (flat->num_dff_inputs > 0 && golden_dff_values == NULL)) {
        exit(1);
    }

    result.hit_gates_counter = flat->num_hittable_gates;
    long long max_vectors = 1LL << flat->num_primary_inputs;
    printFlatLevelsHeader(levels_fp);

    for (long long input_vector = 0; input_vector < max_vectors; input_vector++) {
        resetAndApplyInputVector(flat, values, input_vector);
        simulateFlatCircuit(flat, values, -1);
        printFlatNodesCurrentState(nodes_fp, flat, values, input_vector);

        for (int i = 0; i < flat->num_dff_inputs; i++) {
            golden_dff_values[i] = values[flat->dff_input_nodes[i]];
        }

        for (int hit = 0; hit < flat->num_hittable_gates; hit++) {
            int hit_gate_index = flat->hittable_gates[hit];
            resetAndApplyInputVector(flat, values, input_vector);
            simulateFlatCircuit(flat, values, hit_gate_index);

            for (int i = 0; i < flat->num_dff_inputs; i++) {
                if (values[flat->dff_input_nodes[i]] != golden_dff_values[i]) {
                    result.soft_error_counter++;
                    break;
                }
            }
        }
    }

    result.total_simulations = max_vectors * result.hit_gates_counter;
    if (result.total_simulations > 0) {
        result.soft_error_rate = (double)result.soft_error_counter / (double)result.total_simulations;
    }

    free(values);
    free(golden_dff_values);
    freeFlatNetlist(flat);
    return result;
}
