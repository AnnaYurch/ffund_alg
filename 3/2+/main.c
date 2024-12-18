#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

typedef enum Errors {
    INVALID_MEMORY,
    INVALID_ARG,
    OK
} Errors;

typedef struct vector {
    double *cordinates;
} vector;

void printf_vertor(vector *v, int n, double norm_value) {
    printf("v = ( ");
    for (int i = 0; i < n; i++){
        printf("%.3lf ", v->cordinates[i]);
    }
    printf(") - %f\n", norm_value);
}

Errors norm_max(double *res, vector *v, int n, int p) {
    if (!v || !res) {
        return INVALID_ARG;
    }
    *res = fabs(v->cordinates[0]);
    for (int i = 1; i < n; i++) {
        if (fabs(v->cordinates[i]) > *res) {
            *res = fabs(v->cordinates[i]);
        }
    }
    return OK;
}

Errors norm_p(double *res, vector *v, int n, int p) {
    if (!v || !res || p < 1) {
        return INVALID_ARG;
    }

    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        sum += pow(fabs(v->cordinates[i]), p);
    }

    *res = pow(sum, 1.0 / p);
    return OK;
}

Errors norm_A(double *res, vector *v, int n, int p) {
    if (!v || !res) {
        return INVALID_ARG;
    }

    double A[n][n];
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if (i == j){
                A[i][j] = 1.0; //* (j + 1)
            }else{
                A[i][j] = 0.0;
            }
        }
    }
    // Вычисляем Ax
    double *Ax = (double *)malloc(n * sizeof(double));
    if (Ax == NULL) {
        return INVALID_MEMORY;
    }

    for (int i = 0; i < n; i++) {
        Ax[i] = 0.0;
        for (int j = 0; j < n; j++) {
            Ax[i] += A[i][j] * v->cordinates[j];
        }
    }

    // Вычисляем (Ax, x) (T)
    double scalar_product = 0.0;
    for (int i = 0; i < n; i++) {
        scalar_product += Ax[i] * v->cordinates[i];
    }

    *res = pow(scalar_product, 0.5);
    return OK;
}

Errors max_vect(int n, int p, Errors (*norm)(double *res, vector *v, int n, int p), int count, ...) {
    if (n <= 0) {
        return INVALID_ARG;
    }
    
    va_list args;
    va_start(args, count);
    vector vectors[count];
    for (int i = 0; i < count; i++) {
        vectors[i] = va_arg(args, vector);
    } 
    va_end(args);

    double max_norm = -1.0;
    for (int i = 0; i < count; i++) {
        double norm_value;
        Errors err = norm(&norm_value, &vectors[i], n, p);
        if (err != OK) {
            return err;
        }
        if (norm_value > max_norm) {
            max_norm = norm_value;
        }
    }

    for (int j = 0; j < count; ++j) {
        double norm_value;
        Errors err = norm(&norm_value, &vectors[j], n, p);
        if (err != OK) {
            return err;
        }
        if (norm_value == max_norm) {
            printf_vertor(&vectors[j], n, norm_value);
        }
    }


    return OK;
}

int main() {
    int n = 3;
    int p = 2;
    vector v1;
    vector v2;
    vector v3;

    v1.cordinates = (double *)malloc(n * sizeof(double));
    if (v1.cordinates == NULL) {
        printf("Ошибка выделения памяти1\n");
        return 1;
    }

    v2.cordinates = (double *)malloc(n * sizeof(double));
    if (v2.cordinates == NULL) {
        free(v1.cordinates);
        printf("Ошибка выделения памяти2\n");
        return 1;
    }

    v3.cordinates = (double *)malloc(n * sizeof(double));
    if (v3.cordinates == NULL) {
        free(v1.cordinates);
        free(v2.cordinates);
        printf("Ошибка выделения памяти3\n");
        return 1;
    }

    v1.cordinates[0] = 1.3;
    v1.cordinates[1] = 2.0;
    v1.cordinates[2] = -5.5;

    v2.cordinates[0] = 7.6;
    v2.cordinates[1] = -2.8;
    v2.cordinates[2] = -5.0;

    v3.cordinates[0] = -1.2;
    v3.cordinates[1] = 8.0;
    v3.cordinates[2] = 6.5;

    printf("Norm_max: ");
    Errors err1 = max_vect(n, p, norm_max, 3, v1, v2, v3);
    if (err1 == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    } else if (err1 == INVALID_ARG) {
        printf("INVALID_ARG\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    }

    printf("Norm_p: ");
    Errors err2 = max_vect(n, p, norm_p, 3, v1, v2, v3);
    if (err2 == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    } else if (err2 == INVALID_ARG) {
        printf("INVALID_ARG\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    }

    printf("Norm_A: ");
    Errors err3 = max_vect(n, p, norm_A, 3, v1, v2, v3);
    if (err3 == INVALID_MEMORY) {
        printf("INVALID_MEMORY\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    } else if (err3 == INVALID_ARG) {
        printf("INVALID_ARG\n");
        free(v1.cordinates);
        free(v2.cordinates);
        free(v3.cordinates);
        return 1;
    }
    

    free(v1.cordinates);
    free(v2.cordinates);
    free(v3.cordinates);

    return 0;
}