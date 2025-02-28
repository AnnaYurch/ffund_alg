#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    OK
} Errors;

Errors tr_matrix(const char *input_file, const char *output_file) {
    FILE *infile = fopen(input_file, "r");
    if (!infile) {
        return INVALID_FILE;
    }

    int rows = 0, cols = 0;
    int **matrix = NULL;

    char line[1024];
    while (fgets(line, sizeof(line), infile)) {
        // Count the number of integers in the line
        int count = 0;
        char *token = strtok(line, " \t\n");
        while (token) {
            count++;
            token = strtok(NULL, " \t\n");
        }

        // Update the max column count
        if (count > cols) {
            cols = count;
        }

        // Allocate memory for the matrix
        int **temp_matrix = realloc(matrix, (rows + 1) * sizeof(int *));
        if (!temp_matrix) {
            fclose(infile);
            for (int i = 0; i < rows; i++) {
                free(matrix[i]);
            }
            free(matrix);
            return INVALID_MEMORY;
        }
        matrix = temp_matrix;

        matrix[rows] = (int *)calloc(cols, sizeof(int));
        if (!matrix[rows]) {
            fclose(infile);
            for (int i = 0; i < rows; i++) {
                free(matrix[i]);
            }
            free(matrix);
            return INVALID_MEMORY;
        }

        // Read the integers into the matrix
        token = strtok(line, " \t\n");
        int i = 0;
        while (token) {
            matrix[rows][i] = atoi(token);
            token = strtok(NULL, " \t\n");
            i++;
        }
        rows++;
    }
    fclose(infile);

    printf("Rows: %d, Columns: %d\n", rows, cols);

    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {
        for (int i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
        return INVALID_FILE;
    }

    // Transpose the matrix and write to output file
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            fprintf(outfile, "%d ", matrix[i][j]);
        }
        fprintf(outfile, "\n");
    }
    fclose(outfile);

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return INVALID_ARG;
    }

    Errors result = tr_matrix(argv[1], argv[2]);
    if (result != OK) {
        switch (result) {
            case INVALID_MEMORY:
                fprintf(stderr, "Memory allocation error.\n");
                break;
            case INVALID_ARG:
                fprintf(stderr, "Invalid arguments.\n");
                break;
            case INVALID_FILE:
                fprintf(stderr, "File error.\n");
                break;
            default:
                break;
        }
        return result;
    }

    return 0;
}
