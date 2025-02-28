#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct {
    int (*func)(int);
    float coeff;
} Predicate;

typedef struct {
    int value;    // Значение элемента
    float ording; // Значение для сортировки
} Element;

// Пример предикатов
int pred1(int x) {
    return x % 2; // Четность
}

int pred2(int x) {
    return x > 0; // Положительность
}
static int compare(const void *a, const void *b) {
    float ording_a = ((Element *)a)->ording;
    float ording_b = ((Element *)b)->ording;
    return (ording_a > ording_b) - (ording_a < ording_b);
}

int sort_by_predicates(int *values, size_t values_count, int **result, int ascending, ...) {
    if (values == NULL || result == NULL || values_count == 0) {
        return -1; // Ошибка: неверные входные параметры
    }

    // Подсчет предикатов
    Predicate predicates[10]; // Максимум 10 предикатов
    size_t pred_count = 0;

    va_list args;
    va_start(args, ascending);
    
    while (1) {
        Predicate *pred = va_arg(args, Predicate *);
        if (pred == NULL) {
            break;
        }
        predicates[pred_count++] = *pred;
    }
    va_end(args);

    // Выделение памяти для результата
    *result = malloc(values_count * sizeof(int));
    if (*result == NULL) {
        return -1; // Ошибка: не удалось выделить память
    }

    // Создаем массив элементов для сортировки
    Element *elements = malloc(values_count * sizeof(Element));
    if (elements == NULL) {
        free(*result);
        return -1; // Ошибка: не удалось выделить память
    }

    // Заполняем массив элементов
    for (size_t i = 0; i < values_count; i++) {
        elements[i].value = values[i];
        float sum = 0.0f;
        for (size_t j = 0; j < pred_count; j++) {
            int pred_value = predicates[j].func(values[i]);
            sum += (pred_value == 0 ? 0 : 1) * predicates[j].coeff;
        }
        elements[i].ording = ascending ? sum : -sum; // Учитываем порядок сортировки
    }

    // Сортировка с использованием qsort
    qsort(elements, values_count, sizeof(Element), compare);

    // Заполняем результат отсортированными значениями
    for (size_t i = 0; i < values_count; i++) {
        (*result)[i] = elements[i].value;
    }

    // Освобождаем память
    free(elements);
}



int main() {
    int values[] = {3, -1, 0, 2, -2, 1};
    size_t values_count = sizeof(values) / sizeof(values[0]);
    int *result = NULL;

    Predicate p1 = {pred1, 1.0}; // Предикат 1 с коэффициентом 1.0
    Predicate p2 = {pred2, 2.0}; // Предикат 2 с коэффициентом 2.0

    if (sort_by_predicates(values, values_count, &result, 1, &p1, &p2, NULL) == 0) {
        for (size_t i = 0; i < values_count; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
        free(result);
    } else {
        printf("Ошибка сортировки.\n");
    }

    return 0;
}
