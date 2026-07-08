#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_CUDA_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_CUDA_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

SerResult runSerCudaFlatPrototype(LevelsArray *levels_array,
                                  GatesArray *gates_array,
                                  NodesArray *nodes_array,
                                  NodesArray *primary_inputs_array);

#ifdef __cplusplus
}
#endif

#endif // ECE484_RADHARD_CIRCUIT_DESIGN_BACKEND_CUDA_H
