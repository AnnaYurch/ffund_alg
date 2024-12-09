#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum Errors {
    INVALID_MEMORY,
    OK
} Errors;

typedef enum kOpts {
    OPT_L,
    OPT_R,
    OPT_U,
    OPT_N,
    OPT_C,
    OPT_UNKNOWN
} kOpts;

int my_isdigit(char c) {
    return c >= '0' && c <= '9';
}

int is_seed(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    while (*str != '\0') {
        if (!my_isdigit(*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

int my_strlen(const char* str) {
    const char* s = str;
    while (*s) {
        s++;
    }
    return s - str;
}

int my_atoi(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    while (str[i] == ' ') i++;

    if (str[i] == '-') {
        sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

enum Errors my_strcat(char *str1, const char *str2, char **result) {
    int len_1 = my_strlen(str1);
    int len_2 = my_strlen(str2);
    *result = (char *)malloc(len_1 + len_2 + 1);
    if (*result == NULL) {
        return INVALID_MEMORY;
    }
    for (int i = 0; i < len_1; i++) {
        (*result)[i] = str1[i];
    }
    for (int i = 0; i < len_2; i++) {
        (*result)[len_1 + i] = str2[i];
    }
    (*result)[len_1 + len_2] = '\0';
    return OK;
}

kOpts GetOptionFromArgs(char* flag) {
    if ((flag[0] == '-') && my_strlen(flag) == 2) {
        switch (flag[1]) {
        case 'l':
            return OPT_L;
        case 'r':
            return OPT_R;
        case 'u':
            return OPT_U;
        case 'n':
            return OPT_N;
        case 'c':
            return OPT_C;
        default:
            return OPT_UNKNOWN;
        }
    }
    return OPT_UNKNOWN;
}

Errors HandlerOptR(const char *str, char **new_str) {
    int len = my_strlen(str);
    char *temp = (char*)malloc((len + 1) * sizeof(char));
    if (temp == NULL) {
        return INVALID_MEMORY;
    }

    for (int i = 0; i < len; i++) {
        (temp)[i] = str[len - 1 - i];
    }
    (temp)[len] = '\0';

    *new_str = temp;
    return OK;
}

enum Errors HandlerOptU(char *str, char **result) {
    int len = my_strlen(str);
    char *temp = (char *)malloc(len + 1);
    if (temp == NULL) {
        return INVALID_MEMORY;
    }
    for (int i = 0; i < len; i++) {
        if (i % 2 == 0) {
            (temp)[i] = toupper(str[i]);
        } else {
            (temp)[i] = str[i];
        }
    }
    (temp)[len] = '\0';
    *result = temp;
    return OK;
}

enum Errors HandlerOptN(const char *str, char **result) {
    int len = my_strlen(str);
    char *ar_digit = (char*)malloc(len + 1);
    char *ar_let = (char*)malloc(len + 1);
    char *ar_muut = (char*)malloc(len + 1);

    if (ar_digit == NULL || ar_let == NULL || ar_muut == NULL) {
        return INVALID_MEMORY;
    }

    int digit_i = 0, let_i = 0, muut_i = 0;
    for (int i = 0; i < len; i++) {
        if (isdigit(str[i])) {
            ar_digit[digit_i++] = str[i];
        } else if (isalpha(str[i])) {
            ar_let[let_i++] = str[i];
        } else {
            ar_muut[muut_i++] = str[i];
        }
    }
    ar_digit[digit_i] = '\0';
    ar_let[let_i] = '\0';
    ar_muut[muut_i] = '\0';

    enum Errors err = my_strcat(ar_digit, ar_let, result);
    if (err != OK) {
        free(ar_digit);
        free(ar_let);
        free(ar_muut);
        return err;
    }
    char *temp_result = *result;
    err = my_strcat(temp_result, ar_muut, result);
    free(ar_digit);
    free(ar_let);
    free(ar_muut);
    free(temp_result);
    return err;
}

enum Errors HandlerOptC(int argc, char *argv[], int seed, char **result) {
    int count = argc;
    char **new_strs = (char **)malloc(count * sizeof(char *));

    if (new_strs == NULL) {
        return INVALID_MEMORY;
    }

    for (int i = 0; i < count; i++) {
        new_strs[i] = argv[i];
    }

    srand(seed);

    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = new_strs[i];
        new_strs[i] = new_strs[j];
        new_strs[j] = temp;
    }

    char *temp = (char *)malloc(1);
    if (temp == NULL) {
        free(new_strs);
        return INVALID_MEMORY;
    }
    temp[0] = '\0';

    for (int i = 0; i < count; i++) {
        char *new_res;
        Errors err = my_strcat(temp, new_strs[i], &new_res);
        if (err != OK) {
            free(temp);
            free(new_strs);
            return err;
        }
        free(temp);
        temp = new_res;
    }
    *result = temp;
    free(new_strs);
    return OK;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Error: Not enough arguments (./a.out -flag str)\n");
        return 1;
    }

    kOpts opt = GetOptionFromArgs(argv[1]);
    char *x = argv[2];
    char *result;
    Errors err;
    switch (opt) {
        case OPT_L:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            int n = my_strlen(x);
            printf("%d\n", n);
            break;
        case OPT_R:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }

            char *reverse_str;
            err = HandlerOptR(x, &reverse_str);
            if (err == INVALID_MEMORY) {
                printf("Problems with memory\n");
                break;
            } else {
                printf("%s\n", reverse_str);
                free(reverse_str);
            }
            
            break;
        case OPT_U:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            result;
            err = HandlerOptU(x, &result);
            if (err == INVALID_MEMORY) {
                printf("Problems with memory\n");
                break;
            } else {
                printf("%s\n", result);
                free(result);
            }
            
            break;
        case OPT_N:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            result;
            Errors err = HandlerOptN(x, &result);
            if (err == INVALID_MEMORY) {
                printf("Problems with memory\n");
                break;
            } else {
                printf("%s\n", result);
                free(result);
            }
            
            break;
        case OPT_C:
            if (argc < 4) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            if (is_seed(argv[2]) == 0) {
                printf("Error: seed is not int number.\n");
                return 1;
            }
            int seed = my_atoi(argv[2]);
            char *result;
            err = HandlerOptC(argc - 3, argv + 3, seed, &result);
            if (err == INVALID_MEMORY) {
                printf("Problems with memory\n");
                break;
            } else {
                printf("%s\n", result);
                free(result);
            }
            break;
        default:
            printf("Unknown option.\n");
            return 1;
    }

    return 0;
}