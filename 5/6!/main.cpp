#include <iostream>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <compare>
#include <cmath>

class vector { 
private: 
    double *data;
    size_t size; 
    size_t capacity;

    void reallocate(size_t num) {
        double *new_data = new double[num];
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity = num;
    }

public:
    class Iterator{
        double* ptr;

    public:
        Iterator(double* p) : ptr(p) {}

        double& operator*() { 
            return *ptr; 
        }

        Iterator& operator++() {
            ++ptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator operator+(size_t i) const {
            return Iterator(ptr + i);
        }

        Iterator operator-(size_t i) const {
            return Iterator(ptr - i);
        }

        Iterator& operator--() {
            --ptr;
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        Iterator& operator+=(size_t i) {
            *this = *this + i;
            return *this;
        }

        Iterator& operator-=(size_t i) {
            *this = *this - i;
            return *this;
        }

        int operator==(const Iterator& other) const {
            if (ptr == other.ptr) {
                return 1;
            }
            return 0;
        }
        int operator!=(const Iterator& other) const {
            if (ptr != other.ptr) {
                return 1;
            }
            return 0;
        }

        double& operator[](size_t i) const {
            return *(ptr + i);
        }

        double* operator->() {
            return ptr;
        }

        size_t operator-(Iterator &other) {
            return size_t(ptr - other.ptr);
        }

        bool operator<(Iterator &other) {
            return bool(ptr < other.ptr);
        }

    };

    Iterator begin() {
        return Iterator(data);
    }

    Iterator end() { 
        return Iterator(data + size);
    }

    vector(size_t n_size, double n_val) : size(n_size), capacity(n_size), data(new double[n_size]) {
        for (int i = 0; i < n_size; ++i) {
            data[i] = n_val;
        }
    } 

    vector(size_t n_size) : size(n_size), capacity(n_size), data(new double[n_size]) {
        for (int i = 0; i < n_size; ++i) {
            data[i] = 0.0;
        }
    } 

    vector(vector::Iterator beg, vector::Iterator end) {
        if (end < beg) {
            size = beg - end;
            capacity = size;
            data = new double[size];

            for (size_t i = 0; i < size; ++i) {
                data[i] = *(beg - i);
            }
        } else {
            size =  end - beg;
            capacity = size;
            data = new double[size];

            for (size_t i = 0; i < size; ++i) {
                data[i] = *(beg + i);
            }
        }
    }

    vector(std::initializer_list<double> list) : size(list.size()), capacity(list.size()), data(new double[list.size()]) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = *(list.begin() + i);
        }
    }

    ~vector() {
        delete[] data;
    }

    void display() const {
        for (size_t i = 0; i < size; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    double& at(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    } 

    bool empty() const { 
        return size == 0;
    }

    double& front() const {
        if (empty()) {
            throw std::out_of_range("Index out of range");
        }
        return data[0];
    } 

    double& back() const {
        if (empty()) {
            throw std::out_of_range("Index out of range");
        }
        return data[size - 1];
    }

    double* get_data() const { 
        return data;
    }

    size_t get_size() const {
        return size;
    }

    void reserve(size_t num) {
        if (capacity < num) {
            reallocate(num);
        }
    }

    size_t get_capacity() const {
        return capacity;
    }

    void shrink_to_fit() {
        if (capacity > size) {
            reallocate(size);
        }
    }

    void clear() {
        delete[] data;
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    void insert(size_t index, double elem) {
        if (index > size) {
            throw std::out_of_range("Index out of range");
        }
        if (size == capacity) {
            reserve(capacity == 0 ? 1 : capacity * 2);
        }
        for (size_t i = size; i > index; --i) {
            data[i] = data[i - 1];
        }
        data[index] = elem;
        ++size;
    }

    void erase(size_t index) {
        if (index > size) {
            throw std::out_of_range("Index out of range");
        }

        for (size_t i = index; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        --size;
    }

    void push_back(double elem) {
        if (capacity == size) {
            reserve(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = elem;
    }

    void pop_back() {
        if (empty()) {
            throw std::out_of_range("Index out of range");
        }
        --size;
        shrink_to_fit();
    }

    void resize(size_t n_size, double elem) {
        if (n_size > capacity) {
            reserve(n_size);
        }
        if (n_size > size) {
            for (size_t i = size; i < capacity; ++i) {
                data[i] = elem;
            }
        }
        size = n_size;
        shrink_to_fit();
    } 

    int operator==(const vector &other) const {
        if (size != other.size) {
            return 0;
        }
        for (size_t i = 0; i < size; ++i) {
            if (data[i] != other.data[i]) {
                return 0;
            }
        }
        return 1;
    }

    int operator<=>(const vector& other) const {
        for (size_t i = 0; i < std::min(size, other.size); ++i) {
            if (data[i] != other.data[i]) {
                return data[i] < other.data[i] ? -1 : 1;
            }
        }

        if (size < other.size) {
            return -1;
        }
        if (size > other.size) {
            return 1;
        }
        return 0;
    }

};


int main() {
    try {
        vector v1(4, 5.0);
        vector v2(3);
        double array[] = {5.5, 6.6, 7.7, 8.8};
        vector v3(&array[0], &array[4]);
        vector v4 = {1.1, 2.2, 3.3, 4.4}; 

        std::cout << "Вектор из 4 элементов, инициализированных 5.0: ";
        v1.display();

        std::cout << "Вектор из 3 элементов, инициализированных 0.0: ";
        v2.display();

        std::cout << "Вектор из диапазона итераторов: ";
        v3.display();

        std::cout << "Вектор из списка инициализации: ";
        v4.display();

        std::cout << "Демонстрация v1.at(1): ";
        std::cout << v1.at(1) << std::endl;

        //std::cout << "Демонстрация v1.at(10): ";
        //std::cout << v1.at(10) << std::endl;

        vector v0(0);
        std::cout << "Демонстрация v0.empty(): ";
        std::cout << v0.empty() << std::endl;

        std::cout << "Демонстрация v3.front(): ";
        std::cout << v3.front() << std::endl;

        std::cout << "Демонстрация v3.back(): ";
        std::cout << v3.back() << std::endl;

        std::cout << "Демонстрация v3.get_data(): ";
        std::cout << v3.get_data() << std::endl;

        std::cout << "Демонстрация v3.get_size(): ";
        std::cout << v3.get_size() << std::endl;

        std::cout << "Демонстрация v3.get_capacity(): ";
        std::cout << v3.get_capacity() << std::endl;

        std::cout << "Демонстрация v3.reserve(8): ";
        v3.reserve(8);
        std::cout << std::endl;

        std::cout << "Демонстрация v3.get_capacity(): ";
        std::cout << v3.get_capacity() << std::endl;

        std::cout << "Демонстрация v3.shrink_to_fit(): ";
        v3.shrink_to_fit();
        std::cout << std::endl;

        std::cout << "Демонстрация v3.get_capacity(): ";
        std::cout << v3.get_capacity() << std::endl;

        std::cout << "Демонстрация v3.clear(): ";
        v3.clear();
        std::cout << std::endl;

        std::cout << "Демонстрация v3.get_capacity(): ";
        std::cout << v3.get_capacity() << std::endl;

        std::cout << "Демонстрация v4.insert(2, 9.9): \n";
        v4.display();
        v4.insert(2, 9.9);
        v4.display();

        std::cout << "Демонстрация v4.erase(2): \n";
        v4.display();
        v4.erase(2);
        v4.display();

        std::cout << "Демонстрация v4.push_back(): \n";
        v4.display();
        v4.push_back(8.8);
        v4.display();

        std::cout << "Демонстрация v4.pop_back(): \n";
        v4.display();
        v4.pop_back();
        v4.display();

        std::cout << "Демонстрация v4.resize(8, 8.8): \n";
        v4.display();
        v4.resize(8, 8.8);
        v4.display();

        std::cout << "Демонстрация v4.resize(4, 8.8): \n";
        v4.display();
        v4.resize(4, 8.8);
        v4.display();

        std::cout << "Демонстрация v4 == v3: ";
        std::cout << (v4 == v3) << std::endl;

        std::cout << "Демонстрация v4 <=> v1: ";
        std::cout << (v4 <=> v1) << std::endl;
        v4.display();
        v1.display();

        vector v5 = {1.1, 2.2, 3.3, 4.4}; 

        std::cout << "Демонстрация v4 <=> v5: ";
        std::cout << (v4 <=> v5) << std::endl;
        v4.display();
        v5.display();

        vector vec(5, 1.0);
        std::cout << "Демонстрация vec.begin() to vec.end(): ";

        for (vector::Iterator i = vec.begin(); i != vec.end(); ++i) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;


        std::cout << "Демонстрация *i += 1.0;: ";
        for (vector::Iterator i = vec.begin(); i != vec.end(); ++i) {
            *i += 1.0; 
        }
        vec.display();

      

    } catch (const std::out_of_range &err) { 
        std::cout << "Ошибка: " << err.what() << std::endl;
    } catch (const std::bad_array_new_length &err) { 
        std::cout << "Ошибка: " << err.what() << std::endl;
    } 

    return 0;
}