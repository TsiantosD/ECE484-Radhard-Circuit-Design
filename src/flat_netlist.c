#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flat_netlist.h"
#include "parser.h"

static int checkedNodeIndex(Node *node,
                            NodesArray *nodes_array,
                            NodesArray *primary_inputs_array) {
    if (node == NULL) {
        fprintf(stderr, "Error: cannot flatten NULL node reference\n");
        exit(1);
    }

    for (int i = 0; i < primary_inputs_array->size; i++) {
        if (primary_inputs_array->data[i] == node) {
            return i;
        }
    }

    for (int i = 0; i < nodes_array->size; i++) {
        if (nodes_array->data[i] == node) {
            return primary_inputs_array->size + i;
        }
    }

    fprintf(stderr, "Error: node '%s' was not found while flattening\n", node->name);
    exit(1);
}

static int checkedGateIndex(Gate *gate, GatesArray *gates_array) {
    if (gate == NULL) {
        fprintf(stderr, "Error: cannot flatten NULL gate reference\n");
        exit(1);
    }

    for (int i = 0; i < gates_array->size; i++) {
        if (gates_array->data[i] == gate) {
            return i;
        }
    }

    fprintf(stderr, "Error: gate '%s' was not found while flattening\n", gate->name);
    exit(1);
}

FlatNetlist *flattenNetlist(NodesArray *nodes_array,
                            NodesArray *primary_inputs_array,
                            GatesArray *gates_array,
                            LevelsArray *levels_array) {
    FlatNetlist *flat = (FlatNetlist*)calloc(1, sizeof(FlatNetlist));
    if (flat == NULL) {
        exit(1);
    }

    flat->num_primary_inputs = primary_inputs_array->size;
    flat->num_non_primary_nodes = nodes_array->size;
    flat->num_nodes = primary_inputs_array->size + nodes_array->size;
    flat->num_gates = gates_array->size;
    flat->num_levels = levels_array->size;

    flat->node_names = calloc(flat->num_nodes, sizeof(*flat->node_names));
    flat->node_type = calloc(flat->num_nodes, sizeof(int));
    flat->initial_node_value = calloc(flat->num_nodes, sizeof(int));
    flat->primary_input_nodes = calloc(flat->num_primary_inputs, sizeof(int));
    flat->non_primary_nodes = calloc(flat->num_non_primary_nodes, sizeof(int));
    flat->gate_type = calloc(flat->num_gates, sizeof(int));
    flat->gate_level = calloc(flat->num_gates, sizeof(int));
    flat->gate_input_offset = calloc(flat->num_gates, sizeof(int));
    flat->gate_input_count = calloc(flat->num_gates, sizeof(int));
    flat->gate_output_node = calloc(flat->num_gates, sizeof(int));
    flat->level_gate_offset = calloc(flat->num_levels, sizeof(int));
    flat->level_gate_count = calloc(flat->num_levels, sizeof(int));

    if (!flat->node_names || !flat->node_type || !flat->initial_node_value ||
        !flat->primary_input_nodes || !flat->non_primary_nodes ||
        !flat->gate_type || !flat->gate_level || !flat->gate_input_offset ||
        !flat->gate_input_count || !flat->gate_output_node ||
        !flat->level_gate_offset || !flat->level_gate_count) {
        exit(1);
    }

    for (int i = 0; i < primary_inputs_array->size; i++) {
        Node *node = primary_inputs_array->data[i];
        strncpy(flat->node_names[i], node->name, 20);
        flat->node_names[i][19] = '\0';
        flat->node_type[i] = node->type;
        flat->initial_node_value[i] = node->value;
        flat->primary_input_nodes[i] = i;
    }

    for (int i = 0; i < nodes_array->size; i++) {
        int flat_index = primary_inputs_array->size + i;
        Node *node = nodes_array->data[i];
        strncpy(flat->node_names[flat_index], node->name, 20);
        flat->node_names[flat_index][19] = '\0';
        flat->node_type[flat_index] = node->type;
        flat->initial_node_value[flat_index] = node->value;
        flat->non_primary_nodes[i] = flat_index;

        if (node->is_ff_input == 1) {
            flat->num_dff_inputs++;
        }
    }

    flat->dff_input_nodes = calloc(flat->num_dff_inputs, sizeof(int));
    if (flat->num_dff_inputs > 0 && flat->dff_input_nodes == NULL) {
        exit(1);
    }

    for (int i = 0, c = 0; i < nodes_array->size; i++) {
        Node *node = nodes_array->data[i];
        if (node->is_ff_input == 1) {
            flat->dff_input_nodes[c++] = primary_inputs_array->size + i;
        }
    }

    for (int i = 0; i < gates_array->size; i++) {
        Gate *gate = gates_array->data[i];
        flat->num_gate_inputs += gate->no_inputs;
        if (gate->type != TYPE_DFF && gate->outputs[0]->is_ff_input != 1 && gate->outputs[0]->type != TYPE_OUTPUT) {
            flat->num_hittable_gates++;
        }
    }

    flat->gate_inputs_flat = calloc(flat->num_gate_inputs, sizeof(int));
    flat->hittable_gates = calloc(flat->num_hittable_gates, sizeof(int));
    if ((flat->num_gate_inputs > 0 && flat->gate_inputs_flat == NULL) ||
        (flat->num_hittable_gates > 0 && flat->hittable_gates == NULL)) {
        exit(1);
    }

    for (int i = 0, input_offset = 0, hit_index = 0; i < gates_array->size; i++) {
        Gate *gate = gates_array->data[i];
        flat->gate_type[i] = gate->type;
        flat->gate_level[i] = gate->level;
        flat->gate_input_offset[i] = input_offset;
        flat->gate_input_count[i] = gate->no_inputs;
        flat->gate_output_node[i] = (gate->outputs != NULL && gate->outputs[0] != NULL)
                                      ? checkedNodeIndex(gate->outputs[0], nodes_array, primary_inputs_array)
                                      : -1;

        for (int j = 0; j < gate->no_inputs; j++) {
            flat->gate_inputs_flat[input_offset++] = checkedNodeIndex(gate->inputs[j], nodes_array, primary_inputs_array);
        }

        if (gate->type != TYPE_DFF && gate->outputs[0]->is_ff_input != 1 && gate->outputs[0]->type != TYPE_OUTPUT) {
            flat->hittable_gates[hit_index++] = i;
        }
    }

    int total_level_gates = 0;
    for (int i = 0; i < levels_array->size; i++) {
        flat->level_gate_offset[i] = total_level_gates;
        flat->level_gate_count[i] = levels_array->data[i]->size;
        total_level_gates += levels_array->data[i]->size;
    }

    flat->level_gates_flat = calloc(total_level_gates, sizeof(int));
    if (total_level_gates > 0 && flat->level_gates_flat == NULL) {
        exit(1);
    }

    for (int i = 0, offset = 0; i < levels_array->size; i++) {
        GatesArray *level = levels_array->data[i];
        for (int j = 0; j < level->size; j++) {
            flat->level_gates_flat[offset++] = checkedGateIndex(level->data[j], gates_array);
        }
    }

    return flat;
}

void freeFlatNetlist(FlatNetlist *flat) {
    if (flat == NULL) return;

    free(flat->node_names);
    free(flat->node_type);
    free(flat->initial_node_value);
    free(flat->primary_input_nodes);
    free(flat->non_primary_nodes);
    free(flat->dff_input_nodes);
    free(flat->hittable_gates);
    free(flat->gate_type);
    free(flat->gate_level);
    free(flat->gate_input_offset);
    free(flat->gate_input_count);
    free(flat->gate_output_node);
    free(flat->gate_inputs_flat);
    free(flat->level_gate_offset);
    free(flat->level_gate_count);
    free(flat->level_gates_flat);
    free(flat);
}
