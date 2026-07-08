#include <stdio.h>
#include "backend.h"
#include "backend_cuda.h"

SerResult runSerCuda(LevelsArray *levels_array,
                     GatesArray *gates_array,
                     NodesArray *nodes_array,
                     NodesArray *primary_inputs_array,
                     FILE *nodes_fp,
                     FILE *levels_fp) {
    (void)nodes_fp;
    (void)levels_fp;

    fprintf(stderr,
            "CUDA backend prototype: using flattened netlist and GPU SER kernel. "
            "Per-vector CSV output is intentionally disabled for CUDA runs.\n");

    return runSerCudaFlatPrototype(levels_array, gates_array, nodes_array, primary_inputs_array);
}
