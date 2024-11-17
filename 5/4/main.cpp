#include <iostream>
#include <cmath>
/*
private переменные, недоступные напрямую снаружи класса, можно использовать внутри методов класса (инкапсуляция)
std::cout - использовать cout из пространства имен std
<< - отправить в ...
std::end1 - нужен для завершения строки и сбрасывает буфер вывода, чтоб была гарантия, что все вывелось
*/

//про вывод в си++ и про ссылки и про осановку программы
class Complex {
private:
    double real;
    double imag;    

public: //тута методы
    Complex(double r = 0, double i = 0) : real(r), imag(i) {} //конструктор::начальная инициализация

    //складываем
    Complex add(const Complex &other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    //вычитаем
    Complex subtract(const Complex &other) const {
        return Complex(real - other.real, imag - other.imag);
    }

    Complex multiply(const Complex &other) const {
        return Complex(real * other.real + imag * other.imag, real * other.imag + imag * other.real);
    }    
    
    //делим
    Complex share(const Complex &other) const {
        double denominator = other.real * other.real + other.imag * other.imag;
        if (denominator == 0) {
            throw std::string{"Деление на 0"}; //throw оператор для выброса исключения 
        }                                                 
        double r = (real * other.real + imag * other.imag) / denominator;
        double i = (imag * other.real - real * other.imag) / denominator;
        return Complex(r, i);
    }

    double modul() const {
        return std::sqrt(real * real + imag * imag); //зачем std::?
    }

    double argument() const {
        return std::atan2(imag, real);
    }

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
    Complex c1(3, 4);     // 3 + 4i
    Complex c2(0, 0);    // 1 - 2i

    std::cout << "Первое комплексное число: ";
    c1.display();

    std::cout << "Второе комплексное число: ";
    c2.display();

    Complex sum = c1.add(c2);
    std::cout << "Сложение: ";
    sum.display();

    Complex diff = c1.subtract(c2);
    std::cout << "Вычитание: ";
    diff.display();

    Complex product = c1.multiply(c2);
    std::cout << "Умножение: ";
    product.display();

    try {
        Complex quotient = c1.share(c2);
        std::cout << "Деление: ";
        quotient.display();
    } catch (const std::string &err_mes) {
        std::cout << err_mes << std::endl;
    }

    std::cout << "Модуль первого числа: " << c1.modul() << std::endl;

    std::cout << "Аргумент первого числа: " << c1.argument() << std::endl;

    return 0;
}