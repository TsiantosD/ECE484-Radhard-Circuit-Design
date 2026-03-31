#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "parser.h"
#include "netlist.h"


int parseVerilogFile(char* pathname, NodesArray *nodes_array, NodesArray *primary_inputs_array, GatesArray *gates_array) {
    FILE* fptr;
    int i = 0, buffer_size = INITIAL_BUFFER_SIZE;
    char* buffer = NULL;
    int curr_char;

    // Open the verilog file for reading
    if ((fptr = fopen(pathname, "r")) == NULL) {
        perror("Error");
        fclose(fptr);
        exit(1);
    }

    // Create a read buffer
    if ((buffer = (char*)calloc(INITIAL_BUFFER_SIZE, sizeof(char))) == NULL) {
        exit(1);
    }

    while (1) {
        // Extend the buffer when full
        if (i == buffer_size - 1) {
            buffer_size += INITIAL_BUFFER_SIZE;

            char *tmp_buffer = (char*)realloc(buffer, buffer_size * sizeof(char));
            if (tmp_buffer == NULL) {
                exit(1);
            }
            buffer = tmp_buffer;
        }

        // Get current character
        curr_char = fgetc(fptr);

        // End-of-file
        if (curr_char == EOF) {
            break;
        }

        // Save character to buffer
        buffer[i] = (char)curr_char;

        // Current character is semicolon, process the buffer
        if (buffer[i] == ';') {

            buffer[i + 1] = '\0'; // add null char in the end

            if (strstr(buffer, KEYWORD_MODULE)) {
                // Do nothing
                printf("Module found!\n");
            }
            else if (strstr(buffer, KEYWORD_INPUT)) {
                parseAndCreateNodes(buffer, TYPE_INPUT, nodes_array, primary_inputs_array);
            }
            else if (strstr(buffer, KEYWORD_OUTPUT)) {
                parseAndCreateNodes(buffer, TYPE_OUTPUT, nodes_array, primary_inputs_array);
            }
            else if (strstr(buffer, KEYWORD_WIRE)) {
                parseAndCreateNodes(buffer, TYPE_WIRE, nodes_array, primary_inputs_array);
            }
            else if (strstr(buffer, KEYWORD_DFF)) {
                parseAndCreateGate(buffer, TYPE_DFF, nodes_array, primary_inputs_array, gates_array);
            }
            else if (strstr(buffer, KEYWORD_INV)) {
                parseAndCreateGate(buffer, TYPE_INV, nodes_array, primary_inputs_array, gates_array);
            }
            else if (strstr(buffer, KEYWORD_NAND)) {
                parseAndCreateGate(buffer, TYPE_NAND, nodes_array, primary_inputs_array, gates_array);
            }
            else if (strstr(buffer, KEYWORD_NOR)) {
                parseAndCreateGate(buffer, TYPE_NOR, nodes_array, primary_inputs_array, gates_array);
            }
            else if (strstr(buffer, KEYWORD_OR)) {
                parseAndCreateGate(buffer, TYPE_OR, nodes_array, primary_inputs_array, gates_array);
            }
            else if (strstr(buffer, KEYWORD_AND)) {
                parseAndCreateGate(buffer, TYPE_AND, nodes_array, primary_inputs_array, gates_array);
            }

            // Recreate the buffer
            free(buffer);
            if ((buffer = (char*)calloc(sizeof(char), INITIAL_BUFFER_SIZE)) == NULL) {
                exit(1);
            }
            buffer_size = INITIAL_BUFFER_SIZE;

            i = -1;
        }

        i++;
    }

    free(buffer);
    buffer_size = 0;
    fclose(fptr);

    return 0;
}

/**
 * Expects a buffer with all inputs, or outputs, or wires. Reads the buffer token by token, which are the names of the
 * nodes, for each node, it creates a new node structure and stores it in the related array.
 * 
 * If a node is an input, it stores it in the related primary inputs array.
 * If a node is either a wire or output, it stores it in the related nodes array.
 * 
 * Example of a buffer:
 * "  input GND, VDD, CK, G0, G1,
 *          G2, G3;"
 */
void parseAndCreateNodes(char *buffer, int type, NodesArray *nodes_array, NodesArray *primary_inputs_array) {
    char *token = NULL;
    int f = 1;

    token = strtok(buffer, ", ;\t\r\n");

    while (token != NULL) {
        // Skip the first token - it's always the keyword
        if (f) {
            f = 0;
            token = strtok(0, ", ;\t\r\n");
            continue;
        }

        // Skip specific input nodes
        if (type == TYPE_INPUT && (!strcmp(token, "GND") || !strcmp(token, "VDD") || !strcmp(token, "CK"))) {
            token = strtok(0, ", ;\t\r\n");
            continue;
        }

        // Create a new node
        Node *new_node = (Node*)calloc(1, sizeof(Node));
        strncpy(new_node->name, token, 16);
        new_node->name[15] = '\0';
        new_node->type = type;
        new_node->value = 0;
        new_node->level = (new_node->type == TYPE_INPUT) ? 0 : -1;

        // Add the new node to related array
        if (type == TYPE_INPUT) {
            primary_inputs_array->data = (Node**)realloc(primary_inputs_array->data, (primary_inputs_array->size + 1) * sizeof(Node*));
            if (primary_inputs_array->data == NULL) {
                exit(1);
            }
            primary_inputs_array->data[primary_inputs_array->size] = new_node;
            primary_inputs_array->size++;

        } else if (type == TYPE_OUTPUT || type == TYPE_WIRE) {
            nodes_array->data = (Node**)realloc(nodes_array->data, (nodes_array->size + 1) * sizeof(Node*));
            if (nodes_array->data == NULL) {
                exit(1);
            }
            nodes_array->data[nodes_array->size] = new_node;
            nodes_array->size++;
        }

        token = strtok(0, ", ;");
    }
}

/**
 * Gets a buffer with the initialization of a gate, reads the buffer token by token, creates that gate
 * and stores it to the related array.
 * 
 * TODO: check type inside function
 * 
 * Example of a buffer:
 * "  NAND4_X1 U12610 ( .A1(n5054), .A2(n5055), .A3(n5056), .A4(n5057), .ZN(
        WX11050) );"
 */
void parseAndCreateGate(char *buffer, int type, NodesArray *nodes_array,
                        NodesArray *primary_inputs_array, GatesArray *gates_array) {
    char *token = NULL;
    char *saveptr1, *saveptr2;
    int token_countdown = 2;
    int inputs_index = 0;
    int outputs_index = 0;

    // Create the new gate
    Gate *new_gate = (Gate*)calloc(1, sizeof(Gate));
    new_gate->name[15] = '\0';
    new_gate->type = type;
    new_gate->value = 0;
    new_gate->level = -1;

    token = strtok_r(buffer, " \t\r\n", &saveptr1);

    while (token != NULL) {
        switch (token_countdown) {
            // Get first token (instance type)
            case 2:
                // We have a DFF, set input to one
                if (strstr(token, KEYWORD_DFF)) {
                    new_gate->no_inputs = 1;
                } else if (strstr(token, KEYWORD_INV)) {
                    new_gate->no_inputs = 1;
                } else {
                    // Parse number of inputs from instance name (e.g.:NAND10_X1 -> 10 inputs)
                    char no_inputs_str[16];
                    no_inputs_str[15] = '\0';
                    int i = 0;
                    int j = 0;

                    while (token[i] != '_') {
                        if (token[i] >= '0' && token[i] <= '9') {
                            no_inputs_str[j] = token[i];
                            j++;
                        }
                        i++;
                    }

                    new_gate->no_inputs = atoi(no_inputs_str);
                }

                token_countdown--;

                break;

            // Get second token (instance name)
            case 1:
                // Parse name
                strncpy(new_gate->name, token, 16);
                new_gate->name[15] = '\0';
                token_countdown--;

                break;

            // Parse the arguments
            case 0:
            default:
                // if (*token == '(' || *token == ')') {
                //     token = strtok_r(0, ", ;\t\r\n", &saveptr1);
                //     continue;
                // }
 
                int is_output;
                Node *curr_node = NULL;
                char *argument_name = NULL;
                char *node_name = NULL;

                // Get argument name
                argument_name = strtok_r(token, " .()\t\r\n", &saveptr2);

                if (argument_name == NULL) {
                    break;
                }

                // Ignore the clock
                if (!strcmp(argument_name, "CK")) {
                    break;
                }

                is_output = (strcmp(argument_name, "ZN") == 0
                             || strcmp(argument_name, "Q") == 0
                             || strcmp(argument_name, "QN") == 0);

                // Get the node name
                node_name = strtok_r(0, " .()\t\r\n", &saveptr2);

                // Current argument is output, save it to the outputs array
                if (is_output) {
                    if (new_gate->outputs == NULL) {
                        if (new_gate->type == TYPE_DFF) {
                            new_gate->outputs = (Node**)calloc(2, sizeof(Node*));
                        } else {
                            new_gate->outputs = (Node**)calloc(1, sizeof(Node*));
                        }
                    }

                    curr_node = getNodeByName(nodes_array, node_name);

                    // When an output of a FF is found, mark it as a level 0
                    // and the value always 1 if it's Q or always 0 if it's QN
                    if (type == TYPE_DFF) {
                        curr_node->level = 0;
                        curr_node->value = !strcmp(argument_name, "Q"); // it's either Q or QN
                    }

                    new_gate->outputs[outputs_index] = curr_node;
                    outputs_index++;

                // Current argument is input, save it to the inputs array
                } else {
                    if (new_gate->inputs == NULL) {
                        new_gate->inputs = (Node**)calloc(new_gate->no_inputs, sizeof(Node*));
                    }

                    // Search for the node in nodes array
                    curr_node = getNodeByName(nodes_array, node_name);

                    // When not found, also search in the primary inputs array
                    if (curr_node == NULL) {
                        curr_node = getNodeByName(primary_inputs_array, node_name);
                    }

                    new_gate->inputs[inputs_index] = curr_node;
                    inputs_index++;
                }
                break;
        }

        if (token_countdown == 2 || token_countdown == 1) {
            token = strtok_r(0, " \t\r\n(", &saveptr1);
        } else {
            token = strtok_r(0, ",;", &saveptr1);
        }
    }

    // Store the new gate
    gates_array->data = (Gate**)realloc(gates_array->data, (gates_array->size + 1) * sizeof(Gate*));
    if (gates_array->data == NULL) {
        exit(1);
    }
    gates_array->data[gates_array->size] = new_gate;
    gates_array->size++;
}

Node *getNodeByName(NodesArray *nodes_array, char *node_name) {
    Node *curr_node = NULL;

    for (int i = 0; i < nodes_array->size; i++) {
        curr_node = nodes_array->data[i];

        if (!strcmp(curr_node->name, node_name))
            return curr_node;
    }

    return NULL;
}
