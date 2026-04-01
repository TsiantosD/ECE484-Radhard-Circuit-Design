#include "netlist.h"

#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H

#define INITIAL_BUFFER_SIZE 1000

#define KEYWORD_MODULE "module"

#define KEYWORD_INPUT "input"
#define KEYWORD_OUTPUT "output"
#define KEYWORD_WIRE "wire"

#define TYPE_INPUT 0
#define TYPE_OUTPUT 1
#define TYPE_WIRE 2

#define KEYWORD_DFF "DFF"
#define KEYWORD_INV "INV"
#define KEYWORD_NAND "NAND"
#define KEYWORD_NOR "NOR"
#define KEYWORD_OR "OR"
#define KEYWORD_AND "AND"

#define TYPE_DFF 0
#define TYPE_INV 1
#define TYPE_NAND 2
#define TYPE_NOR 3
#define TYPE_OR 4
#define TYPE_AND 5

int parseVerilogFile(char* pathname, NodesArray *nodes_array, NodesArray *primary_inputs_array, GatesArray *gates_array);

void parseAndCreateNodes(char *buffer, int type, NodesArray *nodes_array,
                         NodesArray *primary_inputs_array);

void parseAndCreateGate(char *buffer, int type, NodesArray *nodes_array,
                        NodesArray *primary_inputs_array, GatesArray *gates_array);

Node *getNodeByName(NodesArray *nodes_array, char *node_name);

#endif //ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H
