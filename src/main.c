#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parser.h"
#include "netlist.h"
#include "levelization.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    long long soft_error_counter = 0;
    long long strikes_counter = 0;
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

    // Store a copy of DFF inputs array
    NodesArray *golden_dff_inputs_array = (NodesArray*)calloc(1, sizeof(NodesArray));
    golden_dff_inputs_array->data = NULL;
    golden_dff_inputs_array->size = 0;

    int dff_inputs_count = 0;

    for (int i = 0; i < nodes_array->size; i++) {
        if (nodes_array->data[i]->is_ff_input == 1) {
            dff_inputs_count++;
        }
    }

    golden_dff_inputs_array->data = (Node**)realloc(golden_dff_inputs_array->data, dff_inputs_count * sizeof(Node*));

    for (int i = 0; i < dff_inputs_count; i++) {
        Node *new_node = (Node*)calloc(1, sizeof(Node));
        golden_dff_inputs_array->data[golden_dff_inputs_array->size] = new_node;
        golden_dff_inputs_array->size++;
    }

    // Simulate the circuit with all input vectors
    long long int max_vectors = pow(2, primary_inputs_array->size);

    for (long long int input_vector = 0; input_vector < max_vectors; input_vector++) {
        long long int tmp_input_vector = (long long)input_vector;
        for (int i = 0; i < primary_inputs_array->size; i++) {
            primary_inputs_array->data[i]->value = tmp_input_vector % 2;
            tmp_input_vector = tmp_input_vector >> 1;
        }

        // Normal simulation to get the steady state
        simulateCircuit(levels_array);

        // Display the nodes for verification purposes
        printNodesCurrentState(input_vector, primary_inputs_array, nodes_array);

        // Display the current circuit's state for visualization purposes
        printLevelsArrayStateCsv(levels_array, gates_array, input_vector);

        // Copy correct DFF input values to golden array
        for (int j = 0, c = 0; j < nodes_array->size; j++) {
            Node *curr_node = nodes_array->data[j];

            if (curr_node->is_ff_input == 1) {
                golden_dff_inputs_array->data[c]->value = curr_node->value;
                strncpy(golden_dff_inputs_array->data[c]->name, curr_node->name, 16);
                golden_dff_inputs_array->data[c]->name[15] = '\0';
                golden_dff_inputs_array->data[c]->type = curr_node->type;
                golden_dff_inputs_array->data[c]->level = curr_node->level;
                golden_dff_inputs_array->data[c]->SET_should_hit = 0;
                golden_dff_inputs_array->data[c]->is_ff_input = 1;
                c++;
            }
        }

        // Hit each gate not connected directly to a DFF. If at least one soft error is found,
        // skip this steady state and continue to the next one.
        int soft_error_found = 0;

        for (int j = 0; j < gates_array->size; j++) {
            if (soft_error_found == 1) {
                break;
            }

            Gate *curr_gate = gates_array->data[j];

            // Ignore FFs, gates connected to FFs, or gates connected to primary outputs
            if (curr_gate->type == TYPE_DFF || curr_gate->outputs[0]->is_ff_input == 1 || curr_gate->outputs[0]->type == TYPE_OUTPUT) {
                continue;
            }

            // Mark gate that should be hit
            curr_gate->outputs[0]->SET_should_hit = 1;

            // Run the simulation to consider if a soft error is propagated
            simulateCircuit(levels_array);

            // Increment strikes counter
            strikes_counter++;

            // Compare new and old DFF inputs
            for (int k = 0; k < golden_dff_inputs_array->size; k++) {
                Node *golden_dff_input = golden_dff_inputs_array->data[k];
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
                if (simulated_dff_input != NULL && simulated_dff_input->value != golden_dff_input->value) {
                    soft_error_counter++;
                    soft_error_found = 1;
                    break;
                }
            }

            // Unmark hit gate
            curr_gate->outputs[0]->SET_should_hit = 0;
        }
    }

    // Calculate Soft Error Rate
    soft_error_rate = soft_error_counter / (pow(2, primary_inputs_array->size) * strikes_counter);

    printf("SER: %f\nCounter: %lld\n", soft_error_rate, soft_error_counter);

    // Clean up
    for (int i = 0; i < golden_dff_inputs_array->size; i++) {
        free(golden_dff_inputs_array->data[i]);
    }
    free(golden_dff_inputs_array->data);
    free(golden_dff_inputs_array);

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
