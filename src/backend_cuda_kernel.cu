#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend_cuda.h"
#include "flat_netlist.h"
#include "parser.h"

static void cudaAbort(const char *file, int line, cudaError_t err) {
    fprintf(stderr, "CUDA error %s:%d: %s\n", file, line, cudaGetErrorString(err));
    cudaDeviceReset();
    exit(1);
}

#define CUDA_CHECK(call) do { \
    cudaError_t err__ = (call); \
    if (err__ != cudaSuccess) { \
        cudaAbort(__FILE__, __LINE__, err__); \
    } \
} while (0)

typedef struct DeviceFlatNetlist_t {
    int num_nodes;
    int num_primary_inputs;
    int num_gates;
    int num_levels;
    int num_dff_inputs;
    int num_hittable_gates;

    int *initial_node_value;
    int *primary_input_nodes;
    int *dff_input_nodes;
    int *hittable_gates;
    int *gate_type;
    int *gate_input_offset;
    int *gate_input_count;
    int *gate_output_node;
    int *gate_inputs_flat;
    int *level_gate_offset;
    int *level_gate_count;
    int *level_gates_flat;
} DeviceFlatNetlist;

__device__ static int simulateCudaGate(const DeviceFlatNetlist flat, const int *values, int gate_index) {
    int offset = flat.gate_input_offset[gate_index];
    int count = flat.gate_input_count[gate_index];

    switch (flat.gate_type[gate_index]) {
        case TYPE_AND:
            for (int i = 0; i < count; i++) {
                if (values[flat.gate_inputs_flat[offset + i]] == 0) return 0;
            }
            return 1;
        case TYPE_OR:
            for (int i = 0; i < count; i++) {
                if (values[flat.gate_inputs_flat[offset + i]] == 1) return 1;
            }
            return 0;
        case TYPE_INV:
            return !values[flat.gate_inputs_flat[offset]];
        case TYPE_NAND:
            for (int i = 0; i < count; i++) {
                if (values[flat.gate_inputs_flat[offset + i]] == 0) return 1;
            }
            return 0;
        case TYPE_NOR:
            for (int i = 0; i < count; i++) {
                if (values[flat.gate_inputs_flat[offset + i]] == 1) return 0;
            }
            return 1;
        default:
            return 0;
    }
}

__device__ static void resetCudaValues(const DeviceFlatNetlist flat, int *values, long long input_vector) {
    for (int i = 0; i < flat.num_nodes; i++) {
        values[i] = flat.initial_node_value[i];
    }

    long long tmp_input_vector = input_vector;
    for (int i = 0; i < flat.num_primary_inputs; i++) {
        values[flat.primary_input_nodes[i]] = tmp_input_vector & 1LL;
        tmp_input_vector >>= 1;
    }
}

__device__ static void simulateCudaCircuit(const DeviceFlatNetlist flat, int *values, int hit_gate_index) {
    for (int level = 0; level < flat.num_levels; level++) {
        int offset = flat.level_gate_offset[level];
        int count = flat.level_gate_count[level];

        for (int j = 0; j < count; j++) {
            int gate_index = flat.level_gates_flat[offset + j];
            int output_node = flat.gate_output_node[gate_index];
            values[output_node] = simulateCudaGate(flat, values, gate_index);
            if (gate_index == hit_gate_index) {
                values[output_node] = !values[output_node];
            }
        }
    }
}

__global__ static void serKernel(DeviceFlatNetlist flat, long long max_vectors, int *case_errors) {
    long long total_cases = max_vectors * (long long)flat.num_hittable_gates;
    long long case_index = blockIdx.x * blockDim.x + threadIdx.x;
    long long stride = (long long)blockDim.x * (long long)gridDim.x;

    extern __shared__ int shared_values[];
    int per_thread_ints = flat.num_nodes + flat.num_dff_inputs;
    int *values = shared_values + threadIdx.x * per_thread_ints;
    int *golden_values = values + flat.num_nodes;

    for (; case_index < total_cases; case_index += stride) {
        int hit_index = case_index % flat.num_hittable_gates;
        long long input_vector = case_index / flat.num_hittable_gates;
        int hit_gate_index = flat.hittable_gates[hit_index];

        resetCudaValues(flat, values, input_vector);
        simulateCudaCircuit(flat, values, -1);
        for (int i = 0; i < flat.num_dff_inputs; i++) {
            golden_values[i] = values[flat.dff_input_nodes[i]];
        }

        resetCudaValues(flat, values, input_vector);
        simulateCudaCircuit(flat, values, hit_gate_index);

        int has_error = 0;
        for (int i = 0; i < flat.num_dff_inputs; i++) {
            if (values[flat.dff_input_nodes[i]] != golden_values[i]) {
                has_error = 1;
                break;
            }
        }
        case_errors[case_index] = has_error;
    }
}

static int *copyIntArrayToDevice(const int *host, size_t count) {
    int *device = NULL;
    if (count == 0) return NULL;
    CUDA_CHECK(cudaMalloc((void**)&device, count * sizeof(int)));
    CUDA_CHECK(cudaMemcpy(device, host, count * sizeof(int), cudaMemcpyHostToDevice));
    return device;
}

static void freeDeviceFlat(DeviceFlatNetlist *device_flat) {
    cudaFree(device_flat->initial_node_value);
    cudaFree(device_flat->primary_input_nodes);
    cudaFree(device_flat->dff_input_nodes);
    cudaFree(device_flat->hittable_gates);
    cudaFree(device_flat->gate_type);
    cudaFree(device_flat->gate_input_offset);
    cudaFree(device_flat->gate_input_count);
    cudaFree(device_flat->gate_output_node);
    cudaFree(device_flat->gate_inputs_flat);
    cudaFree(device_flat->level_gate_offset);
    cudaFree(device_flat->level_gate_count);
    cudaFree(device_flat->level_gates_flat);
}

SerResult runSerCudaFlatPrototype(LevelsArray *levels_array,
                                  GatesArray *gates_array,
                                  NodesArray *nodes_array,
                                  NodesArray *primary_inputs_array) {
    SerResult result = {0};
    FlatNetlist *flat = flattenNetlist(nodes_array, primary_inputs_array, gates_array, levels_array);

    result.hit_gates_counter = flat->num_hittable_gates;
    long long max_vectors = 1LL << flat->num_primary_inputs;
    result.total_simulations = max_vectors * result.hit_gates_counter;

    if (result.total_simulations == 0) {
        freeFlatNetlist(flat);
        return result;
    }

    DeviceFlatNetlist device_flat = {0};
    device_flat.num_nodes = flat->num_nodes;
    device_flat.num_primary_inputs = flat->num_primary_inputs;
    device_flat.num_gates = flat->num_gates;
    device_flat.num_levels = flat->num_levels;
    device_flat.num_dff_inputs = flat->num_dff_inputs;
    device_flat.num_hittable_gates = flat->num_hittable_gates;
    device_flat.initial_node_value = copyIntArrayToDevice(flat->initial_node_value, flat->num_nodes);
    device_flat.primary_input_nodes = copyIntArrayToDevice(flat->primary_input_nodes, flat->num_primary_inputs);
    device_flat.dff_input_nodes = copyIntArrayToDevice(flat->dff_input_nodes, flat->num_dff_inputs);
    device_flat.hittable_gates = copyIntArrayToDevice(flat->hittable_gates, flat->num_hittable_gates);
    device_flat.gate_type = copyIntArrayToDevice(flat->gate_type, flat->num_gates);
    device_flat.gate_input_offset = copyIntArrayToDevice(flat->gate_input_offset, flat->num_gates);
    device_flat.gate_input_count = copyIntArrayToDevice(flat->gate_input_count, flat->num_gates);
    device_flat.gate_output_node = copyIntArrayToDevice(flat->gate_output_node, flat->num_gates);
    device_flat.gate_inputs_flat = copyIntArrayToDevice(flat->gate_inputs_flat, flat->num_gate_inputs);
    device_flat.level_gate_offset = copyIntArrayToDevice(flat->level_gate_offset, flat->num_levels);
    device_flat.level_gate_count = copyIntArrayToDevice(flat->level_gate_count, flat->num_levels);

    int total_level_gates = 0;
    for (int i = 0; i < flat->num_levels; i++) {
        total_level_gates += flat->level_gate_count[i];
    }
    device_flat.level_gates_flat = copyIntArrayToDevice(flat->level_gates_flat, total_level_gates);

    int *device_case_errors = NULL;
    int *host_case_errors = (int*)calloc(result.total_simulations, sizeof(int));
    if (host_case_errors == NULL) {
        fprintf(stderr, "Failed to allocate host CUDA result buffer.\n");
        cudaDeviceReset();
        exit(1);
    }
    CUDA_CHECK(cudaMalloc((void**)&device_case_errors, result.total_simulations * sizeof(int)));

    int per_thread_ints = flat->num_nodes + flat->num_dff_inputs;
    int shared_limit_bytes = 48 * 1024;
    CUDA_CHECK(cudaDeviceGetAttribute(&shared_limit_bytes, cudaDevAttrMaxSharedMemoryPerBlock, 0));

    int threads_per_block = 128;
    size_t per_thread_shared_bytes = (size_t)per_thread_ints * sizeof(int);
    if (per_thread_shared_bytes * threads_per_block > (size_t)shared_limit_bytes) {
        threads_per_block = (int)((size_t)shared_limit_bytes / per_thread_shared_bytes);
        if (threads_per_block < 1) {
            fprintf(stderr,
                    "CUDA backend cannot run this circuit with the prototype shared-memory kernel: "
                    "%zu bytes/thread required, %d bytes/block available.\n",
                    per_thread_shared_bytes, shared_limit_bytes);
            cudaDeviceReset();
            exit(1);
        }
    }

    int blocks = (int)((result.total_simulations + threads_per_block - 1) / threads_per_block);
    int max_grid_x = 65535;
    CUDA_CHECK(cudaDeviceGetAttribute(&max_grid_x, cudaDevAttrMaxGridDimX, 0));
    if (blocks > max_grid_x) {
        blocks = max_grid_x;
    }
    size_t shared_bytes = (size_t)threads_per_block * per_thread_shared_bytes;

    serKernel<<<blocks, threads_per_block, shared_bytes>>>(device_flat, max_vectors, device_case_errors);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    CUDA_CHECK(cudaMemcpy(host_case_errors, device_case_errors,
                          result.total_simulations * sizeof(int), cudaMemcpyDeviceToHost));

    for (long long i = 0; i < result.total_simulations; i++) {
        result.soft_error_counter += host_case_errors[i];
    }
    result.soft_error_rate = (double)result.soft_error_counter / (double)result.total_simulations;

    free(host_case_errors);
    cudaFree(device_case_errors);
    freeDeviceFlat(&device_flat);
    CUDA_CHECK(cudaDeviceReset());
    freeFlatNetlist(flat);
    return result;
}
