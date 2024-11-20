#include <iostream>
#include <cmath>

//Что такое std::cout? Что такое std и cout?

class Complex { //Что такое класс и зачем он нужен?
private:
    double real; //Зачем при обьявлении класса закрытые переменные и есть ли открытые переменные? И зачем они нам? 
    double imag;    

public: //А открытые методы для чего? И есть ли закрытые?
    Complex(double r = 0, double i = 0) : real(r), imag(i) {} //что такое конструктор, зачем он нужен, что там за аргументы???

    // Метод сложения
    Complex add(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    // Метод вычитания
    Complex subtract(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }

    // Метод умножения
    Complex multiply(const Complex& other) const {
        double r = real * other.real - imag * other.imag;
        double i = real * other.imag + imag * other.real;
        return Complex(r, i);
    }

    // Метод деления
    Complex divide(const Complex& other) const {
        double denominator = other.real * other.real + other.imag * other.imag;
        if (denominator == 0) {
            throw std::runtime_error("Division by zero");
        }
        double r = (real * other.real + imag * other.imag) / denominator;
        double i = (imag * other.real - real * other.imag) / denominator;
        return Complex(r, i);
    }

    // Метод для получения модуля комплексного числа
    double modulus() const {
        return std::sqrt(real * real + imag * imag);
    }

    // Метод для получения аргумента комплексного числа
    double argument() const {
        return std::atan2(imag, real);
    }

    // Метод для отображения комплексного числа
    void display() const {
        std::cout << real;
        if (imag >= 0) {
            std::cout << " + " << imag << "i";
        } else {
            std::cout << " - " << -imag << "i";
        }
        std::cout << std::endl;
    }
};

int main() {
    // Создание комплексных чисел
    Complex c1(3, 4);     // 3 + 4i
    Complex c2(1, -2);    // 1 - 2i

    // Демонстрация работы методов
    std::cout << "Первое комплексное число: ";
    c1.display();

    std::cout << "Второе комплексное число: ";
    c2.display();

    // Сложение
    Complex sum = c1.add(c2);
    std::cout << "Сложение: ";
    sum.display();

    // Вычитание
    Complex diff = c1.subtract(c2);
    std::cout << "Вычитание: ";
    diff.display();

    // Умножение
    Complex product = c1.multiply(c2);
    std::cout << "Умножение: ";
    product.display();

    // Деление
    try {
        Complex quotient = c1.divide(c2);
        std::cout << "Деление: ";
        quotient.display();
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    // Модуль
    std::cout << "Модуль первого числа: " << c1.modulus() << std::endl;

    // Аргумент
    std::cout << "Аргумент первого числа: " << c1.argument() << std::endl;

    return 0;
}