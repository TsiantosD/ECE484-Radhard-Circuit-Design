#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H

#include <string.h>

typedef struct Node_t {
    char name[16];
    char value;
    unsigned short int type;
} Node;

typedef struct Gate_t {
    char name[16];
    char value;
    unsigned short int type;
    int no_inputs;
    Node **inputs;
    Node **outputs;
} Gate;

typedef struct Levels_t {
    Gate **data;
    int size;
} Levels;

typedef struct GatesArray_t {
    Gate **data;
    int size;
} GatesArray;

typedef struct NodesArray_t {
    Node **data;
    int size;
} NodesArray;


#endif //ECE484_RADHARD_CIRCUIT_DESIGN_NETLIST_H
