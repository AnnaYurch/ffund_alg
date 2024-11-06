#include <stdio.h>
#include <stdarg.h> 
#include <math.h>

double geometric_mean(int count, ...) {
    va_list args;
    va_start(args, count);

    double product = 1.0;

    for (int i = 0; i < count; i++) {
        double num = va_arg(args, double);
        product *= num;
    }

    va_end(args);

    return pow(product, 1.0 / count);
}

double power(double base, int exp) {
    if (exp == 0 && base == 0) {
        return NAN;
    } else if (exp == 0) {
        return 1.0;
    } else if (exp == 1) {
        return base;
    } else if (exp % 2 == 0 ) {
        return pow( base * base, exp / 2);
    } else {
        return pow( base * base, exp / 2) * base;
    }
}

int main() {
    printf("Среднее геометрическое: %f\n", geometric_mean(3, 2.0, 8.0, 4.0));
    printf("Среднее геометрическое: %f\n", geometric_mean(4, 1.0, 2.0, 3.0, 4.0));

    double result;

    result = power(2.0, 3);
    if (isnan(result)) {
        printf("Error: 0^0 is undefined.\n");
    } else {
        printf("2.0^3 = %f\n", result);
    }

    result = power(5.0, 0);
    if (isnan(result)) {
        printf("Error: 0^0 is undefined.\n");
    } else {
        printf("5.0^0 = %f\n", result);
    }

    result = power(2.0, -2);
    if (isnan(result)) {
        printf("Error: 0^0 is undefined.\n");
    } else {
        printf("2.0^-2 = %f\n", result);
    }

    result = power(0.0, 0);
    if (isnan(result)) {
        printf("Error: 0^0 is undefined.\n");
    } else {
        printf("0.0^0 = %f\n", result);
    }

    result = power(2.0, 5);
    if (isnan(result)) {
        printf("Error: 0^0 is undefined.\n");
    } else {
        printf("2.0^5 = %f\n", result);
    }

    return 0;
}