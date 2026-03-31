#include <stdio.h>
#include "netlist.h"
#include "parser.h"


void printGatesArray(GatesArray *gates_array) {
    if (gates_array == NULL || gates_array->data == NULL) {
        printf("Gates array is empty\n");
        return;
    }

    printf("=== Gates Array Dump (Total Gates: %d) ===\n", gates_array->size);

    for (int i = 0; i < gates_array->size; i++) {
        Gate *curr_gate = gates_array->data[i];

        printf("%5d Gate: %-10s | Type: %-2d | Level: %d\n", 
               i, curr_gate->name, curr_gate->type, curr_gate->level);

        printf("      Inputs (%d): ", curr_gate->no_inputs);
        if (curr_gate->inputs != NULL) {
            for (int j = 0; j < curr_gate->no_inputs; j++) {
                if (curr_gate->inputs[j] != NULL) {
                    printf("%s%s", curr_gate->inputs[j]->name, 
                           (j < curr_gate->no_inputs - 1) ? ", " : "");
                } else {
                    printf("(null) ");
                }
            }
        } else {
            printf("None");
        }
        printf("\n");

        printf("      Outputs   : ");
        if (curr_gate->outputs != NULL) {
            if (curr_gate->outputs[0] != NULL) {
                printf("%s", curr_gate->outputs[0]->name);
            }

            if (curr_gate->type == TYPE_DFF && curr_gate->outputs[1] != NULL) {
                printf(", %s", curr_gate->outputs[1]->name);
            }
        } else {
            printf("None");
        }
        printf("\n--------------------------------------------------\n");
    }
}

void printLevelsArray(LevelsArray *levels_array) {
    if (levels_array == NULL || levels_array->data == NULL) {
        printf("Levels array is empty\n");
        return;
    }

    printf("\n=== Levels Array Dump (Total Levels: %d) ===\n", levels_array->size);

    for (int i = 0; i < levels_array->size; i++) {
        GatesArray *curr_level_gates = levels_array->data[i];

        if (curr_level_gates == NULL) {
            printf("Level %2d: (null)\n", i + 1);
            continue;
        }

        printf("Level %2d (Total Gates: %3d) | Gates: ", i + 1, curr_level_gates->size);
        
        if (curr_level_gates->size == 0) {
            printf("None");
        } else {
            for (int j = 0; j < curr_level_gates->size; j++) {
                Gate *curr_gate = curr_level_gates->data[j];
                if (curr_gate != NULL) {
                    printf("%s", curr_gate->name);
                    if (j < curr_level_gates->size - 1) {
                        printf(", ");
                    }
                } else {
                    printf("(null)");
                }
            }
        }
        printf("\n");
    }
    printf("============================================\n\n");
}