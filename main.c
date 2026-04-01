#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parser.h"
#include "netlist.h"
#include "levelization.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
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

    int i = 0;
    for (long long int input_vector = 0; input_vector < pow(2, primary_inputs_array->size); input_vector++) {
        long long int tmp_input_vector = (long long)input_vector;
        for (int i = 0; i < primary_inputs_array->size; i++) {
            primary_inputs_array->data[i]->value = tmp_input_vector % 2;
            tmp_input_vector = tmp_input_vector>>1;
        }   
        simulateCircuit(levels_array);
        printLevelsArray(levels_array);
        printf("%d, %lf\n", i, pow(2, primary_inputs_array->size));
        i = i + 1;
    }


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
