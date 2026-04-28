#include <stdio.h>
#include <string.h>
#include "netlist.h"
#include "parser.h"


void printNodesCurrentState(FILE *fp, long long int input_vector, NodesArray *primary_inputs, NodesArray *nodes) {
    if (fp == NULL) return;

    // Print the header exactly once
    if (input_vector == 0) {
        fprintf(fp, "VEC");

        // Print all primary input names
        for (int i = 0; i < primary_inputs->size; i++) {
            char clean_name[128];
            strncpy(clean_name, primary_inputs->data[i]->name, sizeof(clean_name) - 1);
            clean_name[sizeof(clean_name) - 1] = '\0';
            clean_name[strcspn(clean_name, "\r\n")] = 0; 
            
            if (strlen(clean_name) == 0) continue;
            
            fprintf(fp, ",%s", clean_name);
        }

        // Print all internal wire/output names
        for (int i = 0; i < nodes->size; i++) {
            char clean_name[128];
            strncpy(clean_name, nodes->data[i]->name, sizeof(clean_name) - 1);
            clean_name[sizeof(clean_name) - 1] = '\0';
            clean_name[strcspn(clean_name, "\r\n")] = 0; 
            
            if (strlen(clean_name) == 0) continue;
            
            fprintf(fp, ",%s", clean_name);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "%lld", input_vector);
    for (int i = 0; i < primary_inputs->size; i++) {
        char clean_name[128];
        strncpy(clean_name, primary_inputs->data[i]->name, sizeof(clean_name) - 1);
        clean_name[sizeof(clean_name) - 1] = '\0';
        clean_name[strcspn(clean_name, "\r\n")] = 0;
        
        if (strlen(clean_name) == 0) continue; 
        
        fprintf(fp, ",%d", primary_inputs->data[i]->value);
    }
    
    for (int i = 0; i < nodes->size; i++) {
        char clean_name[128];
        strncpy(clean_name, nodes->data[i]->name, sizeof(clean_name) - 1);
        clean_name[sizeof(clean_name) - 1] = '\0';
        clean_name[strcspn(clean_name, "\r\n")] = 0;
        
        if (strlen(clean_name) == 0) continue; 
        
        fprintf(fp, ",%d", nodes->data[i]->value);
    }
    fprintf(fp, "\n");
}

void printLevelsArrayStateCsv(FILE *fp, LevelsArray *levels_array, GatesArray *gates_array, long long int input_vector) {
    if (fp == NULL) return;

    if (levels_array == NULL || levels_array->data == NULL) {
        if (input_vector == 0) {
            fprintf(stderr, "ERROR,Levels array is empty or uninitialized.\n");
        }
        return;
    }

    if (input_vector == 0) {
        fprintf(fp, "VECTOR,LEVEL,GATE_NAME,GATE_TYPE,INPUTS(name=val),OUTPUTS(name=val)\n");
    }

    if (gates_array != NULL && gates_array->data != NULL) {
        for (int i = 0; i < gates_array->size; i++) {
            Gate *curr_gate = gates_array->data[i];

            if (curr_gate != NULL && curr_gate->type == TYPE_DFF) {
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

                    if (curr_gate->outputs[1] != NULL) {
                        char temp[32];
                        snprintf(temp, sizeof(temp), " %s=%d", curr_gate->outputs[1]->name, curr_gate->outputs[1]->value);
                        strcat(outputs_buf, temp);
                    }
                } 
                
                if (strlen(outputs_buf) == 0) {
                    strcpy(outputs_buf, "NONE");
                }

                fprintf(fp, "%lld,%d,%s,%d,%s,%s\n", 
                       (long long int)input_vector,
                       curr_gate->level,
                       curr_gate->name, 
                       curr_gate->type, 
                       inputs_buf,
                       outputs_buf);
            }
        }
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

            fprintf(fp, "%lld,%d,%s,%d,%s,%s\n", 
                   input_vector,
                   i+1,
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
