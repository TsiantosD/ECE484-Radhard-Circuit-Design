#include <stdio.h>
#include <stdlib.h>
#include "levelization.h"
#include "parser.h"
#include "netlist.h"


void levelizeGates(LevelsArray *levels_array, GatesArray* gates_array) {
    int unmarked_gates_exist = 1;

    while (unmarked_gates_exist) {
        unmarked_gates_exist = 0;

        for (int i = 0; i < gates_array->size; i++) {
            Gate *curr_gate = gates_array->data[i];

            // Skip the DFFs
            if (curr_gate->type == TYPE_DFF) {
                continue;
            }

            // the level of the gate has been calculated
            if (curr_gate->level != -1) {
                continue;
            }

            int should_update = 1;
            int max_val = -1;

            for (int j = 0; j < curr_gate->no_inputs; j++) {
                Node *curr_node = curr_gate->inputs[j];

                // The gate's level cannot be determined yet, it has an input of
                // a higher level node which is not yet determined.
                if (curr_node->level == -1) {
                    should_update = 0;
                    unmarked_gates_exist = 1;
                    break;
                }

                if (curr_node->level > max_val) {
                    max_val = curr_node->level;
                }
            }

            // The gate's level is determined, mark it and add the gate to the related array
            if (should_update) {
                curr_gate->level = max_val + 1;

                // set the level of the gate output 
                if (curr_gate->type != TYPE_DFF) {
                    curr_gate->outputs[0]->level = curr_gate->level;
                } 

                // Expand the levels array when needed
                if (curr_gate->level > levels_array->size) {
                    levels_array->data = (GatesArray**)realloc(levels_array->data, curr_gate->level * sizeof(GatesArray*));
                    if (levels_array->data == NULL) {
                        exit(1);
                    }
                    levels_array->size = curr_gate->level;
                    levels_array->data[curr_gate->level - 1] = (GatesArray*)calloc(1, sizeof(GatesArray));
                    if (levels_array->data[curr_gate->level - 1] == NULL) {
                        exit(1);
                    }
                    levels_array->data[curr_gate->level - 1]->size = 0;
                }

                // Add the gate to the level array
                GatesArray *curr_level = levels_array->data[curr_gate->level - 1];

                curr_level->data = (Gate**)realloc(curr_level->data, (curr_level->size + 1) * sizeof(Gate*));
                if (curr_level->data == NULL) {
                    exit(1);
                }
                curr_level->data[curr_level->size] = curr_gate;
                curr_level->size++;
            }
        }
    }

    return;
}
