#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    OK
} Errors;

Errors add_to_ans_file(char **result, size_t *capacity, const char *name_of_file) {
    if (*result == NULL) {
        return INVALID_ARG;
    } 

    size_t new_size = *capacity + strlen(name_of_file) + 15;
    char *temp = (char*)realloc(*result, new_size);
    if (temp == NULL) {
        return INVALID_MEMORY;
    }
    *result = temp;
    *capacity = new_size;

    char text[] = "\n";
    strcat(*result, text);
    strcat(*result, text);
    strcat(*result, name_of_file);
    
    return OK;
}

Errors add_to_ans(char **result, size_t *capacity, int line_of_file, int pos_of_str) {
    if (*result == NULL) {
        return INVALID_ARG;
    } 

    char text1[] = "\nIn line: ";
    char text2[] = ", pos: ";
    char itoa_line[12];
    char itoa_pos[12];

    size_t new_size = *capacity + strlen(text1) + strlen(text2) + strlen(itoa_line) + strlen(itoa_pos) + 30;
    char *temp = (char*)realloc(*result, new_size);
    if (temp == NULL) {
        return INVALID_MEMORY;
    }
    *result = temp;
    *capacity = new_size;

    sprintf(itoa_line, "%d", line_of_file);
    sprintf(itoa_pos, "%d", pos_of_str);

    strcat(*result, text1);
    strcat(*result, itoa_line);
    strcat(*result, text2);
    strcat(*result, itoa_pos);
    return OK;
}

Errors search_str_in_this_file(FILE *file, size_t *capacity, const char *str, char **result) {
    if (file == NULL || *result == NULL || str == NULL) {
        return INVALID_ARG;
    } 

    size_t str_len = strlen(str);
    
    int c; //текущий символ
    int line_of_file = 1; //текущая строка
    int pos_of_str = 0; //текущая позиция в строке
    int index = 0; //индекс для прохода по строке
    int ans_pos = 0; //позиция откуда начинает строка совпадать

    while ((c = getc(file)) != EOF) {
        pos_of_str++;
        if (c == '\n') {
            line_of_file++;
            pos_of_str = 0;
            index = 0;
        }
        if (c == str[index]) {
            index++;
            if (index == 1) {
                ans_pos = pos_of_str;
            } 
            if (index == str_len) {
                Errors err = add_to_ans(result, capacity, line_of_file, ans_pos);
                if (err != OK) {
                    return err;
                }
                index = 0;
            }
        } else if (index > 0) {
            //обнуляем все
            fseek(file, -index, SEEK_CUR);
            pos_of_str -= (index);
            ans_pos = 0;
            index = 0;
            
        } 
    }

    return OK;
}

Errors is_str_in_files(char **result, size_t *capacity, int amount_of_file, const char *str, ...) {
    if (amount_of_file <= 0 || *result == NULL || str == NULL) {
        return INVALID_ARG;
    } 

    va_list files;
    va_start(files, str);

    FILE *file;
    char *name_of_file;

    for (int i = 0; i < amount_of_file; ++i) {
        name_of_file = va_arg(files, char*);
        file = fopen(name_of_file, "r");
        if (!file) {
            va_end(files);
            return INVALID_FILE;
        }

        Errors err_f = add_to_ans_file(result, capacity, name_of_file);
        if (err_f != OK) {
            fclose(file);
            va_end(files);
            return err_f;
        }

        Errors err = search_str_in_this_file(file, capacity, str, result);
        fclose(file);

        if (err != OK) {
            va_end(files);
            return err;
        }
    }

    va_end(files);
    return OK;
}

int main() {
    char *result = (char*)malloc(1);
    if (result == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    int amount_of_file = 3;
    const char *str = "eww";
    size_t capacity = 1;

    Errors err = is_str_in_files(&result, &capacity, amount_of_file, str, "t1.txt", "t2.txt", "t3.txt");
    if (err == INVALID_MEMORY) {
        printf("Problems with memory\n");
    } else if (err == INVALID_FILE) {
        printf("Problems with file\n");
    } else if (err == INVALID_ARG) {
        printf("Problems with args\n");
    } else {
        printf("%s\n", result);
    }
    
    free(result);
    return 0;
}