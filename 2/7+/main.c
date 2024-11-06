#include <stdio.h>
#include <math.h>

double bisection(double a, double b, double eps, double (*fun)(double)) {
    if (fun(a) * fun(b) >= 0) {
        printf("Не решается бисекцией");
        return NAN;
    }

    double mid;
    double fun_point_c;

    while ((b - a) / 2.0 >= eps) {
        mid = (a + b) / 2.0;
        fun_point_c = fun(mid);

        if (fun_point_c == 0.0) {
            return mid;
        } else if (fun(a) * fun_point_c < 0.0) {
            b = mid;
        } else {
            a = mid;
        }
    }

    return (a + b) / 2;
}

double fun1(double x) {
    return x * x - 2;  // 1.4142135
}

double fun2(double x) {
    return cos(x) - x; // 0.739085
}

double fun3(double x) {
    return x * x * x - x - 6; // 6
}

double fun4(double x) {
    return x * x * x - 6 * x * x + 11 * x - 6; // 1, 2, 3
}

int main() {
    double eps = 1e-6;
    //a, b,eps, fun
    double res = bisection(0, 2, eps, fun1);
    if (!isnan(res)) { 
        printf("Корнем уравнения x^2 - 2 = 0 является x = %.6f\n", res);
    } else {
        printf(" x^2 - 2 = 0\n");
    }

    res = bisection(0, 1, eps, fun2);
    if (!isnan(res)) { 
        printf("Корнем уравнения cos(x) - x = 0 является x = %.6f\n", res);
    } else {
        printf(" cos(x) - x = 0\n");
    }

    res = bisection(1, 4, eps, fun3);
    if (!isnan(res)) { 
        printf("Корнем уравнения x^3 - x - 6 = 0 является x = %.6f\n", res);
    } else {
        printf(" x^3 - x - 6 = 0\n");
    }

    res = bisection(2.5, 3.5, eps, fun4); // 0.5, 1.5   1.5, 2.5
    if (!isnan(res)) { 
        printf("Корнем уравнения x^3 - 6x^2 + 11x - 6 = 0 является x = %.6f\n", res);
    } else {
        printf(" x^3 - 6x^2 + 11x - 6 = 0\n");
    }

    return 0;
}