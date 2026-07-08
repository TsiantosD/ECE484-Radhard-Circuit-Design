#include <stdio.h>
#include "backend.h"

SerResult runSerCuda(LevelsArray *levels_array,
                     GatesArray *gates_array,
                     NodesArray *nodes_array,
                     NodesArray *primary_inputs_array,
                     FILE *nodes_fp,
                     FILE *levels_fp) {
    (void)levels_array;
    (void)gates_array;
    (void)nodes_array;
    (void)primary_inputs_array;
    (void)nodes_fp;
    (void)levels_fp;

    fprintf(stderr,
            "CUDA backend was selected, but this binary was built without CUDA support. "
            "Build on a CUDA system with: make -C src BACKEND=cuda\n");
    SerResult result = {0};
    return result;
}
