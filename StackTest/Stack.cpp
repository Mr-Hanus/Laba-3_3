#include "Stack.h"
#include <iostream>
#include <fstream>
#include <algorithm>

const int Stack::MIN_CAPACITY = 4;

void Stack::growIfNeeded() {
    if (size < capacity) return;
    
    int newCap = capacity * 2;
    std::string* newData = new std::string[newCap];
    
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }
    
    delete[] data;
    data = newData;
    capacity = newCap;
}

void Stack::shrinkIfNeeded() {
    if (capacity <= MIN_CAPACITY) return;
    if (size * 4 >= capacity) return;
    
    int newCap = capacity / 2;
    if (newCap < MIN_CAPACITY) newCap = MIN_CAPACITY;
    
    std::string* newData = new std::string[newCap];
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }
    
    delete[] data;
    data = newData;
    capacity = newCap;
}

Stack::Stack(int initialCapacity) 
    : size(0), capacity(std::max(initialCapacity, MIN_CAPACITY)) {
    data = new std::string[capacity];
}

Stack::~Stack() {
    delete[] data;
}

// Основные операции
void Stack::push(const std::string& val) {
    growIfNeeded();
    data[size++] = val;
}

std::string Stack::pop() {
    if (size == 0) {
        throw std::runtime_error("Stack underflow");
    }
    
    std::string result = data[--size];
    shrinkIfNeeded();
    return result;
}

std::string Stack::top() const {
    if (size == 0) {
        throw std::runtime_error("Stack is empty");
    }
    return data[size - 1];
}

int Stack::getSize() const { return size; }
int Stack::getCapacity() const { return capacity; }
bool Stack::isEmpty() const { return size == 0; }

void Stack::readFromFile(const std::string& filename) {
    clear();
    
    std::ifstream in(filename);
    if (!in.is_open()) return;
    
    std::string val;
    while (in >> val) {
        push(val);
    }
    
    in.close();
}

void Stack::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    
    for (int i = 0; i < size; ++i) {
        out << data[i];
        if (i + 1 < size) out << " ";
    }
    
    out.close();
}

bool Stack::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    
    // Записываем размер стека
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Записываем элементы (снизу вверх)
    for (int i = 0; i < size; ++i) {
        size_t len = data[i].size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(data[i].c_str(), len);
    }
    
    out.close();
    return true;
}

bool Stack::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    // Читаем размер стека
    int savedSize = 0;
    in.read(reinterpret_cast<char*>(&savedSize), sizeof(savedSize));
    
    // Читаем элементы
    for (int i = 0; i < savedSize; ++i) {
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string value(len, '\0');
        in.read(&value[0], len);
        
        push(value);
    }
    
    in.close();
    return true;
}

void Stack::clear() {
    delete[] data;
    data = new std::string[MIN_CAPACITY];
    size = 0;
    capacity = MIN_CAPACITY;
}

void Stack::print() const {
    if (size == 0) {
        std::cout << "(empty stack)" << std::endl;
        return;
    }
    
    std::cout << "Bottom -> ";
    for (int i = 0; i < size; ++i) {
        std::cout << data[i];
        if (i + 1 < size) std::cout << " ";
    }
    std::cout << " <- Top" << std::endl;
}

bool Stack::checkIntegrity() const {
    if (data == nullptr) return false;
    if (size < 0) return false;
    if (size > capacity) return false;
    if (capacity < MIN_CAPACITY) return false;
    return true;
}
