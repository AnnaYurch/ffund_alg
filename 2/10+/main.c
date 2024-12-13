#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    OK
} Errors;

double fastpow(double base, int exp) {
    if (exp == 0) {
        return 1.0;
    }

    if (exp < 0) {
        return 1.0 / fastpow(base, -exp);
    }

    if (exp % 2 == 0) {
        double half = fastpow(base, exp / 2);
        return half * half;
    } else {
        return base * fastpow(base, exp - 1);
    }
    
}

Errors calc_polinomial(double a, int count, double *temp, double *gg) {
    if (temp == NULL || gg == NULL) {
        return INVALID_ARG;
    }
    double res = *temp;
    for (int i = 1; i <= count; ++i) {
        res += temp[i] * fastpow(a, i);
    }
    *gg = res;
    return OK;
}

Errors polinomial_decomposition(double eps, double a, double **arr, int n, ...) {
    if (arr == NULL || eps <= 0) {
        return INVALID_ARG;
    }

    double *temp = (double*)malloc(sizeof(double) * (n + 1));
    if (temp == NULL) {
        return INVALID_MEMORY;
    }

    *arr = (double*)malloc(sizeof(double) * (n + 1));
    if (*arr == NULL) {
        free(temp);
        return INVALID_MEMORY;
    }

    va_list arg;
    va_start(arg, n);

    for (int i = 0; i <= n; ++i) {
        temp[i] = va_arg(arg, double);
        //printf("%lf ", temp[i]);
    }
    //printf("\n");
    va_end(arg);

    unsigned long int fact = 1;
    Errors err;
    double prev_val = 0.0;

    for (int i = 0; i <= n; ++i) {
        double gg;
        err = calc_polinomial(a, n - i, temp, &gg);
        //printf("%lf ", gg);

        if (err != OK) {
            free(temp);
            free(*arr);
            return err;
        }
        
        (*arr)[i] = gg / fact;
        if (i > 0 && fabs((*arr)[i] - prev_val) < eps) {
            break;
        }

        fact *= (i + 1);
        prev_val = (*arr)[i];
        //printf("%lf ", (*arr)[i]);

        //дифф
        for (int j = 0; j <= n - i - 1; ++j) {
            temp[j] = (j + 1) * temp[j + 1];
            //printf("%d - %lf\n", j, temp[j]);
        }
    }
    //printf("\n");
    free(temp);
    return OK;
}

int main() {
    double eps = 0.00001;
    double a = 3;
    double *arr;
    int n = 4;

    Errors err = polinomial_decomposition(eps, a, &arr, n, -2.0, 1.0, -3.0, 0.0, 1.0);
    if (err == INVALID_MEMORY) {
        printf("Problems with memory\n");
    } else if(err == INVALID_ARG) {
        printf("Problems with arg\n");
    } else {
        for (int i = 0; i <= n; i++) {
            printf("g%d = %lf\n", i, arr[i]);
        }
        free(arr);
    }

    
    return 0;
}