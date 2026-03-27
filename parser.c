#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "parser.h"

#define TYPE_INPUT 0
#define TYPE_OUTPUT 1
#define TYPE_WIRE 2

int parseVerilogFile(char* pathname) {
    FILE* fptr;
    int i = 0, buffer_size = 100;
    char* buffer = NULL;

    fptr = fopen(pathname, "r");

    if (fptr == NULL) {
        perror("Error");
    }

    buffer = (char*)calloc(sizeof(char) * 100);
    if (buffer == NULL) {
        exit(1);
    }

    while (1) {
        if ((i % 100) == 0) {
            buffer_size += 100;
            buffer = (char*)realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL) {
                exit(1);
            }
        }
        if ((buffer[i] = fgetc(fptr)) == EOF) {
            break;
        }
        if (buffer[i] == ';') {
            buffer[i] = '/0';
            if (strstr(buffer, "input")){
                parse_input_output_wire(TYPE_INPUT, buffer);
            }
            else if (strstr(buffer, "output")){
                parse_input_output_wire(TYPE_OUTPUT, buffer);
            }
            else if (strstr(buffer, "wire")) {
                parse_input_output_wire(TYPE_WIRE, buffer);
            }
            else if ()
            
        
        }

        i++;
    }
    return 0;
}

void parse_input_output_wire(int type, char *buffer) {
    
    char *token = NULL, *save_ptr = NULL;
    int offset = 0, i = 0;

    switch (type){
        case TYPE_INPUT:
            offset = 6;
            break;
        case TYPE_OUTPUT:
            offset = 7; 
            break;
        case TYPE_WIRE:
            offset = 7;
            break;
        default:
            printf("Invalid type\n");
            exit(1);
            break;
    }

    while (1) {
        if (buffer[i] == ' ') {
            offset++;
        }
        else {
            break;
        }
        i++;
    }
    
    while (1) {
        token = strtok_r(buffer + (offset * sizeof(char)),', ',&save_ptr);
        if (token == NULL) {
            break;
        }
        if(!strcmp(token, "GND") || !strcmp(token, "VDD") || !strcmp(token, "CK")) {
            continue;
        } 
        // need to create the node here to add the corresponding input to its struct
    } 
}

