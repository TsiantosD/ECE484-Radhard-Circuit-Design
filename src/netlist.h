#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H

#include <string.h>

typedef struct Node_t {
    char name[16];
    int value;
    unsigned short int type;
    int level;
    int SET_should_hit;
    int is_ff_input;
} Node;

typedef struct Gate_t {
    char name[16];
    unsigned short int type;
    int no_inputs;
    int level;
    Node **inputs;
    Node **outputs;
} Gate;

typedef struct GatesArray_t {
    Gate **data;
    int size;
} GatesArray;

typedef struct LevelsArray_t {
    GatesArray **data;
    int size;
} LevelsArray;

typedef struct NodesArray_t {
    Node **data;
    int size;
} NodesArray;

void printNodesCurrentState(long long int input_vector, NodesArray *primary_inputs, NodesArray *nodes);
void printLevelsArrayStateCsv(LevelsArray *levels_array, GatesArray *gates_array, long long int input_vector);
void printLevelsArray(LevelsArray *levels_array);
void printGatesArray(GatesArray *gates_array);

#endif //ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H
