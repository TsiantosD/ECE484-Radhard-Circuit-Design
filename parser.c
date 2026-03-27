#include <stdio.h>
#include <errno.h>
#include "parser.h"

int parseVerilogFile(char* pathname) {
    FILE* fptr;
    int i;
    char* buffer;

    fptr = fopen(pathname, "r");

    if (fptr == NULL) {
        perror("Error");
    }

    buffer = (char*)malloc(sizeof(char) * 100);



    return 0;
}
