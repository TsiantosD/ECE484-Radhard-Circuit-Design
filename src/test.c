#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "netlist.h"
#include "parser.h"
#include "simulation.h"

int main() {
    char *saveptr1, *saveptr2;
    char *token = NULL;
    char *argument_name = NULL;
    char *node_name = NULL;

    char buffer[] = "  NAND3_X1 U13 ( .A1(n4), .A2(n5), .A3(n10), .ZN(G17) );";

    token = strtok_r(buffer, " \t\r\n(", &saveptr1);
    printf("Gate Type: %s\n", token);

    token = strtok_r(NULL, " \t\r\n(", &saveptr1);
    printf("Instance Name: %s\n", token);

    token = strtok_r(NULL, ",;", &saveptr1);

    while (token != NULL) {
        argument_name = strtok_r(token, " .()\t\r\n", &saveptr2);
        node_name = strtok_r(NULL, " .()\t\r\n", &saveptr2);

        printf("    Argument: %s\n    Node: %s\n", argument_name, node_name);

        token = strtok_r(NULL, ",;", &saveptr1);
    }

    Gate *gate = (Gate*)calloc(1, sizeof(Gate));
    gate->type = TYPE_AND;
    gate->no_inputs = 3;

    gate->inputs = (Node**)calloc(3, sizeof(Node*));
    gate->inputs[0] = (Node*)calloc(1, sizeof(Node));
    gate->inputs[0]->value = 1;
    gate->inputs[1] = (Node*)calloc(1, sizeof(Node));
    gate->inputs[1]->value = 1;
    gate->inputs[2] = (Node*)calloc(1, sizeof(Node));
    gate->inputs[2]->value = 1;

    gate->outputs = (Node**)calloc(1, sizeof(Node*));
    gate->outputs[0] = (Node*)calloc(1, sizeof(Node));

    gate->outputs[0]->value = simulateGate(gate);

    gate->outputs[0]->name[0] = 'Q';
    if (gate->outputs[0]->name[0] == 'Q')
        printf("True\n");

    printf("%d\n", gate->outputs[0]->value);

    return 0;
}
