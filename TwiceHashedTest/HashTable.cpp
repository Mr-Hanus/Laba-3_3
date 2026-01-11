#include "HashTable.h"
#include <iostream>
#include <fstream>
#include <functional>

static int hash1(int key, int capacity) {
    return key % capacity;
}

static int hash2(int key, int capacity) {
    return 1 + (key % (capacity - 1));
}


HashNode::HashNode() : key(0), value(""), occupied(false), deleted(false) {}

int HashNode::getKey() const { return key; }

const std::string& HashNode::getValue() const { return value; }

bool HashNode::isOccupied() const { return occupied; }

bool HashNode::isDeleted() const { return deleted; }

void HashNode::setKey(int k) { key = k; }

void HashNode::setValue(const std::string& v) { value = v; }

void HashNode::markOccupied() {
    occupied = true;
    deleted = false;
}

void HashNode::markDeleted() { deleted = true; }

void HashNode::markEmpty() {
    occupied = false;
    deleted = false;
}


HashTable::HashTable(int initialCapacity) {
    capacity = initialCapacity;
    size = 0;
    table = new HashNode[capacity];
}

HashTable::~HashTable() {
    delete[] table;
}

void HashTable::rehash() {
    int oldCapacity = capacity;
    HashNode* oldTable = table;

    capacity = capacity * 2 + 1;
    table = new HashNode[capacity];
    size = 0;

    for (int i = 0; i < oldCapacity; ++i) {
        if (oldTable[i].isOccupied() && !oldTable[i].isDeleted()) {
            insert(oldTable[i].getKey(), oldTable[i].getValue());
        }
    }

    delete[] oldTable;
}

bool HashTable::insert(int key, const std::string& value) {
    if (getLoadFactor() >= 0.7f) {
        rehash();
    }

    int h1 = hash1(key, capacity);
    int h2 = hash2(key, capacity);
    int i = h1;

    for (int attempt = 0; attempt < capacity; ++attempt) {
        if (!table[i].isOccupied() || table[i].isDeleted()) {
            table[i].setKey(key);
            table[i].setValue(value);
            table[i].markOccupied();
            ++size;
            return true;
        }
        if (table[i].isOccupied() && !table[i].isDeleted() && table[i].getKey() == key) {
            table[i].setValue(value);
            return true;
        }
        i = (i + h2) % capacity;
    }
    return false;
}

std::string HashTable::search(int key) const {
    int h1 = hash1(key, capacity);
    int h2 = hash2(key, capacity);
    int i = h1;

    for (int attempt = 0; attempt < capacity; ++attempt) {
        if (!table[i].isOccupied()){
            if (!table[i].isDeleted()) {
            return "";
            } else {
            i = (i + h2)% capacity;
            continue;
            }
        }
        if (table[i].isOccupied() && !table[i].isDeleted() && table[i].getKey() == key) {
            return table[i].getValue();
        }
        i = (i + h2) % capacity;
    }
    return "";
}

bool HashTable::remove(int key) {
    int h1 = hash1(key, capacity);
    int h2 = hash2(key, capacity);
    int i = h1;

    for (int attempt = 0; attempt < capacity; ++attempt) {
        if (!table[i].isOccupied() ){
            if (!table[i].isDeleted()) {
                return false;
            } else {
            i = (i + h2 ) % capacity;
            }
        }
        if (table[i].isOccupied() && !table[i].isDeleted() && table[i].getKey() == key) {
            table[i].markDeleted();
            --size;
            return true;
        }
        i = (i + h2) % capacity;
    }
    return false;
}

void HashTable::print() const {
    for (int i = 0; i < capacity; ++i) {
        std::cout << "[" << i << "]: ";
        if (table[i].isOccupied() && !table[i].isDeleted()) {
            std::cout << table[i].getKey() << " -> " << table[i].getValue();
        } else if (table[i].isDeleted()) {
            std::cout << "DELETED";
        } else {
            std::cout << "EMPTY";
        }
        std::cout << std::endl;
    }
}

void HashTable::clear() {
    for (int i = 0; i < capacity; ++i) {
        table[i].markEmpty();
    }
    size = 0;
}

void HashTable::readFromFile(const std::string& filename) {
    clear();
    std::ifstream in(filename);
    if (!in.is_open()) return;

    int key;
    std::string value;
    while (in >> key) {
        std::getline(in, value);
        if (!value.empty() && value[0] == ' ') value.erase(0, 1);
        insert(key, value);
    }
    in.close();
}

void HashTable::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    for (int i = 0; i < capacity; ++i) {
        if (table[i].isOccupied() && !table[i].isDeleted()) {
            out << table[i].getKey() << " " << table[i].getValue() << std::endl;
        }
    }
    out.close();
}

bool HashTable::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));

    for (int i = 0; i < capacity; ++i) {
        if (table[i].isOccupied() && !table[i].isDeleted()) {
            int key = table[i].getKey();
            out.write(reinterpret_cast<const char*>(&key), sizeof(key));

            const std::string& value = table[i].getValue();
            int len = static_cast<int>(value.size());
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));

            if (len > 0) {
                out.write(value.data(), len);
            }
        }
    }

    out.close();
    return true;
}

bool HashTable::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;

    // Очищаем старую таблицу
    delete[] table;

    int savedSize, savedCapacity;
    in.read(reinterpret_cast<char*>(&savedSize), sizeof(savedSize));
    in.read(reinterpret_cast<char*>(&savedCapacity), sizeof(savedCapacity));

    size = savedSize;
    capacity = savedCapacity;
    table = new HashNode[capacity];

    // Заполняем нулями (на всякий случай)
    for (int i = 0; i < capacity; ++i) {
        table[i].markEmpty();
    }

    for (int i = 0; i < size; ++i) {
        int key;
        in.read(reinterpret_cast<char*>(&key), sizeof(key));

        int len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        std::string value;
        if (len > 0) {
            value.resize(len);
            in.read(&value[0], len);
        }

        // Находим свободный слот вручную (как в insert, но без рехэша)
        int h1 = hash1(key, capacity);
        int h2 = hash2(key, capacity);
        int j = h1;

        for (int attempt = 0; attempt < capacity; ++attempt) {
            if (!table[j].isOccupied() || table[j].isDeleted()) {
                table[j].setKey(key);
                table[j].setValue(value);
                table[j].markOccupied();
                break;
            }
            if (table[j].isOccupied() && !table[j].isDeleted() && table[j].getKey() == key) {
                table[j].setValue(value);
                break;
            }
            j = (j + h2) % capacity;
        }
    }

    in.close();
    return true;
}
