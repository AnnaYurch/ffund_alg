#include <stdio.h>
#include <math.h>
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
            //HandlerOptC(x);
            break;
        default:
            printf("Unknown option.\n");
            return 1;
    }

    return 0;
}