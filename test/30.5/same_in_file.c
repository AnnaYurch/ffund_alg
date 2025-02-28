#include <stdio.h>

//на вход программе на си продаются через аргументы командной строки пути ко входному и выходному текстовым файлам
//входной файл содержит только символы букв, цифр, пробелов, табуляций и переносов строк
//программа должна преобразовать входной файл в выходной, заменив последовательности подряд идущих
//одинаковых символов одним символом с указанием исходного количества подряд идущих одинаковых символов
//пример: 
//было: Tthis iss Inpp2pput 
//стало: Tthis is(2) Inp(2)2p(2)ut
//при реализации используйте цикл чтения файла с обработкой символа конца файла и принцип границы
typedef enum Errors{
    INVALID_MEMORY,
    INVALID_ARG,
    INVALID_FILE,
    OK
} Errors;

Errors compress(const char *name_input, const char *name_output) {
    FILE *input = fopen(name_input, "r");
    if (input == NULL) {
        return INVALID_FILE;
    }
    FILE *output = fopen(name_output, "w");
    if (output == NULL) {
        fclose(input);
        return INVALID_FILE;
    }

    int count = 0;
    int current;
    int previous = EOF;

    while ((current = getc(input)) != EOF) {
        if (current == previous) {
            count++;
        } else if (current != previous && count > 0 && previous != EOF) {
            fprintf(output, "%c(%d)", previous, count + 1);
            count = 0;
        } else if (previous != EOF){
            fprintf(output, "%c", previous);
        }
        previous = current;
    }

    if (count > 0) {
        fprintf(output, "%c(%d)", previous, count + 1);
    } else if (previous != EOF) {
        fprintf(output, "%c", previous);
    }

    fclose(input);
    fclose(output);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("we need more args\n");
        return 1;
    }

    Errors err = compress(argv[1], argv[2]);
    if (err == INVALID_ARG) {
        printf("INVALID_ARG\n");
        return 1;
    } else if (err == INVALID_FILE) {
        printf("INVALID_FILE\n");
        return 1;
    } else if (err == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
        return 1;
    }

    return 0;
}
