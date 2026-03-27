#include <stdio.h>
#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <path/to/file.v>\n", argv[0]);
        return 1;
    }

    char* pathname = argv[1];

    parseVerilogFile(pathname);

    return 0;
}
