#include <stdio.h>
#include <string.h>

int main() {
    char *saveptr1, *saveptr2;
    char *token = NULL;
    char *argument_name = NULL;
    char *node_name = NULL;

    char buffer[] = "  NAND4_X1 U12610 ( .A1(n5054), .A2(n5055), .A3(n5056), .A4(n5057), .ZN(\n        WX11050) );";

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

    return 0;
}