#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    OK
} Errors;

//векторное произведение
double vector_product(double x1, double y1, double x2, double y2, double x3, double y3) {
    return (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
}

Errors is_convex(int *result, int num_vertices, ...) {
    if (num_vertices < 3) {
        return INVALID_ARG;
    }
    va_list args;
    va_start(args, num_vertices);

    double x[num_vertices], y[num_vertices];

    for (int i = 0; i < num_vertices; i++) {
        x[i] = va_arg(args, double);
        y[i] = va_arg(args, double);
    }
    va_end(args);

    int sign = 0;

    for (int i = 0; i < num_vertices; i++) {
        double current_vector_product = vector_product(
            x[i],
            y[i],
            x[(i + 1) % num_vertices],
            y[(i + 1) % num_vertices],
            x[(i + 2) % num_vertices],
            y[(i + 2) % num_vertices]
        );

        if (current_vector_product != 0) {
            if (sign == 0) {
                if (current_vector_product > 0) {
                    sign = 1;
                }
                else {
                    sign = -1;
                }
            }
            else if ((current_vector_product > 0 && sign < 0) || (current_vector_product < 0 && sign > 0)) {
                *result = 0;
                return OK;
            }
        }
    }
    *result = 1;
    return OK;
}

double calc_polinomial(double xx, int n, ...) {
    double res = 0;

    va_list nums;
    va_start(nums, n);
    for (int i = 0; i <= n; ++i) {
        res = xx * res + va_arg(nums, double);
    }
    va_end(nums);
    return res;
}

void itoa_base(int value, char *str, int base) {
    char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i = 0;

    do {
        str[i++] = digits[value % base];
        value /= base;
    } while (value > 0);

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

Errors is_kaprecar_num(char *str, int base, int *result) {
    int dem_num = strtol(str, NULL, base);
    int square_num = dem_num * dem_num;
    int max_length = (int)(log(square_num) / log(base)) + 1; //макс длина в base
    char *square_str = malloc(max_length + 1);
    if (square_str == NULL) {
        return INVALID_MEMORY;
    }
    itoa_base(square_num, square_str, base);

    int square_len = strlen(square_str);

    for (int i = 0; i <= square_len; i++) {
        char *left = malloc(square_len + 1);
        char *right = malloc(i + 1);

        if (left == NULL || right == NULL) {
            free(square_str);
            free(left);
            free(right);
            return INVALID_MEMORY;
        }

        strncpy(right, square_str + square_len - i, i); //Количество символов, которые нужно скопировать из src в right
        right[i] = '\0';
        strncpy(left, square_str, square_len - i);
        left[square_len - i] = '\0';

        int left_num = strtol(left, NULL, base);
        int right_num = strtol(right, NULL, base);

        if (dem_num == left_num + right_num && right_num > 0) {
            free(square_str);
            free(left);
            free(right);
            *result = 1;
            return OK;
        }
        free(left);
        free(right);
    }

    free(square_str);
    *result = 0;
    return OK;
}

Errors find_kaprecar_num(int base, int n, ...) {
    if (n <= 0) {
        return INVALID_ARG;
    }
    va_list nums;
    va_start(nums, n);
    for (int i = 0; i < n; ++i) {
        char *str = va_arg(nums, char*);
        int dem_num = strtol(str, NULL, base);
        if (dem_num < 0) {
            return INVALID_ARG;
        }
        int res;
        Errors err = is_kaprecar_num(str, base, &res);
        if (err != OK) {
            return err;
        }

        if (res == 1) {
            printf("%s - число Капрекара\n", str);
        } else {
            printf("%s - не число Капрекара\n", str);
        }
    }

    va_end(nums);
    return OK;
}

int main() {
    ///////////////////////////////////////////////// выпуклый ли многоугольник 1, впуклый 0
    int result11 = 0;
    int num_vertices1 = 5;
    Errors err1 = is_convex(&result11, num_vertices1, 0.0, 0.0, 0.0, 5.0, 5.0, 5.0, 3.0,3.0, 5.0,0.0);
    if (err1 == INVALID_ARG) {
        printf("Ошибка в подаваемых аргументах\n");
    } else if (err1 == INVALID_MEMORY) {
        printf("Ошибка с памятью\n");
    } else if (result11 == 1) {
        printf("Многоугольник выпуклый\n");
    } else {
        printf("Многоугольник не выпуклый\n");
    }

    int result12 = 0;
    int num_vertices2 = 5;
    Errors err2 = is_convex(&result12, num_vertices2, 0.0, 3.0, 3.0, 3.0, 5.0, 8.0, 6.0, 6.0, 3.0, 0.0);
    if (err2 == INVALID_ARG) {
        printf("Ошибка в подаваемых аргументах\n");
    } else if (err2 == INVALID_MEMORY) {
        printf("Ошибка с памятью\n");
    } else if (result12 == 1) {
        printf("Многоугольник выпуклый\n");
    } else {
        printf("Многоугольник не выпуклый\n");
    }
    ///////////////////////////////////////////////// вычисление значения многочлена
    double xx2 = 3;
    int n2 = 4;
    double result2 = calc_polinomial(xx2, n2, 2.0, 1.0, 3.0, 0.0, 1.0); //от коэф старшей до свободного члена
    printf("fun2 result = %f\n", result2);
    ///////////////////////////////////////////////// числа Капрекара кв числа можно разбить на две части
                                                   // 45^2 = 2025 || 45 = 20 + 25 
    int base3 = 10;
    Errors result3 = find_kaprecar_num(base3, 6, "45", "297", "9", "2", "10", "22"); // 3 капр 3 не капр
    if (result3 == INVALID_ARG) {
        printf("Ошибка в подаваемых аргументах\n");
    } else if (result3 == INVALID_MEMORY) {
        printf("Ошибка с памятью\n");
    } else {
        printf("Конец\n");
    }
    
    return 0;
}