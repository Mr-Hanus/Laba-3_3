#pragma once
#include <string>
#include <fstream>
#include <stdexcept>

class Stack {
private:
    std::string* data;
    int size;
    int capacity;
    static const int MIN_CAPACITY;
    
    void growIfNeeded();
    void shrinkIfNeeded();

public:
    Stack(int initialCapacity = 10);
    ~Stack();
    
    void push(const std::string& val);
    std::string pop();
    std::string top() const;
    
    int getSize() const;
    int getCapacity() const;
    bool isEmpty() const;
    
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);
    
    void clear();
    void print() const;
    
    bool checkIntegrity() const;
};
