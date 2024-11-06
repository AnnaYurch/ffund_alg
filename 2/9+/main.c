#include <stdio.h>
#include <stdarg.h>
#include <math.h>

int nod(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void get_prime_dels(int base, int *factors, int *count) {
    int idx = 0;
    for (int i = 2; i <= base; i++) {
        if (base % i == 0) {
            factors[idx++] = i;
            while (base % i == 0) {
                base /= i;
            }
        }
    }
    *count = idx;
}

int has_finite_representation(double fraction, int base) {
    double numer = fraction;
    double denom = 1.0;
    
    while (numer - floor(numer) > 0) {
        numer *= 10;
        denom *= 10;
    }

    int numerator = (int)numer;
    int denominator = (int)denom;

    int del = nod(numerator, denominator);
    numerator /= del;
    denominator /= del;

    int dels[30];
    int dels_count = 0;
    get_prime_dels(base, dels, &dels_count);

    while (denominator > 1) {
        int divisible = 0;
        for (int i = 0; i < dels_count; i++) {
            while (denominator % dels[i] == 0) {
                denominator /= dels[i];
                divisible = 1;
            }
        }
        if (!divisible) {
            return 0;
        }
    }
    return denominator == 1;
}

void check_finite_representations(int base, int count, ...) {
    va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) {
        double fraction = va_arg(args, double);

        if (fraction <= 0 || fraction >= 1) {
            printf("Число %f вне диапазона (0, 1)\n", fraction);
            continue;
        }

        int result = has_finite_representation(fraction, base);
        
        printf("Дробь: %f -> ", fraction);
        if (result) {
            printf("Имеет конечное представление в системе с основанием %d.\n", base);
        } else {
            printf("Не имеет конечного представления в системе с основанием %d.\n", base);
        }
    }
    
    va_end(args);
}

int main() {
    int base = 10;
    printf("Основание: %d\n", base);
    check_finite_representations(base, 3, 0.5, 0.1, 0.3333);

    base = 2;
    printf("Основание: %d\n", base);
    check_finite_representations(base, 3, 0.5, 0.25, 0.1); //0.1, 0.01, 0.000110011...

    base = 8;
    printf("Основание: %d\n", base);
    check_finite_representations(base, 3, 0.5, 0.25, 0.1); //0.4, 0.2, 0.075000...

    return 0;
}