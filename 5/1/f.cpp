#include <iostream>
#include <bitset>
#include <limits>

class binary_int {
private:
    int value; // Основное числовое значение, которое храним

    // Метод для сложения двух чисел с использованием только побитовых операций
    int add(int a, int b) const {
        while (b != 0) {
            int carry = a & b; // находим перенос
            a = a ^ b;         // складываем без учета переноса
            b = carry << 1;    // переносим биты
        }
        return a;
    }

    // Метод для вычитания двух чисел с использованием только побитовых операций
    int subtract(int a, int b) const {
        return add(a, negate(b));
    }

    // Метод для умножения двух чисел с использованием только побитовых операций
    int multiply(int a, int b) const {
        int result = 0;
        while (b != 0) {
            if (b & 1) {
                result = add(result, a);
            }
            a <<= 1;
            b >>= 1;
        }
        return result;
    }

    // Метод для смены знака числа
    int negate(int x) const {
        return add(~x, 1); // Инверсия всех битов и прибавление 1
    }

public:
    // Конструктор
    binary_int(int val = 0) : value(val) {}

    // Перегрузка унарного минуса
    binary_int operator-() const {
        return binary_int(negate(value));
    }

    // Префиксный инкремент
    binary_int& operator++() {
        value = add(value, 1);
        return *this;
    }

    // Постфиксный инкремент
    binary_int operator++(int) {
        binary_int temp = *this;
        value = add(value, 1);
        return temp;
    }

    // Префиксный декремент
    binary_int& operator--() {
        value = subtract(value, 1);
        return *this;
    }

    // Постфиксный декремент
    binary_int operator--(int) {
        binary_int temp = *this;
        value = subtract(value, 1);
        return temp;
    }

    // Оператор +=
    binary_int& operator+=(const binary_int& other) {
        value = add(value, other.value);
        return *this;
    }

    // Оператор +
    binary_int operator+(const binary_int& other) const {
        return binary_int(add(value, other.value));
    }

    // Оператор -=
    binary_int& operator-=(const binary_int& other) {
        value = subtract(value, other.value);
        return *this;
    }

    // Оператор -
    binary_int operator-(const binary_int& other) const {
        return binary_int(subtract(value, other.value));
    }

    // Оператор *=
    binary_int& operator*=(const binary_int& other) {
        value = multiply(value, other.value);
        return *this;
    }

    // Оператор *
    binary_int operator*(const binary_int& other) const {
        return binary_int(multiply(value, other.value));
    }

    // Оператор <<= (сдвиг влево с модификацией объекта)
    binary_int& operator<<=(int shift) {
        value <<= shift;
        return *this;
    }

    // Оператор << (сдвиг влево с возвратом нового объекта)
    binary_int operator<<(int shift) const {
        return binary_int(value << shift);
    }

    // Оператор >>= (сдвиг вправо с модификацией объекта)
    binary_int& operator>>=(int shift) {
        value >>= shift;
        return *this;
    }

    // Оператор >> (сдвиг вправо с возвратом нового объекта)
    binary_int operator>>(int shift) const {
        return binary_int(value >> shift);
    }

    // Перегрузка оператора вывода для печати в двоичном формате
    friend std::ostream& operator<<(std::ostream& os, const binary_int& bi) {
        os << std::bitset<32>(bi.value);
        return os;
    }

    // Метод для получения двух объектов с половинами бит
    std::pair<binary_int, binary_int> split_bits() const {
        int high_bits = value & 0xFFFF0000; // Старшие 16 бит
        int low_bits = value & 0x0000FFFF;  // Младшие 16 бит
     ;   return {binary_int(high_bits), binary_int(low_bits)};
    }
};
/*

int main() {
    // Инициализация объектов
    binary_int a(5);  // a = 5
    binary_int b(3);  // b = 3

    // Демонстрация перегрузки оператора вывода (вывод в двоичном формате)
    std::cout << "a = " << a << std::endl;  // Вывод a в двоичном формате
    std::cout << "b = " << b << std::endl;  // Вывод b в двоичном формате

    // Демонстрация оператора +=
    a += b;  // a = a + b
    std::cout << "a += b -> a = " << a << std::endl;  // Вывод a после сложения

    // Демонстрация оператора +
    binary_int c = a + b;  // c = a + b
    std::cout << "a + b = " << c << std::endl;  // Вывод c (результат сложения)

    // Демонстрация оператора -=
    a -= b;  // a = a - b
    std::cout << "a -= b -> a = " << a << std::endl;  // Вывод a после вычитания

    // Демонстрация оператора -
    binary_int d = a - b;  // d = a - b
    std::cout << "a - b = " << d << std::endl;  // Вывод d (результат вычитания)

    // Демонстрация оператора *=
    a *= b;  // a = a * b
    std::cout << "a *= b -> a = " << a << std::endl;  // Вывод a после умножения

    // Демонстрация оператора *
    binary_int e = a * b;  // e = a * b
    std::cout << "a * b = " << e << std::endl;  // Вывод e (результат умножения)

    // Демонстрация префиксного инкремента
    ++a;  // a = a + 1
    std::cout << "++a -> a = " << a << std::endl;  // Вывод a после префиксного инкремента

    // Демонстрация постфиксного инкремента
    binary_int f = a++;  // f = a, потом a = a + 1
    std::cout << "f = a++ -> f = " << f << ", a = " << a << std::endl;  // Вывод f и a

    // Демонстрация префиксного декремента
    --a;  // a = a - 1
    std::cout << "--a -> a = " << a << std::endl;  // Вывод a после префиксного декремента

    // Демонстрация постфиксного декремента
    binary_int g = a--;  // g = a, потом a = a - 1
    std::cout << "g = a-- -> g = " << g << ", a = " << a << std::endl;  // Вывод g и a

    // Демонстрация сдвига влево
    binary_int h = a << 2;  // Сдвиг a на 2 бита влево
    std::cout << "a << 2 -> h = " << h << std::endl;  // Вывод h после сдвига влево

    // Демонстрация сдвига вправо
    binary_int i = a >> 2;  // Сдвиг a на 2 бита вправо
    std::cout << "a >> 2 -> i = " << i << std::endl;  // Вывод i после сдвига вправо

    // Демонстрация метода split_bits (делим на старшие и младшие биты)
    auto [high, low] = a.split_bits();  // Получаем старшие и младшие биты
    std::cout << "High bits: " << high << ", Low bits: " << low << std::endl;  // Вывод старших и младших битов

    return 0;
}
*/
int main() {
    binary_int a(10);
    binary_int b(3);

    std::cout << "a = " << a << " (" << 10 << ")\n";
    std::cout << "b = " << b << " (" << 3 << ")\n";
    binary_int c = a + b;
    std::cout << "a + b = " << c << " (" << (10 + 3) << ")\n";

    c -= binary_int(5);
    std::cout << "c - 5 = " << c << " (" << ((10 + 3) - 5) << ")\n";

    binary_int d = a * b;
    std::cout << "a * b = " << d << " (" << (10 * 3) << ")\n";

    std::pair<binary_int, binary_int> split = a.split_bits();
    std::cout << "Split a: High bits = " << split.first << ", Low bits = " << split.second << "\n";

    return 0;
}