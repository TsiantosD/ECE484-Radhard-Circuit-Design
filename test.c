#include <stdio.h>
#include <string.h>

int main() {
    char *saveptr1, *saveptr2;
    char *token = NULL;
    char *subtoken = NULL;

    char buffer[100] = "  NAND4_X1 U9343 ( .A1(n3806), .A2(n3807), .A3(n3808), .A4(n3809), .ZN(WX3232) );";

    token = strtok_r(buffer, ", ;", &saveptr1);
    printf("token: %s\n", token);

    while (token != NULL) {
        printf("%s\n", token);

        subtoken = strtok_r(token, " .()", &saveptr2);
        while (subtoken != NULL) {
            printf("    %s\n", subtoken);
            subtoken = strtok_r(0, " .()", &saveptr2);
        }

        token = strtok_r(0, ", ;", &saveptr1);
    }

    return 0;
}