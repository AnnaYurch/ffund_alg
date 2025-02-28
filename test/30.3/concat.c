#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
//0 - функция завершилась успешно
//1 - параметр result == NULL
//2 - парпметр first == NULL
//3 - проблемы с памятью
//NULL - последний подаваемый параметр
//добавление очередного символа в строку за o(1). перерасход памяяти для размещения в ней результирующей 
//строки недопустим

int concat(char **result, char *first, ...) {
    if(result == NULL) {
        return 1;
    }
    if(first == NULL) {
        return 2;
    }

    va_list args;
    int len_of_res = 0;
    va_start(args, first);
    len_of_res += strlen(first);
    char *cur_arg = NULL;
    while((cur_arg = va_arg(args, char*)) != NULL) {
        len_of_res += strlen(cur_arg);
    }
    va_end(args);

    *result = (char*)realloc(*result, len_of_res + 1);
    if (*result == NULL) {
        return 3;
    }


    *result = strcat(*result, first);
    va_start(args, first);
    while((cur_arg = va_arg(args, char*)) != NULL) {
        *result = strcat(*result, cur_arg);
    }
    va_end(args);
    return 0;
}

int main() {
    char *str_1 = "a";
    char *str_2 = "b";
    char *str_3 = "c";
    char *str_4 = "d";

    char *res = NULL;

    int err = concat(&res, str_1, str_2, str_3, str_4, NULL);
    switch (err) {
        case 0:
            printf("%s\n", res);
            break;
        case 1:
            printf("параметр result == NULL");
            break;
        case 2:
            printf("параметр first == NULL");
            break;
        case 3:
            printf("проблемы с памятью");
            break;
    }
    return 0;
}