#pragma once
#include <string>
#include <fstream>

class HashNode {
private:
    int key;
    std::string value;
    bool occupied;
    bool deleted;

public:
    HashNode();
    
    int getKey() const;
    const std::string& getValue() const;
    bool isOccupied() const;
    bool isDeleted() const;

    void setKey(int k);
    void setValue(const std::string& v);
    void markOccupied();
    void markDeleted();
    void markEmpty();
};

class HashTable {
private:
    HashNode* table;
    int size;
    int capacity;

    void rehash();

public:
    HashTable(int initialCapacity = 11);
    ~HashTable();

    bool insert(int key, const std::string& value);
    std::string search(int key) const;
    bool remove(int key);

    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    int getCapacity() const { return capacity; }
    float getLoadFactor() const { return static_cast<float>(size) / capacity; }

    void print() const;
    void clear();

    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);

    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);

    // Для тестов
    const HashNode* getTable() const { return table; }
};
