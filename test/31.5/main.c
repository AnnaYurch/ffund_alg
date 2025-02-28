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

    char line[1024];
    while (fgets(line, sizeof(line), infile)) {
        rows++;
        int count = 0;
        char *token = strtok(line, " \t\n");
        while (token) {
            count++;
            token = strtok(NULL, " \t\n");
        }

        if (count > cols) {
            cols = count;
        }
    }

    fseek(infile, 0, SEEK_SET);
 // Вернуться в начало файла для чтения данных

    int **matrix = (int**)malloc(rows * sizeof(int*));
    if (!matrix) {
        fclose(infile);
        return INVALID_MEMORY;
    }

    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        if (!matrix[i]) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            fclose(infile);
            return INVALID_MEMORY;
        }
    }

    int r = 0;
    while (fgets(line, sizeof(line), infile)) {
        char *tok = strtok(line, " ");
        for (int i = 0; i < cols; i++) {
            if (tok) {
                matrix[r][i] = atoi(tok);
                tok = strtok(NULL, " ");
            } else {
                matrix[r][i] = 0;
            }
        }
        r++;
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

    // Транспонирование матрицы и запись в выходной файл
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            fprintf(outfile, "%d ", matrix[i][j]);
        }
        fprintf(outfile, "\n");
    }
    fclose(outfile);

    // Освобождение выделенной памяти
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
