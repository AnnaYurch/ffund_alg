#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#define FILL_BYTE 0x00
#define BUFFER_SIZE 1024

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    INVALID_DATA,
    INVALID_LIMIT,
    INVALID_WRITTEN,
    INVALID_READ,
    INVALID_PATH,
    INVALID_SYSCALL,
    WE_FIND_STR,
    WE_NOT_FIND_STR,
    OK
} Errors;

void xorN(const char *filename, int N) {
    FILE *file = fopen(filename, "r");  // Открываем файл в текстовом режиме
    if (!file) {
        fprintf(stderr, "Ошибка при fopen %s\n", filename);
        return;
    }

    int block_len_bits = 1 << N;          //длина блока в битах (возведение в ^2)
    int block_len_bytes = (block_len_bits + 7) / 8; // Размер блока в байтах

    char *buffer = (char*)malloc(block_len_bytes);  // Буфер для чтения данных из файла
    if (!buffer) {
        fprintf(stderr, "Ошибка при malloc\n");
        fclose(file);
        return;
    }

    char *result = (char*)calloc(block_len_bytes, 1); //Выделяем память для храниния XOR для каждого блока 
    if (!result) {
        fprintf(stderr, "Ошибка при calloc\n");
        free(buffer);
        fclose(file);
        return;
    }

    // Чтение блоков файла
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, block_len_bytes, file)) > 0) {
        // Дополняем последний блок пробелами, если количество символов меньше необходимого
        if (bytes_read < block_len_bytes) {
            memset(buffer + bytes_read, FILL_BYTE, block_len_bytes - bytes_read);
        }

        // XOR всех блоков
        for (int i = 0; i < block_len_bytes; i++) {
            result[i] ^= buffer[i];
        }
    }

    printf("%s: xor%d: ", filename, N);
    for (int i = 0; i < block_len_bytes; i++) {
        printf("%02X", result[i]); //в 16 формате
    }
    printf("\n");

    free(buffer);
    free(result);
    fclose(file);
}

Errors mask(const char *filename, uint32_t maskValue) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return INVALID_FILE;
    }

    uint32_t num;
    int count = 0;
    while (fscanf(file, "%u", &num) == 1) {
        //printf("%u\n", num);
        if ((num & maskValue) == maskValue) {
            count++;
        }
    }

    printf("Файл %s содержит %d чисел, соответствующих маске %X\n", filename, count, maskValue);

    fclose(file);
    return OK;
}

void copyN(const char *filename, int N) {
    char baseFilename[strlen(filename) + 1];
    char extension[10];
    char *dotPos = strrchr(filename, '.');  // Ищем последнюю точку в имени файла

    if (dotPos) {
        size_t nameLength = dotPos - filename;
        strncpy(baseFilename, filename, nameLength);
        baseFilename[nameLength] = '\0'; 
        strcpy(extension, dotPos);  
    } else {
        strcpy(baseFilename, filename);
        strcpy(extension, ""); 
    }

    for (int i = 1; i <= N; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            char newFilename[strlen(filename) + 1 + 10 + 1 + 9];
            snprintf(newFilename, sizeof(newFilename), "%s_copy%d%s", baseFilename, i, extension);

            FILE *src = fopen(filename, "r");
            FILE *dest = fopen(newFilename, "w");

            if (!src || !dest) {
                fprintf(stderr, "Ошибка при открытии файлов\n");
                exit(1);
            }

            char ch;
            while ((ch = fgetc(src)) != EOF) {
                fputc(ch, dest);
            }

            fclose(src);
            fclose(dest);

            printf("Создана копия %s\n", newFilename);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }
}

Errors search_str_in_this_file(FILE *file, const char *str) {
    if (file == NULL || str == NULL) {
        return INVALID_ARG;
    } 

    size_t str_len = strlen(str);
    
    int c; //текущий символ
    int line_of_file = 1; //текущая строка
    int ans_line_of_file = 1;
    int pos_of_str = 0; //текущая позиция в строке
    int index = 0; //индекс для прохода по строке
    int ans_pos = 0; //позиция откуда начинает строка совпадать
    int count_of_n = 0;

    while ((c = getc(file)) != EOF) {  
        pos_of_str++;
        
        if (c == str[index]) {
            index++;
            if (index == 1) {
                ans_pos = pos_of_str;
                ans_line_of_file = line_of_file;
                if (c == '\n') { //что если "\n"
                    count_of_n++;
                    line_of_file++;
                    pos_of_str = 0;
                }
            } else if (c == '\n') { //что если "\n"
                count_of_n++;
                line_of_file++;
                pos_of_str = 0;
            }
            if (c == '\n' && str_len == 1) {
                fclose(file);
                return WE_FIND_STR;
            }
            if (index == str_len) {
                fclose(file);
                return WE_FIND_STR;
            }
        } else if (index > 0) {
            //обнуляем все
            fseek(file, -index, SEEK_CUR);
            pos_of_str -= (index);
            ans_pos = 0;
            index = 0;
            line_of_file = ans_line_of_file;
            
        } else if (c == '\n') {
            line_of_file++;
            pos_of_str = 0;
            index = 0;
        }
    }

    fclose(file);
    return OK;
}

void findInFiles(int fileCount, char **files, const char *str) {
    int found = 0;

    for (int i = 0; i < fileCount; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            fprintf(stderr, "Ошибка fork\n");
            exit(1);
        }

        if (pid == 0) { 
            FILE *file = fopen(files[i], "r");
            if (!file) {
                fprintf(stderr, "Ошибка открытия файла\n");
                exit(1);
            }
            Errors err = search_str_in_this_file(file, str);
            if (err == WE_FIND_STR) {
                printf("Строка найдена в файле: %s\n", files[i]);
                exit(0);
            }
            exit(1);
        }
    }

    for (int i = 0; i < fileCount; i++) {
        int status;
        wait(&status);
        if (WEXITSTATUS(status) == 0) {
            found = 1;
        }
    }

    if (!found) {
        printf("Строка не найдена ни в одном файле\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <файлы> <операция>\n", argv[0]);
        return 1;
    }

    int fileCount;
    if (strncmp(argv[argc - 2], "find", 4) == 0 || strncmp(argv[argc - 2], "mask", 4) == 0) {
        fileCount = argc - 3;
    } else {
        fileCount = argc - 2;
    }

    char **files = &argv[1];

    if (strncmp(argv[argc - 1], "xor", 3) == 0) {
        int N = atoi(argv[argc - 1] + 3);
        for (int i = 0; i < fileCount; i++) {
            xorN(files[i], N);
        }
    } else if (strncmp(argv[argc - 2], "mask", 4) == 0) {
        uint32_t maskValue = strtoul(argv[argc - 1], NULL, 16);
        for (int i = 0; i < fileCount; i++) {
            Errors err2 = mask(files[i], maskValue);
            if (err2 == INVALID_FILE) {
                printf("INVALID_FILE\n");
                return 1;
            }
        }
    } else if (strncmp(argv[argc - 1], "copy", 4) == 0) {
        int N = atoi(argv[argc - 1] + 4);
        if (N > 100) {
            printf("Слишком много копий файлов\n");
            return 1;
        }
        //printf("%d", fileCount);
        for (int i = 0; i < fileCount; i++) {
            copyN(files[i], N);
        }
    } else if (strcmp(argv[argc - 2], "find") == 0) {
        findInFiles(fileCount, files, argv[argc - 1]);
    } else {
        fprintf(stderr, "Неизвестная команда: %s\n", argv[argc - 1]);
        return 1;
    }

    return 0;
}