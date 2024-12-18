#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <limits.h>
//#include <unistd.h>
//#include <ctype.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    OK
} Errors;

int main() {
    FILE *file = fopen("t.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "We can't open this file\n");
        return 1;
    }

    char s[1024];
    int num1, num2, num3;
    unsigned int num4;
    if (overfscanf(file, "%CV %Zr %3s %Cv %Ro", &num1, 16, &num4, s, &num2, 16, &num3) == INVALID_MEMORY) {
        printf("Memory allocation failed\n");
        fclose(file);
        return 1;
    }
    printf("Results: %d %d %s %d %d\n", num1, num4, s, num2, num3);





    fclose(file);
    return 0;
}