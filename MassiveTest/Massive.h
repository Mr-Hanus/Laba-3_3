#pragma once
#include <string>
#include <fstream>

class Massive {
private:
    std::string* data;
    int size;
    int capacity;
    static const int MIN_CAPACITY;
    
    void ensureCapacity();
    void shrinkCapacity();

public:
    Massive(int initialCapacity = 10);
    ~Massive();
    
    void addEnd(const std::string& val);
    bool addAt(int index, const std::string& val);
    
    std::string get(int index) const;
    bool set(int index, const std::string& val);
    
    bool removeAt(int index);
    
    int getSize() const;
    int getCapacity() const;
    
    // Файловые операции
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    // Бинарная сериализация
    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);
    
    void clear();
    void print() const;
    
    bool checkIntegrity() const;
};
