#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LEN 1000

char digit_to_char(int num) {
    if (num >= 0 && num <= 9)
        return '0' + num;
    else if (num >= 10 && num < 36)
        return 'A' + (num - 10);
}

int char_to_digit(char c) {
    if (isdigit(c))
        return c - '0';
    else if (isalpha(c))
        return toupper(c) - 'A' + 10;
    return -1;
}

bool is_valid_number_in_base(char* num, int base) {
    for (int i = 0; num[i] != '\0'; i++) {
        int digit = char_to_digit(num[i]);
        if (digit < 0 || digit >= base) {
            return false;
        }
    }
    return true;
}

void add_to_res_num_in_base(char* num1, char* num2, int base, char* res) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    
    int i = len1 - 1, j = len2 - 1, k = 0, carry = 0;

    // Очищаем результат
    memset(res, 0, MAX_LEN + 2);
    
    while (i >= 0 || j >= 0 || carry) {
        //вытаскиваем цифры для сложения
        int digit1 = (i >= 0) ? char_to_digit(num1[i--]) : 0;
        int digit2 = (j >= 0) ? char_to_digit(num2[j--]) : 0;

        int sum = digit1 + digit2 + carry;
        res[k++] = digit_to_char(sum % base);
        carry = sum / base;
    }
    
    for (int x = 0; x < k / 2; x++) {
        char temp = res[x];
        res[x] = res[k - x - 1];
        res[k - x - 1] = temp;
    }
    
    res[k] = '\0';
    
    char* non_zero_result = res;
    while (*non_zero_result == '0' && *(non_zero_result + 1) != '\0') {
        non_zero_result++;
    }
    
    if (non_zero_result != res) {
        memmove(res, non_zero_result, strlen(non_zero_result) + 1); // перекопируеваем
    }
}

char* sum_in_base(int base, int count, ...) {
    va_list args;
    va_start(args, count);

    static char res[MAX_LEN + 1] = "0";
    char temp_res[MAX_LEN + 1];

    for (int i = 0; i < count; i++) {
        char* next_num = va_arg(args, char*);

        if (!is_valid_number_in_base(next_num, base)) {
            printf("Ошибочка: Число %s не входит в систему счисления %d\n", next_num, base);
            va_end(args);
            return NULL;
        }

        add_to_res_num_in_base(res, next_num, base, temp_res);
        strcpy(res, temp_res);
    }

    va_end(args);
    return res;
}

int main() {
    int base = 10;
    if (base < 2 || base > 36) {
        printf("Система счисления не подходит\n");
        return 1;
    }
    char* res = sum_in_base(base, 3, "00010", "2", "0000000");
    if (res != 0) {
        printf("РЕЗУЛЬТАТ: %s\n", res);
    }
    return 0;
}