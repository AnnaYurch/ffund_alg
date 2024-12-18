#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

typedef enum errorCode{
    OPT_SUCCESS,
  OPT_ERROR_ARGUMENTS,
    OPT_ERROR_MEMORY
}errorCode;

typedef struct vector{
    double* coordinates;
}vector;

void printVector(vector* v, int n){
    
    static int count = 1;
    printf("%d. v = ( ", count);
    for (int i = 0; i < n; i++){
        printf("%.2lf ", v->coordinates[i]);
    }
    printf(")\n");
    count++;
}

double fast_pow(double val, int p){
    if (p < 0){ return fast_pow(1.0 / val, -p);}
    else if (p == 0){return 1.0;}
    else if (p % 2 == 0){return fast_pow(val*val, p/2);}
    else{return val * fast_pow(val * val,(p - 1) / 2);}
}

double normMax(vector* v, int n, int p){
    double max = fabs(v->coordinates[0]);
    for (int i = 0; i < n; i++){
        if (fabs(v->coordinates[i]) > max){
            max = fabs(v->coordinates[i]);
        }
    }
    return max;
}

double normP(vector* v, int n, int p){
    double sum = 0.0;
    for (int i = 0; i < n; i++){
        sum += fast_pow(v->coordinates[i], p);
    }
    return fast_pow(sum, -p);
}

double normMatrix(vector* v, int n, int p){
    double A[n][n];
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if (i == j){
                A[i][j] = 1.0;
            }else{
                A[i][j] = 0.0;
            }
        }
    }
    vector res;
    res.coordinates = (double*)malloc(n * sizeof(double));
    double result = 0.0;
    for (int i = 0; i < n; i++){
        double tmp = 0.0;
        for (int j = 0; j < n; j++){
            tmp += A[i][j] * v->coordinates[j];
        }
        res.coordinates[i] = tmp;
    }
    for (int k = 0; k < n; k++){
        result += res.coordinates[k] * v->coordinates[k];
    }
    free(res.coordinates);
    return sqrt(result);
}

void foundMax(double norma(vector* v, int n, int p), int p, int n, int count, ...){
   va_list args;
   va_start(args, count);
   vector vectors[count];
   for (int i = 0; i < count; i++){
        vectors[i] = va_arg(args, vector);
    } 
    va_end(args);
    double max_norm = -1.0;
    vector* max_vector = NULL;
    for (int k = 0; k < count; k++){
        double norm = norma(&vectors[k], n, p);
        if (norm > max_norm){
            max_norm = norm;
            
        }
    }
    for (int j = 0; j < count; j++){
        if (norma(&vectors[j], n, p) == max_norm){
            max_vector = &vectors[j];
            printVector(max_vector, n);
        }

    }

}

int main(int argc, char* argv[]){
    int n = 3;
    int p = 2;
    vector v_1, v_2, v_3;
    v_1.coordinates = (double*)malloc(n * sizeof(double));
    if (v_1.coordinates == NULL){
        return OPT_ERROR_MEMORY;
    }
    v_1.coordinates[0] = 1.3;
    v_1.coordinates[1] = 2.0;
    v_1.coordinates[2] = -5.5;

    v_2.coordinates = (double*)malloc(n * sizeof(double));
    if (v_2.coordinates == NULL){
        free(v_1.coordinates);
        return OPT_ERROR_MEMORY;
    }
    v_2.coordinates[0] = 7.6;
    v_2.coordinates[1] = -2.8;
    v_2.coordinates[2] = -5.0;

    v_3.coordinates = (double*)malloc(n * sizeof(double));
    if (v_3.coordinates == NULL){
        free(v_1.coordinates);
        free(v_2.coordinates);
        return OPT_ERROR_MEMORY;
    }
    v_3.coordinates[0] = -1.2;
    v_3.coordinates[1] = 8.0;
    v_3.coordinates[2] = 6.5;

    printf("normMax:\n");
    foundMax(normMax, p, n, 3, v_1, v_2, v_3);
    printf("normP:\n");
    foundMax(normP, p, n, 3, v_1, v_2, v_3);
    printf("normMatrix:\n");
    foundMax(normMatrix, p, n, 3, v_1, v_2, v_3);

    free(v_1.coordinates);
    free(v_2.coordinates);
    free(v_3.coordinates);
}

/////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct matrix {
    double **data; // Двумерный массив для хранения матрицы
    int size;      // Размерность матрицы (n)
} matrix;

Errors norm_A(double *res, vector *v, int n, int p, matrix *A) {
    if (!v || !res || !A || A->size != n) {
        return INVALID_ARG;
    }

    // Вычисляем Ax
    double *Ax = (double *)malloc(n * sizeof(double));
    if (Ax == NULL) {
        return INVALID_MEMORY;
    }

    for (int i = 0; i < n; i++) {
        Ax[i] = 0.0;
        for (int j = 0; j < n; j++) {
            Ax[i] += A->data[i][j] * v->cordinates[j]; // Ax[i] = sum(A[i][j] * x[j])
        }
    }

    // Вычисляем (Ax, x)
    double dot_product = 0.0;
    for (int i = 0; i < n; i++) {
        dot_product += Ax[i] * v->cordinates[i]; // (Ax, x) = sum(Ax[i] * x[i])
    }

    // Вычисляем норму
    *res = sqrt(dot_product);

    free(Ax);
    return OK;
}
