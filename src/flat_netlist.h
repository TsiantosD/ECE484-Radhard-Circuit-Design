#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_FLAT_NETLIST_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_FLAT_NETLIST_H

#include "netlist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlatNetlist_t {
    int num_nodes;
    int num_primary_inputs;
    int num_non_primary_nodes;
    int num_gates;
    int num_levels;
    int num_gate_inputs;
    int num_dff_inputs;
    int num_hittable_gates;

    char (*node_names)[20];
    int *node_type;
    int *initial_node_value;

    int *primary_input_nodes;
    int *non_primary_nodes;
    int *dff_input_nodes;
    int *hittable_gates;

    int *gate_type;
    int *gate_level;
    int *gate_input_offset;
    int *gate_input_count;
    int *gate_output_node;
    int *gate_inputs_flat;

    int *level_gate_offset;
    int *level_gate_count;
    int *level_gates_flat;
} FlatNetlist;

FlatNetlist *flattenNetlist(NodesArray *nodes_array,
                            NodesArray *primary_inputs_array,
                            GatesArray *gates_array,
                            LevelsArray *levels_array);

void freeFlatNetlist(FlatNetlist *flat);

#ifdef __cplusplus
}
#endif

#endif // ECE484_RADHARD_CIRCUIT_DESIGN_FLAT_NETLIST_H
