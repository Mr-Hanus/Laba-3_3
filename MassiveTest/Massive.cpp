#include "Massive.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

const int Massive::MIN_CAPACITY = 10;

void Massive::ensureCapacity() {
    if (size >= capacity) {
        int newCap = (capacity > 0) ? capacity * 2 : MIN_CAPACITY;
        std::string* newData = new std::string[newCap];
        if (data && size > 0) {
            std::copy(data, data + size, newData);
        }
        delete[] data;
        data = newData;
        capacity = newCap;
    }
}

void Massive::shrinkCapacity() {
    if (capacity > MIN_CAPACITY && size < capacity / 4) {
        int newCap = std::max(capacity / 2, MIN_CAPACITY);
        std::string* newData = new std::string[newCap];
        if (data && size > 0) {
            std::copy(data, data + size, newData);
        }
        delete[] data;
        data = newData;
        capacity = newCap;
    }
}

Massive::Massive(int initialCapacity)
    : size(0), capacity(std::max(initialCapacity, MIN_CAPACITY)) {
    data = new std::string[capacity];
}

Massive::~Massive() {
    delete[] data;
}

void Massive::addEnd(const std::string& val) {
    ensureCapacity();
    data[size++] = val;
}

bool Massive::addAt(int index, const std::string& val) {
    if (index < 0 || index > size) return false;
    ensureCapacity();
    for (int i = size; i > index; i--) {
        data[i] = data[i - 1];
    }
    data[index] = val;
    size++;
    return true;
}

std::string Massive::get(int index) const {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

bool Massive::set(int index, const std::string& val) {
    if (index < 0 || index >= size) return false;
    data[index] = val;
    return true;
}

bool Massive::removeAt(int index) {
    if (index < 0 || index >= size) return false;
    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
    shrinkCapacity();
    return true;
}

int Massive::getSize() const { return size; }
int Massive::getCapacity() const { return capacity; }

// Файловые операции
void Massive::readFromFile(const std::string& filename) {
    clear();
    std::ifstream in(filename);
    if (!in.is_open()) return;
    
    std::string val;
    while (in >> val) {
        addEnd(val);
    }
    in.close();
}

void Massive::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    for (int i = 0; i < size; i++) {
        out << data[i];
        if (i < size - 1) out << " ";
    }
    out.close();
}

// Бинарная сериализация
bool Massive::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    
    // Записываем размер массива
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Записываем каждый элемент
    for (int i = 0; i < size; i++) {
        size_t len = data[i].size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(data[i].c_str(), len);
    }
    
    out.close();
    return true;
}

bool Massive::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    // Читаем размер массива
    int savedSize = 0;
    in.read(reinterpret_cast<char*>(&savedSize), sizeof(savedSize));
    
    // Читаем каждый элемент
    for (int i = 0; i < savedSize; i++) {
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string value(len, '\0');
        in.read(&value[0], len);
        
        addEnd(value);
    }
    
    in.close();
    return true;
}

void Massive::clear() {
    delete[] data;
    data = new std::string[MIN_CAPACITY];
    size = 0;
    capacity = MIN_CAPACITY;
}

void Massive::print() const {
    for (int i = 0; i < size; i++) {
        std::cout << data[i];
        if (i < size - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

bool Massive::checkIntegrity() const {
    return data != nullptr && size >= 0 && size <= capacity && capacity >= MIN_CAPACITY;
}
