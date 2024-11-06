#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LEN 100 // Определите максимальную длину чисел

// Функция для преобразования символа в цифру
int char_to_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1; // Для случая, если символ не цифра
}

// Функция для преобразования цифры в символ
char digit_to_char(int digit) {
    if (digit >= 0 && digit <= 9) return '0' + digit;
    return 'A' + digit - 10;
}

// Функция для проверки, находится ли число в правильной системе счисления
bool is_valid_number_in_base(char* num, int base) {
    for (int i = 0; num[i] != '\0'; i++) {
        int digit = char_to_digit(num[i]);
        if (digit < 0 || digit >= base) {
            return false; // Если цифра не допустима в этой системе счисления
        }
    }
    return true;
}

void add_numbers_in_base(char* num1, char* num2, int base, char* result) {
    // Проверка корректности чисел в указанной системе счисления
    if (!is_valid_number_in_base(num1, base)) {
        printf("Error: Number %s is not valid in base %d\n", num1, base);
        return;
    }
    if (!is_valid_number_in_base(num2, base)) {
        printf("Error: Number %s is not valid in base %d\n", num2, base);
        return;
    }

    int len1 = strlen(num1);
    int len2 = strlen(num2);
    
    // Обратные индексы для чисел
    int i = len1 - 1, j = len2 - 1, k = 0, carry = 0;

    // Очищаем результат
    memset(result, 0, MAX_LEN + 2);
    
    // Сложение
    while (i >= 0 || j >= 0 || carry) {
        int digit1 = (i >= 0) ? char_to_digit(num1[i--]) : 0;
        int digit2 = (j >= 0) ? char_to_digit(num2[j--]) : 0;
        
        int sum = digit1 + digit2 + carry;
        result[k++] = digit_to_char(sum % base);
        carry = sum / base;
    }
    
    // Результат находится в обратном порядке, перевернем его
    for (int x = 0; x < k / 2; x++) {
        char temp = result[x];
        result[x] = result[k - x - 1];
        result[k - x - 1] = temp;
    }
    
    result[k] = '\0';
    
    // Убираем ведущие нули
    char* non_zero_result = result;
    while (*non_zero_result == '0' && *(non_zero_result + 1) != '\0') {
        non_zero_result++;
    }
    
    // Скопируем результат без ведущих нулей в начало строки
    if (non_zero_result != result) {
        memmove(result, non_zero_result, strlen(non_zero_result) + 1);
    }
}

int main() {
    char result[MAX_LEN + 2];

    // Пример 1: корректные числа в шестнадцатеричной системе
    add_numbers_in_base("1F", "2A", 16, result);
    printf("Result: %s\n", result);

    // Пример 2: число не в правильной системе счисления
    add_numbers_in_base("1G", "2A", 16, result); // G не допустима в 16-ричной системе
}