#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"
#include "parser.h"
#include "simulation.h"

SerResult runSerLegacy(LevelsArray *levels_array,
                       GatesArray *gates_array,
                       NodesArray *nodes_array,
                       NodesArray *primary_inputs_array,
                       FILE *nodes_fp,
                       FILE *levels_fp) {
    SerResult result = {0};

    NodesArray *golden_dff_inputs_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    if (golden_dff_inputs_array == NULL) {
        exit(1);
    }
    golden_dff_inputs_array->data = NULL;
    golden_dff_inputs_array->size = 0;

    int dff_inputs_count = 0;
    for (int i = 0; i < nodes_array->size; i++) {
        if (nodes_array->data[i]->is_ff_input == 1) {
            dff_inputs_count++;
        }
    }

    golden_dff_inputs_array->data = (Node**)realloc(golden_dff_inputs_array->data, dff_inputs_count * sizeof(Node*));
    if (dff_inputs_count > 0 && golden_dff_inputs_array->data == NULL) {
        exit(1);
    }

    for (int i = 0; i < dff_inputs_count; i++) {
        Node *new_node = (Node*)calloc(1, sizeof(Node));
        golden_dff_inputs_array->data[golden_dff_inputs_array->size] = new_node;
        golden_dff_inputs_array->size++;
    }

    for (int i = 0; i < gates_array->size; i++) {
        Gate *curr_gate = gates_array->data[i];
        if (curr_gate->type == TYPE_DFF || curr_gate->outputs[0]->is_ff_input == 1 || curr_gate->outputs[0]->type == TYPE_OUTPUT) {
            continue;
        }
        result.hit_gates_counter++;
    }

    long long int max_vectors = 1LL << primary_inputs_array->size;

    for (long long int input_vector = 0; input_vector < max_vectors; input_vector++) {
        long long int tmp_input_vector = input_vector;
        for (int i = 0; i < primary_inputs_array->size; i++) {
            primary_inputs_array->data[i]->value = tmp_input_vector % 2;
            tmp_input_vector = tmp_input_vector >> 1;
        }

        simulateCircuit(levels_array);
        printNodesCurrentState(nodes_fp, input_vector, primary_inputs_array, nodes_array);
        printLevelsArrayStateCsv(levels_fp, levels_array, gates_array, input_vector);

        for (int j = 0, c = 0; j < nodes_array->size; j++) {
            Node *curr_node = nodes_array->data[j];
            if (curr_node->is_ff_input == 1) {
                golden_dff_inputs_array->data[c]->value = curr_node->value;
                strncpy(golden_dff_inputs_array->data[c]->name, curr_node->name, 20);
                golden_dff_inputs_array->data[c]->name[19] = '\0';
                golden_dff_inputs_array->data[c]->type = curr_node->type;
                golden_dff_inputs_array->data[c]->level = curr_node->level;
                golden_dff_inputs_array->data[c]->SET_should_hit = 0;
                golden_dff_inputs_array->data[c]->is_ff_input = 1;
                c++;
            }
        }

        for (int j = 0; j < gates_array->size; j++) {
            Gate *curr_gate = gates_array->data[j];
            if (curr_gate->type == TYPE_DFF || curr_gate->outputs[0]->is_ff_input == 1 || curr_gate->outputs[0]->type == TYPE_OUTPUT) {
                continue;
            }

            curr_gate->outputs[0]->SET_should_hit = 1;
            simulateCircuit(levels_array);

            for (int k = 0; k < golden_dff_inputs_array->size; k++) {
                Node *golden_dff_input = golden_dff_inputs_array->data[k];
                Node *simulated_dff_input = NULL;

                for (int m = 0; m < nodes_array->size; m++) {
                    Node *curr_node = nodes_array->data[m];
                    if (!strcmp(golden_dff_input->name, curr_node->name)) {
                        simulated_dff_input = curr_node;
                        break;
                    }
                }

                if (simulated_dff_input != NULL && simulated_dff_input->value != golden_dff_input->value) {
                    result.soft_error_counter++;
                    break;
                }
            }

            curr_gate->outputs[0]->SET_should_hit = 0;
        }
    }

    result.total_simulations = max_vectors * result.hit_gates_counter;
    if (result.total_simulations > 0) {
        result.soft_error_rate = (double)result.soft_error_counter / (double)result.total_simulations;
    }

    for (int i = 0; i < golden_dff_inputs_array->size; i++) {
        free(golden_dff_inputs_array->data[i]);
    }
    free(golden_dff_inputs_array->data);
    free(golden_dff_inputs_array);

    return result;
}

void printSerResult(SerResult result) {
    printf("Total simulations: %lld\n", result.total_simulations);
    printf("Number of hit gates: %lld\n", result.hit_gates_counter);
    printf("Number of simulations with Soft Error(s): %lld\n", result.soft_error_counter);
    printf("SER: %.2f%%\n", result.soft_error_rate * 100.0);
}
