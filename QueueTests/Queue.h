#pragma once
#include <string>
#include <fstream>
#include <stdexcept>

class Queue {
private:
    std::string* data;
    int size;
    int capacity;
    int front;
    int back;
    static const int MIN_CAPACITY ;
    
    void growIfNeeded();
    void shrinkIfNeeded();

public:
    Queue(int initialCapacity = 10);
    ~Queue();
    
    // Основные операции
    void enqueue(const std::string& val);
    std::string dequeue();
    std::string peek() const;
    
    // Информация
    int getSize() const;
    int getCapacity() const;
    bool isEmpty() const;
    
    // Файловые операции
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    // Бинарная сериализация
    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);
    
    // Вспомогательные
    void clear();
    void print() const;
    
    // Для тестов
    bool checkIntegrity() const;
};
