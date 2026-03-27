#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "parser.h"

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

int parseVerilogFile(char* pathname) {
    FILE* fptr;
    int i = 0, buffer_size = INITIAL_BUFFER_SIZE;
    char* buffer = NULL;

    fptr = fopen(pathname, "r");

    if (fptr == NULL) {
        perror("Error");
        fclose(fptr);
        exit(1);
    }

    buffer = (char*)calloc(INITIAL_BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        exit(1);
    }

    while (1) {
        // Extend the buffer
        if (i != 0 && (i % INITIAL_BUFFER_SIZE) == 0) {
            buffer_size += INITIAL_BUFFER_SIZE;
            buffer = (char*)realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL) {
                exit(1);
            }
        }

        // End-of-file
        if ((buffer[i] = fgetc(fptr)) == EOF) {
            break;
        }

        if (buffer[i] == ';') {
            if (strstr(buffer, KEYWORD_MODULE)) {
                // Do nothing
                printf("Module found!\n");
            }
            else if (strstr(buffer, KEYWORD_INPUT)) {
                parseNode(buffer, TYPE_INPUT);
            }
            else if (strstr(buffer, KEYWORD_OUTPUT)) {
                parseNode(buffer, TYPE_OUTPUT);
            }
            else if (strstr(buffer, KEYWORD_WIRE)) {
                parseNode(buffer, TYPE_WIRE);
            }
            else if (strstr(buffer, KEYWORD_DFF)) {
                parseGate(buffer, TYPE_DFF);
            }
            else if (strstr(buffer, KEYWORD_INV)) {
                parseGate(buffer, TYPE_INV);
            }
            else if (strstr(buffer, KEYWORD_NAND)) {
                parseGate(buffer, TYPE_NAND);
            }
            else if (strstr(buffer, KEYWORD_NOR)) {
                parseGate(buffer, TYPE_NOR);
            }
            else if (strstr(buffer, KEYWORD_OR)) {
                parseGate(buffer, TYPE_OR);
            }
            else if (strstr(buffer, KEYWORD_AND)) {
                parseGate(buffer, TYPE_AND);
            }

            // Clear the buffer
            free(buffer);
            buffer = (char*)calloc(sizeof(char), INITIAL_BUFFER_SIZE);
            i = -1;
        }

        i++;
    }
    return 0;
}

void parseNode(char *buffer, int type) {
    char *token = NULL;
    int f = 1;

    token = strtok(buffer, ", ;");

    printf("Node of type: %d\n", type);
    while (token != NULL) {
        // Skip the first token (input, output or wire)
        if (f) {
            f = 0;
            token = strtok(0, ", ;");
            continue;
        }

        if (!strcmp(token, "GND") || !strcmp(token, "VDD") || !strcmp(token, "CK")) {
            token = strtok(0, ", ;");
            continue;
        }

        printf("%s\n", token);

        token = strtok(0, ", ;");
    } 
}

void parseGate(char *buffer, int type) {
    char *token = NULL;
    int count = 2;

    char name[16];

    token = strtok(buffer, ", ;");

    printf("Gate of type: %d\n", type);

    while (token != NULL) {
        if (count == 2) {
            // Parse gate type of flip flop
            count--;

        } else if (count == 1) {
            // Parse name
            strcpy(name, token);
            count--;
        }

        // Handle adding gate
        printf("%s\n", token);

        token = strtok(0, ", ;");
    }
}
