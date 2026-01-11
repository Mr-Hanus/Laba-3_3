#pragma once
#include <string>
#include <fstream>

class CuckooNode {
private:
    int key;
    std::string value;
    bool occupied;

public:
    CuckooNode();
    
    int getKey() const;
    const std::string& getValue() const;
    bool isOccupied() const;

    void setKey(int k);
    void setValue(const std::string& v);
    void markOccupied();
    void markEmpty();
};

class CuckooHashTable {
private:
    CuckooNode* table1;
    CuckooNode* table2;
    int size;
    int capacity;

    void rehash();
    bool insertHelper(int key, const std::string& value, int tableNum, int depth);

public:
    CuckooHashTable(int initialCapacity = 11);
    ~CuckooHashTable();

    bool insert(int key, const std::string& value);
    std::string search(int key) const;
    bool remove(int key);

    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    int getCapacity() const { return capacity; }

    void print() const;
    void clear();

    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);

    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);

    // Для тестов
    const CuckooNode* getTable1() const { return table1; }
    const CuckooNode* getTable2() const { return table2; }
};
