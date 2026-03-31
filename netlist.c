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