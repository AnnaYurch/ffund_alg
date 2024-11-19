#include <iostream>
#include <bitset>

class binary_int {
private:
    int value;

    int add(int a, int b) const {
        while(b != 0) {
            int carry = a & b;
            a = a ^ b;
            b = carry << 1;
        }
        return a;
    }

    int negate(int a) const {
        return add(~a, 1);
    }

    int subtract(int a, int b) const {
        return add(a, negate(b));
    }

    int multiply (int a, int b) const {
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

public:
    binary_int(int val = 0) : value(val) {}

    int get_value() const {
        return value;
    }

    binary_int operator-() const {
        return binary_int(negate(value));
    }

    binary_int &operator++() {
        value = add(value, 1);
        return *this;
    }
    
    binary_int operator++(int) { //посфиксный
        binary_int temp = *this;
        value = add(value, 1);
        return temp;
    }

    binary_int &operator--() {
        value = subtract(value, 1);
        return *this;
    }

    binary_int operator--(int) {
        binary_int temp = *this;
        value = subtract(value, 1);
        return temp;
    }

    binary_int &operator+=(const binary_int &other) {
        value = add(value, other.value);
        return *this;
    }

    binary_int operator+(const binary_int &other) const {
        return binary_int(add(value, other.value));
    }

    binary_int &operator-=(const binary_int &other) {
        value = subtract(value, other.value);
        return *this;
    }

    binary_int operator-(const binary_int &other) const {
        return binary_int(subtract(value, other.value));
    }

    binary_int &operator*=(const binary_int &other) {
        value = multiply(value, other.value);
        return *this;
    }

    binary_int operator*(const binary_int &other) const {
        return binary_int(multiply(value, other.value));
    }

    binary_int &operator<<=(int shift) {
        value <<= shift;
        return *this;
    }

    binary_int operator<<(int shift) const {
        return binary_int(value << shift);
    }

    binary_int &operator>>=(int shift) {
        value >>= shift;
        return *this;
    }

    binary_int operator>>(int shift) const {
        return binary_int(value >> shift);
    }
    
    std::pair<binary_int, binary_int> split_bits() const {
        int high_bits = value & 0xFFFF0000;
        int low_bits = value & 0x0000FFFF;
        return {binary_int(high_bits), binary_int(low_bits)};
    }
};

std::ostream &operator<<(std::ostream &os, const binary_int &bi) {
        os << std::bitset<32>(bi.get_value());
        return os;
    }

int main() {
    binary_int a(5);
    binary_int b(3);

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;

    binary_int m = -a; //a = 5; b = 3
    std::cout << "-a = " << m << std::endl;

    a += b; //a = 8; b = 3
    std::cout << "a += b -> a = " << a << std::endl;

    binary_int c = a + b; //a = 8; b = 3
    std::cout << "a + b = " << c << std::endl;

    a -= b; //a = 5; b = 3
    std::cout << "a -= b -> a = " << a << std::endl;

    binary_int d = a - b; //a = 5; b = 3
    std::cout << "a - b = " << d << std::endl;

    a *= b; //a = 15; b = 3
    std::cout << "a *= b -> a = " << a << std::endl;

    binary_int e = a * b; //a = 15; b = 3
    std::cout << "a * b = " << e << std::endl;

    ++a; //a = 16; b = 3
    std::cout << "++a -> a = " << a << std::endl;

    binary_int f = a++; //a = 17; b = 3
    std::cout << "f = a++ -> f = " << f << ", a = " << a << std::endl;

    --a; //a = 16; b = 3
    std::cout << "--a -> a = " << a << std::endl;

    binary_int g = a--; //a = 15; b = 3
    std::cout << "g = a-- -> g = " << g << ", a = " << a << std::endl;

    binary_int h = a << 2; //a = 15; b = 3
    std::cout << "a << 2 -> h = " << h << std::endl;

    binary_int i = a >> 2; //a = 15; b = 3
    std::cout << "a >> 2 -> i = " << i << std::endl;

    a <<= 2; //a = 111100
    std::cout << "a <<= b -> a = " << a << std::endl;

    a >>= 1; //a = 11110
    std::cout << "a >>= b -> a = " << a << std::endl;

    binary_int p(4294967295);
    std::pair<binary_int, binary_int> res = p.split_bits();
    std::cout << "High bits: " << res.first << ", Low bits: " << res.second << std::endl;

    return 0;
}