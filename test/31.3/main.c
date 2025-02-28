#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>
//нужно реализовать свой fprintf
//format - указатель на форматную строку, содержащую некоторый текст и, возможно, форматные флаги:
//%c - вывод символа
//%s - вывод строки
//%d{base} - вывод целого знакового значения в сс с основанием base (от 2 до 36)
//%p - вывод значения указателя в шестнадцатеричном формате
//%f - вывод вещественного числа одинарной точности в формате представления вещественного числа float (в сс с основанием 16)
//%F - вывод вещественного числа двойной точности в формате представления вещественного числа double (в сс с основанием 16)
//%% - вывод символа %
//возвращаемое значение - количество символов, напечатанных в поток вывода
//продемонстрировать работу функции в main

size_t own_fprintf(FILE *output_stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    size_t count = 0;
    while (*format) {
        if(*format == '%') {
            format++;
            if (*format == 'c') {
                fputc(va_arg(args, int), output_stream);
                count++;
            } else if (*format == 's') {
                char *str = va_arg(args, char*);
                while (*str) {
                    fputc((*str++), output_stream);
                    count++;
                }
            } else if (*format == 'd') {
                format++; // Переход к основанию
                int base = 10; // По умолчанию десятичная система
                if (*format >= '0' && *format <= '9') {
                    base = *format - '0'; // Получаем основание
                    format++;
                }
                if (*format >= '0' && *format <= '9') {
                    base = base * 10 + *format - '0'; // Получаем основание
                    format++;
                }

                int d = va_arg(args, int);
                if (base < 2 || base > 36) {
                    base = 10; // Если основание неверное, используем десятичное
                }

                // Выводим число в указанной системе счисления
                char buffer[36]; // max length for base 36
                int i = 0;
                unsigned int ud = d < 0 ? -d : d; // Обработка отрицательных чисел
                do {
                    buffer[i++] = "0123456789abcdefghijklmnopqrstuvwxyz"[ud % base];
                    ud /= base;
                } while (ud > 0);
                if (d < 0) {
                    fputc('-', output_stream);
                    count++;
                }
                while (i > 0) {
                    fputc(buffer[--i], output_stream);
                    count++;
                }
            } else if (*format == 'p') {
                void *ptr = va_arg(args, void*);
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "0x%lx", (long)ptr); // Форматируем в шестнадцатеричном виде
                for (char *p = buffer; *p; p++) {
                    fputc(*p, output_stream);
                    count++;
                }
            } else if (*format == 'f') {
                float f = va_arg(args, double);
                // Преобразуем float в шестнадцатеричное представление
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "%a", f); // Используем snprintf для преобразования
                for (char *p = buffer; *p; p++) {
                    fputc(*p, output_stream);
                    count++;
                }
            } else if (*format == 'F') {
                double d = va_arg(args, double);
                // Преобразуем float в шестнадцатеричное представление
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "%a", d); // Используем snprintf для преобразования
                for (char *p = buffer; *p; p++) {
                    fputc(*p, output_stream);
                    count++;
                }
            } else if (*format == '%') {
                fputc('%', output_stream);
                count++;
            } else {
                fputc('%', output_stream);
                count++;
                fputc(*format, output_stream);
                count++;
            }

        } else {
            fputc(*format, output_stream);
            count++;
        }
        format++;
    }

    va_end(args);
    return count;
}

int main() {
    char ch = 'r';
    const char *str = "Hello, World!";
    int d = 8;
    float b = 3.14;
    double c = 3.14;
    
    
    size_t printed_size = own_fprintf(stdout, "Char: %c, String: %s, Integer: %d8, Pointer: %p, Float: %f, Double: %F, Percent: %%\n", ch, str, d, (void*)&d, b, c);
    printf("Total printed characters: %zu\n", printed_size);
    fprintf(stdout, "Char: %c, String: %s, Integer: %d10, Pointer: %p, Float: %f, Double: %F, Percent: %%\n", ch, str, d, (void*)&d, b, c);
    return 0;

}