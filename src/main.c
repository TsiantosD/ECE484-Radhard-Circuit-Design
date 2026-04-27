#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parser.h"
#include "netlist.h"
#include "levelization.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    long long soft_error_counter = 0;
    double soft_error_rate = 0.0;

    if (argc < 2) {
        printf("Usage: %s <path/to/file.v>\n", argv[0]);
        return 1;
    }

    char *pathname = argv[1];

    // Initialize nodes (internal and primary outputs)
    NodesArray *nodes_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    if (nodes_array == NULL) {
        exit(1);
    }
    nodes_array->size = 0;

    // Initialize primary inputs
    NodesArray *primary_inputs_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    if (primary_inputs_array == NULL) {
        exit(1);
    }
    primary_inputs_array->size = 0;

    // Initialize gates array
    GatesArray *gates_array = (GatesArray*)calloc(1, sizeof(GatesArray));
    if (gates_array == NULL) {
        exit(1);
    }
    gates_array->size = 0;

    parseVerilogFile(pathname, nodes_array, primary_inputs_array, gates_array);
    
    // Initialize levels array
    LevelsArray *levels_array = (LevelsArray*)calloc(1, sizeof(LevelsArray)); 
    if (levels_array == NULL) {
        exit(1);
    }
    levels_array->size = 0;

    levelizeGates(levels_array, gates_array);

    // Simulate the circuit with all input vectors
    for (long long int input_vector = 0; input_vector < pow(2, primary_inputs_array->size); input_vector++) {
        long long int tmp_input_vector = (long long)input_vector;
        for (int i = 0; i < primary_inputs_array->size; i++) {
            primary_inputs_array->data[i]->value = tmp_input_vector % 2;
            tmp_input_vector = tmp_input_vector >> 1;
        }

        // Normal simulation to get the correct values
        simulateCircuit(levels_array);

        // Store correct DFF inputs
        NodesArray *dff_inputs_array = (NodesArray*)calloc(1, sizeof(NodesArray));
        dff_inputs_array->data = NULL;
        dff_inputs_array->size = 0;

        for (int j = 0; j < nodes_array->size; j++) {
            Node *curr_node = nodes_array->data[j];

            if (curr_node->is_ff_input == 1) {
                dff_inputs_array->data = (Node**)realloc(dff_inputs_array->data, (dff_inputs_array->size + 1) * sizeof(Node**));
                dff_inputs_array->data[dff_inputs_array->size] = curr_node;
                dff_inputs_array->size++;
            }
        }

        // Hit each gate not connected directly to a FF
        for (int j = 0; j < gates_array->size; j++) {
            Gate *curr_gate = gates_array->data[j];

            // Ignore FFs and gates connected to FFs
            if (curr_gate->type == TYPE_DFF || curr_gate->outputs[0]->is_ff_input == 1) {
                continue;
            }

            // Mark gate that should be hit
            curr_gate->outputs[0]->SET_should_hit = 1;

            printf("Mark %s\n", curr_gate->outputs[0]->name);

            // Run the simulation to consider if a soft error is propagated
            simulateCircuit(levels_array);

            // Compare new and old DFF inputs
            for (int k = 0; k < dff_inputs_array->size; k++) {
                Node *golden_dff_input = dff_inputs_array->data[k];
                Node *simulated_dff_input = NULL;

                // Linear search to find the DFF node
                for (int m = 0; m < nodes_array->size; m++) {
                    Node *curr_node = nodes_array->data[m];

                    // Compare by name
                    if (!strcmp(golden_dff_input->name, curr_node->name)) {
                        simulated_dff_input = curr_node;
                        break;
                    }
                }

                // Compare the two nodes
                printf("Node: %s, simulated: %d, golden: %d\n", simulated_dff_input->name, simulated_dff_input->value, golden_dff_input->value);
                if (simulated_dff_input->value != golden_dff_input->value) {
                    soft_error_counter++;
                    break;
                }
            }

            // Unmark hit gate
            curr_gate->outputs[0]->SET_should_hit = 0;
        }

        // Display the nodes for verification purposes
        // printNodesCurrentState(input_vector, primary_inputs_array, nodes_array);

        // Display the current circuit's state for visualization purposes
        // printLevelsArrayStateCsv(levels_array, gates_array, input_vector);
    }

    // Calculate Soft Error Rate
    soft_error_rate = soft_error_counter / pow(2, primary_inputs_array->size);

    printf("SER: %f\nCounter: %lld\n", soft_error_rate, soft_error_counter);

    // Clean up
    for (int i = 0; i < gates_array->size; i++) {
        Gate *curr_gate = gates_array->data[i];

        free(curr_gate->inputs);
        free(curr_gate->outputs);
        free(curr_gate);
    }
    free(gates_array->data);
    free(gates_array);

    for (int i = 0; i < primary_inputs_array->size; i++) {
        free(primary_inputs_array->data[i]);
    }
    free(primary_inputs_array->data);
    free(primary_inputs_array);


    for (int i = 0; i < nodes_array->size; i++) {
        free(nodes_array->data[i]);
    }
    free(nodes_array->data);
    free(nodes_array);

    for (int i = 0; i < levels_array->size; i++) {
        GatesArray *curr_gates_array = levels_array->data[i];
        free(curr_gates_array->data);
        free(curr_gates_array);
    }
    free(levels_array->data);
    free(levels_array);

    return 0;
}
