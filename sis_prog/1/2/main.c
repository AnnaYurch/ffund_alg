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
    OK
} Errors;

Errors xorN(const char *filename, int N) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return INVALID_FILE;
    }

    int blockSize = 1 << N;
    uint8_t *buffer = (uint8_t *)malloc(blockSize);
    if (!buffer) {
        perror("Ошибка выделения памяти");
        fclose(file);
        return INVALID_MEMORY;
    }

    uint8_t xorResult = 0;
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, blockSize, file)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            xorResult ^= buffer[i];
        }
    }

    printf("XOR-%d для файла %s: %02X\n", N, filename, xorResult);

    free(buffer);
    fclose(file);
    return OK;
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
        /*
        int N = atoi(argv[argc - 1] + 3);
        for (int i = 0; i < fileCount; i++) {
            Errors err1 = xorN(files[i], N);
            if (err1 == INVALID_FILE) {
                printf("INVALID_FILE\n");
                return 1;
            } 
        }
        */
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
        /*
        findInFiles(fileCount, files, argv[argc - 1]);
        */
    } else {
        fprintf(stderr, "Неизвестная команда: %s\n", argv[argc - 1]);
        return 1;
    }

    return 0;
}