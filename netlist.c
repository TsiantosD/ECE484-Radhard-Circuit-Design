#include <stdio.h>
#include <string.h>
#include "netlist.h"
#include "parser.h"


void printLevelsArray(LevelsArray *levels_array) {
    if (levels_array == NULL || levels_array->data == NULL) {
        printf("ERROR    Levels array is empty or uninitialized.\n");
        return;
    }

    printf("%-8s %-7s %-12s %-11s %-10s %-30s %s\n", 
           "HEADER", "LEVEL", "GATE_NAME", "GATE_TYPE", "GATE_VAL", "INPUTS", "OUTPUTS");
    
    for (int i = 0; i < levels_array->size; i++) {
        GatesArray *curr_level_gates = levels_array->data[i];

        if (curr_level_gates == NULL || curr_level_gates->data == NULL) {
            continue;
        }

        for (int j = 0; j < curr_level_gates->size; j++) {
            Gate *curr_gate = curr_level_gates->data[j];

            if (curr_gate == NULL) {
                continue;
            }

            char inputs_buf[256] = ""; // Large enough to hold many pins safely
            if (curr_gate->inputs != NULL && curr_gate->no_inputs > 0) {
                for (int k = 0; k < curr_gate->no_inputs; k++) {
                    if (curr_gate->inputs[k] != NULL) {
                        char temp[32];
                        snprintf(temp, sizeof(temp), "%s=%d%s", 
                                 curr_gate->inputs[k]->name, 
                                 curr_gate->inputs[k]->value,
                                 (k < curr_gate->no_inputs - 1) ? "," : ""); 
                        strcat(inputs_buf, temp);
                    }
                }
            } else {
                strcpy(inputs_buf, "NONE");
            }

            char outputs_buf[128] = "";
            if (curr_gate->outputs != NULL) {
                if (curr_gate->outputs[0] != NULL) {
                    char temp[32];
                    snprintf(temp, sizeof(temp), "%s=%d", curr_gate->outputs[0]->name, curr_gate->outputs[0]->value);
                    strcat(outputs_buf, temp);
                }

                if (curr_gate->type == TYPE_DFF && curr_gate->outputs[1] != NULL) {
                    char temp[32];
                    snprintf(temp, sizeof(temp), ",%s=%d", curr_gate->outputs[1]->name, curr_gate->outputs[1]->value);
                    strcat(outputs_buf, temp);
                }
            } 
            
            if (strlen(outputs_buf) == 0) {
                strcpy(outputs_buf, "NONE");
            }

            printf("%-8s %-7d %-12s %-11d %-10d %-30s %s\n", 
                   "DATA",
                   i,                      
                   curr_gate->name, 
                   curr_gate->type, 
                   curr_gate->value,
                   inputs_buf,
                   outputs_buf);
        }
    }
}

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
