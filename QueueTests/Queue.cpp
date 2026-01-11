#include "Queue.h"
#include <iostream>
#include <fstream>
#include <algorithm>

const int Queue::MIN_CAPACITY = 4;

void Queue::growIfNeeded() {
    if (size < capacity) return;
    
    int newCap = capacity * 2;
    std::string* newData = new std::string[newCap];
    
    for (int i = 0; i < size; ++i) {
        newData[i] = data[(front + i) % capacity];
    }
    
    delete[] data;
    data = newData;
    capacity = newCap;
    front = 0;
    back = size % capacity;
}

void Queue::shrinkIfNeeded() {
    if (capacity <= MIN_CAPACITY) return;
    if (size * 4 >= capacity) return;
    
    int newCap = capacity / 2;
    if (newCap < MIN_CAPACITY) newCap = MIN_CAPACITY;
    
    std::string* newData = new std::string[newCap];
    
    // Перекопируем элементы с учётом front
    for (int i = 0; i < size; ++i) {
        newData[i] = data[(front + i) % capacity];
    }
    
    delete[] data;
    data = newData;
    capacity = newCap;
    front = 0;
    back = size % capacity;
}

// Конструктор/деструктор
Queue::Queue(int initialCapacity) 
    : size(0), capacity(std::max(initialCapacity, MIN_CAPACITY)),
      front(0), back(0) {
    data = new std::string[capacity];
}

Queue::~Queue() {
    delete[] data;
}

// Основные операции
void Queue::enqueue(const std::string& val) {
    growIfNeeded();
    data[back] = val;
    back = (back + 1) % capacity;
    size++;
}

std::string Queue::dequeue() {
    if (size == 0) {
        throw std::runtime_error("Queue underflow");
    }
    
    std::string result = data[front];
    front = (front + 1) % capacity;
    size--;
    shrinkIfNeeded();
    return result;
}

std::string Queue::peek() const {
    if (size == 0) {
        throw std::runtime_error("Queue is empty");
    }
    return data[front];
}

// Информация
int Queue::getSize() const { return size; }
int Queue::getCapacity() const { return capacity; }
bool Queue::isEmpty() const { return size == 0; }

// Файловые операции
void Queue::readFromFile(const std::string& filename) {
    clear();
    
    std::ifstream in(filename);
    if (!in.is_open()) return;
    
    std::string val;
    while (in >> val) {
        enqueue(val);
    }
    
    in.close();
}

void Queue::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    
    for (int i = 0; i < size; ++i) {
        out << data[(front + i) % capacity];
        if (i + 1 < size) out << " ";
    }
    
    out.close();
}

// Бинарная сериализация
bool Queue::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    
    // Записываем размер очереди
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Записываем элементы (от front до back)
    for (int i = 0; i < size; ++i) {
        size_t len = data[(front + i) % capacity].size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(data[(front + i) % capacity].c_str(), len);
    }
    
    out.close();
    return true;
}

bool Queue::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    // Читаем размер очереди
    int savedSize = 0;
    in.read(reinterpret_cast<char*>(&savedSize), sizeof(savedSize));
    
    // Читаем элементы
    for (int i = 0; i < savedSize; ++i) {
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string value(len, '\0');
        in.read(&value[0], len);
        
        enqueue(value);
    }
    
    in.close();
    return true;
}

// Вспомогательные методы
void Queue::clear() {
    delete[] data;
    data = new std::string[MIN_CAPACITY];
    size = 0;
    capacity = MIN_CAPACITY;
    front = 0;
    back = 0;
}

void Queue::print() const {
    if (size == 0) {
        std::cout << "(empty queue)" << std::endl;
        return;
    }
    
    std::cout << "Front -> ";
    for (int i = 0; i < size; ++i) {
        std::cout << data[(front + i) % capacity];
        if (i + 1 < size) std::cout << " ";
    }
    std::cout << " <- Back" << std::endl;
}

bool Queue::checkIntegrity() const {
    if (data == nullptr) return false;
    if (size < 0) return false;
    if (size > capacity) return false;
    if (capacity < MIN_CAPACITY) return false;
    if (front < 0 || front >= capacity) return false;
    if (back < 0 || back >= capacity) return false;
    return true;
}
