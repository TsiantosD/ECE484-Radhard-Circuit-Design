#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_H

#include <stdio.h>
#include "netlist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SerResult_t {
    long long total_simulations;
    long long hit_gates_counter;
    long long soft_error_counter;
    double soft_error_rate;
} SerResult;

SerResult runSerLegacy(LevelsArray *levels_array,
                       GatesArray *gates_array,
                       NodesArray *nodes_array,
                       NodesArray *primary_inputs_array,
                       FILE *nodes_fp,
                       FILE *levels_fp);

SerResult runSerFlatCpu(LevelsArray *levels_array,
                        GatesArray *gates_array,
                        NodesArray *nodes_array,
                        NodesArray *primary_inputs_array,
                        FILE *nodes_fp,
                        FILE *levels_fp);

SerResult runSerCuda(LevelsArray *levels_array,
                     GatesArray *gates_array,
                     NodesArray *nodes_array,
                     NodesArray *primary_inputs_array,
                     FILE *nodes_fp,
                     FILE *levels_fp);

void printSerResult(SerResult result);

#ifdef __cplusplus
}
#endif

#endif // ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_H
