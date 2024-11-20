#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

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
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

char* my_strcat(char *str1, char *str2) {
    int len_1 = my_strlen(str1);
    int len_2 = my_strlen(str2);

    char *res = (char *)malloc(sizeof(len_1 + len_2 + 1));
    if (res == NULL) {
        fprintf(stderr, "Memory yps!\n");
        return NULL;
    }

    for (int i = 0; i < len_1; i++) {
        res[i] = str1[i];
    }

    for (int i = 0; i < len_2; i++) {
        res[len_1 + i] = str2[i];
    }

    str1[len_1 + len_2] = '\0';

    return res;
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

int HandlerOptL(const char *str) {
    return my_strlen(str);
}

char* HandlerOptR(char *str) {
    int count = my_strlen(str);
    int start = 0;
    int end = count - 1;
    while(start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;

        start++;
        end--;
    }

    return str;
}

char* HandlerOptU(char *str) {
    int start = 0;
    while(start < my_strlen(str)) {
        if (start % 2 == 0 ) {
            str[start] = toupper(str[start]);
        }
        start++;
    }
    return str;
}

char* HandlerOptN(const char *str) {
    int len = my_strlen(str);

    char *ar_digit = (char*)malloc(len + 1);
    char *ar_let = (char*)malloc(len + 1);
    char *ar_muut = (char*)malloc(len + 1);

    if (ar_digit == NULL || ar_let == NULL || ar_muut == NULL) {
        printf("Memory yps2!\n");
        return NULL;
    }

    int digit_i = 0, let_i = 0, muut_i = 0;
    int start = 0;

    while (start < len) {
        if (isdigit(str[start])) {
            ar_digit[digit_i++] = str[start];
        } else if (isalpha(str[start])) {
            ar_let[let_i++] = str[start];
        } else {
            ar_muut[muut_i++] = str[start];
        }
        start++;
    }

    ar_digit[digit_i] = '\0';
    ar_let[let_i] = '\0';
    ar_muut[muut_i] = '\0';

    char *final_result = my_strcat(my_strcat(ar_digit, ar_let), ar_muut);

    free(ar_digit);
    free(ar_let);
    free(ar_muut);
    
    return final_result;
}

char* HandlerOptC(int argc, char *argv[], int seed) {
    int count = argc;
    char **new_strs = (char **)malloc(count * sizeof(char *));
    if (new_strs == NULL) {
        printf("Memory yps3");
        return NULL;
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

    char *result = (char *)malloc(1);
    if (result == NULL) {
        printf("Memory yps4");
        free(new_strs);
        return NULL;
    }
    
    result[0] = '\0';

    for (int i = 0; i < count; i++) {
        char *new_res = my_strcat(result, new_strs[i]);
        if (new_res == NULL) {
            printf("Memory yps5");
            free(result);
            free(new_res);
            return NULL;
        } 
        free(result);
        result = new_res;
    }
    
    free(new_strs);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Error: Not enough arguments.\n");
        return 1;
    }
    kOpts opt = GetOptionFromArgs(argv[1]);
    char *x = argv[2];
    
    switch (opt) {
        case OPT_L:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            int n = HandlerOptL(x);
            printf("%d\n", n);
            break;
        case OPT_R:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            printf("%s\n", HandlerOptR(x));
            break;
        case OPT_U:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            printf("%s\n", HandlerOptU(x));
            break;
        case OPT_N:
            if (argc != 3) {
                printf("Error: Not enough arguments.\n");
                return 1;
            }
            char *result = HandlerOptN(x);
            if (result != NULL) {
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
            char *res = HandlerOptC(argc - 3, argv + 3, seed);
            if (res != NULL) {
                printf("%s\n", res);
                free(res);
            }
            break;
        default:
            printf("Unknown option.\n");
            return 1;
    }

    return 0;
}