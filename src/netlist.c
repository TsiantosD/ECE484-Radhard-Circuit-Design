#include <stdio.h>
#include <string.h>
#include "netlist.h"
#include "parser.h"


void printOutputCsv(long long int input_vector, NodesArray *primary_inputs, NodesArray *nodes) {
    // Print the header exactly once
    if (input_vector == 0) {
        printf("VEC");

        // Print all primary input names
        for (int i = 0; i < primary_inputs->size; i++) {
            printf(",%s", primary_inputs->data[i]->name);
        }

        // Print all internal wire/output names
        for (int i = 0; i < nodes->size; i++) {
            printf(",%s", nodes->data[i]->name);
        }
        printf("\n");
    }

    // Print the values
    printf("%lld", input_vector);
    for (int i = 0; i < primary_inputs->size; i++) {
        printf(",%d", primary_inputs->data[i]->value);
    }
    for (int i = 0; i < nodes->size; i++) {
        printf(",%d", nodes->data[i]->value);
    }
    printf("\n");
}

void printLevelsArrayStateCsv(LevelsArray *levels_array, long long int input_vector) {
    if (levels_array == NULL || levels_array->data == NULL) {
        if (input_vector == 0) {
            printf("ERROR,Levels array is empty or uninitialized.\n");
        }
        return;
    }

    if (input_vector == 0) {
        printf("VECTOR,LEVEL,GATE_NAME,GATE_TYPE,INPUTS(name=val),OUTPUTS(name=val)\n");
    }

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

            char inputs_buf[256] = ""; 
            if (curr_gate->inputs != NULL && curr_gate->no_inputs > 0) {
                for (int k = 0; k < curr_gate->no_inputs; k++) {
                    if (curr_gate->inputs[k] != NULL) {
                        char temp[32];
                        snprintf(temp, sizeof(temp), "%s=%d%s", 
                                 curr_gate->inputs[k]->name, 
                                 curr_gate->inputs[k]->value,
                                 (k < curr_gate->no_inputs - 1) ? " " : ""); 
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
                    snprintf(temp, sizeof(temp), " %s=%d", curr_gate->outputs[1]->name, curr_gate->outputs[1]->value);
                    strcat(outputs_buf, temp);
                }
            } 
            
            if (strlen(outputs_buf) == 0) {
                strcpy(outputs_buf, "NONE");
            }

            printf("%lld,%d,%s,%d,%s,%s\n", 
                   input_vector,
                   i,                      
                   curr_gate->name, 
                   curr_gate->type, 
                   inputs_buf,
                   outputs_buf);
        }
    }
}

void printLevelsArray(LevelsArray *levels_array) {
    if (levels_array == NULL || levels_array->data == NULL) {
        printf("ERROR,Levels array is empty or uninitialized.\n");
        return;
    }

    printf("LEVEL,GATE_NAME,GATE_TYPE,INPUTS(name=val),OUTPUTS(name=val)\n");

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

            char inputs_buf[256] = ""; 
            if (curr_gate->inputs != NULL && curr_gate->no_inputs > 0) {
                for (int k = 0; k < curr_gate->no_inputs; k++) {
                    if (curr_gate->inputs[k] != NULL) {
                        char temp[32];
                        // Notice the space " " instead of "," for separation
                        snprintf(temp, sizeof(temp), "%s=%d%s", 
                                 curr_gate->inputs[k]->name, 
                                 curr_gate->inputs[k]->value,
                                 (k < curr_gate->no_inputs - 1) ? " " : ""); 
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
                    snprintf(temp, sizeof(temp), " %s=%d", curr_gate->outputs[1]->name, curr_gate->outputs[1]->value);
                    strcat(outputs_buf, temp);
                }
            } 
            
            if (strlen(outputs_buf) == 0) {
                strcpy(outputs_buf, "NONE");
            }

            printf("%d,%s,%d,%s,%s\n", 
                   i,                      
                   curr_gate->name, 
                   curr_gate->type, 
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
