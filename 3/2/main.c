#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

// Структура для представления вектора
typedef struct {
    double* coords; // Указатель на массив координат
    int size; // Размер вектора
} Vector;

// Функция для вычисления бесконечной нормы
double norm_infinity(Vector* v) {
    double max_val = fabs(v->coords[0]); // Начальное значение
    for (int i = 1; i < v->size; i++) {
        double val = fabs(v->coords[i]);
        if (val > max_val) {
            max_val = val;
        }
    }
    return max_val; // Возвращаем максимальное значение
}

// Функция для вычисления нормы p
double norm_p(Vector* v, double p) {
    double sum = 0.0; // Начальная сумма
    for (int i = 0; i < v->size; i++) {
        sum += pow(fabs(v->coords[i]), p); // Суммируем |x_j|^p
    }
    return pow(sum, 1.0 / p); // Возвращаем p-норму
}

// Функция для вычисления нормы A
double norm_A(Vector* v, double** A) {
    double result = 0.0;
    for (int i = 0; i < v->size; i++) {
        for (int j = 0; j < v->size; j++) {
            result += A[i][j] * v->coords[j] * v->coords[i]; // Квадратичная форма
        }
    }
    return sqrt(result); // Возвращаем норму
}

// Основная функция для нахождения максимальных норм
void find_max_norms(int n, int vector_count, Vector* vectors[], int func_count, double** A, ...) {
    va_list args;
    va_start(args, A);
    
    // Массивы для хранения максимальных значений и индексов
    double* max_values = malloc(func_count * sizeof(double));
    int* max_indices = malloc(func_count * sizeof(int));
    int* max_counts = malloc(func_count * sizeof(int));
    
    for (int i = 0; i < func_count; i++) {
        max_values[i] = -1.0; // Инициализация максимума
        max_indices[i] = -1; // Инициализация индексов
        max_counts[i] = 0; // Инициализация счетчиков
    }

    // Перебор векторов
    for (int i = 0; i < vector_count; i++) {
        Vector* v = vectors[i];
        for (int j = 0; j < func_count; j++) {
            // Получаем указатель на функцию
            double (*norm_func)(Vector*) = va_arg(args, double(*)(Vector*));
            double norm_value;

            // Для нормы A передаем матрицу A
            if (j == 2) {
                norm_value = norm_A(v, A);
            } else {
                norm_value = norm_func(v);
            }
            
            if (norm_value > max_values[j]) {
                max_values[j] = norm_value; // Обновляем максимальное значение
                max_indices[j] = i; // Обновляем индекс
                max_counts[j] = 1; // Сбрасываем счетчик
            } else if (norm_value == max_values[j]) {
                max_counts[j]++; // Увеличиваем счетчик
            }
        }
    }

    // Вывод результатов
    for (int j = 0; j < func_count; j++) {
        printf("Norm %d: Max Value = %f, Count = %d\n", j + 1, max_values[j], max_counts[j]);
        if (max_counts[j] > 0) {
            printf("Index of vector with max norm: %d\n", max_indices[j]);
        }
    }

    // Освобождение памяти
    free(max_values);
    free(max_indices);
    free(max_counts);
    
    va_end(args);
}

// Пример использования
int main() {
    // Размерность векторов
    int n = 3;

    // Создаем векторы
    int vector_count = 3; // Количество векторов
    Vector* vectors[3]; // Массив указателей на векторы
    for (int i = 0; i < vector_count; i++) {
        vectors[i] = malloc(sizeof(Vector)); // Выделение памяти для структуры
        vectors[i]->size = n; // Установка размерности
        vectors[i]->coords = malloc(n * sizeof(double)); // Выделение памяти для координат
        for (int j = 0; j < n; j++) {
            vectors[i]->coords[j] = rand() % 10; // Заполнение случайными числами
        }
    }

    // Положительно определенная матрица A
    double** A = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        A[i] = malloc(n * sizeof(double)); // Выделение памяти для каждой строки матрицы
        for (int j = 0; j < n; j++) {
            A[i][j] = (i == j) ? 2.0 : 1.0; // Пример матрицы
        }
    }
    // Вызов функции
    find_max_norms(n, vector_count, vectors, 3, A, norm_infinity, norm_p, norm_A);

    // Освобождение памяти
    for (int i = 0; i < vector_count; i++) {
        free(vectors[i]->coords); // Освобождаем память координат
        free(vectors[i]); // Освобождаем память структуры вектора
    }
    
    for (int i = 0; i < n; i++) {
        free(A[i]); // Освобождаем память каждой строки матрицы
    }
    free(A); // Освобождаем память матрицы

    return 0;
}