#include "CuckooHash.h"
#include <iostream>
#include <fstream>

static int hash1(int key, int capacity) {
    return key % capacity;
}

static int hash2(int key, int capacity) {
    return (key / capacity) % capacity;
}

// CuckooNode methods (без изменений)
CuckooNode::CuckooNode() : key(0), value(""), occupied(false) {}

int CuckooNode::getKey() const { return key; }

const std::string& CuckooNode::getValue() const { return value; }

bool CuckooNode::isOccupied() const { return occupied; }

void CuckooNode::setKey(int k) { key = k; }

void CuckooNode::setValue(const std::string& v) { value = v; }

void CuckooNode::markOccupied() { occupied = true; }

void CuckooNode::markEmpty() { occupied = false; }

// CuckooHashTable methods
CuckooHashTable::CuckooHashTable(int initialCapacity) {
    capacity = initialCapacity;
    size = 0;
    table1 = new CuckooNode[capacity];
    table2 = new CuckooNode[capacity];
}

CuckooHashTable::~CuckooHashTable() {
    delete[] table1;
    delete[] table2;
}

void CuckooHashTable::rehash() {
    int oldCapacity = capacity;
    CuckooNode* oldTable1 = table1;
    CuckooNode* oldTable2 = table2;

    capacity = capacity * 2 + 1;
    table1 = new CuckooNode[capacity];
    table2 = new CuckooNode[capacity];
    size = 0;

    for (int i = 0; i < oldCapacity; ++i) {
        if (oldTable1[i].isOccupied()) {
            insert(oldTable1[i].getKey(), oldTable1[i].getValue());
        }
        if (oldTable2[i].isOccupied()) {
            insert(oldTable2[i].getKey(), oldTable2[i].getValue());
        }
    }

    delete[] oldTable1;
    delete[] oldTable2;
}



bool CuckooHashTable::insert(int key, const std::string& value) {
    if (search(key) != "") {
        return false;
    }

    if (static_cast<float>(size + 1) / (capacity * 2) >= 0.5f) {
        rehash();
    }

    int currentKey = key;
    std::string currentValue = value;
    int currentTable = 1; // 1 = table1, 2 = table2

    const int MAX_ATTEMPTS = capacity * 100; // очень большой запас — вставка почти всегда удаётся

    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        int pos = (currentTable == 1) ? hash1(currentKey, capacity) : hash2(currentKey, capacity);
        CuckooNode* slot = (currentTable == 1) ? &table1[pos] : &table2[pos];

        if (!slot->isOccupied()) {
            slot->setKey(currentKey);
            slot->setValue(currentValue);
            slot->markOccupied();
            ++size;
            return true;
        }

        // Вытеснение
        int displacedKey = slot->getKey();
        std::string displacedValue = slot->getValue();
        slot->setKey(currentKey);
        slot->setValue(currentValue);

        currentKey = displacedKey;
        currentValue = displacedValue;
        currentTable = (currentTable == 1) ? 2 : 1;
    }

    // Если не удалось — рехэш и повтор
    // rehash();
    //
    // currentKey = key;
    // currentValue = value;
    // currentTable = 1;
    //
    // for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
    //     int pos = (currentTable == 1) ? hash1(currentKey, capacity) : hash2(currentKey, capacity);
    //     CuckooNode* slot = (currentTable == 1) ? &table1[pos] : &table2[pos];
    //
    //     if (!slot->isOccupied()) {
    //         slot->setKey(currentKey);
    //         slot->setValue(currentValue);
    //         slot->markOccupied();
    //         ++size;
    //         return true;
    //     }
    //
    //     int displacedKey = slot->getKey();
    //     std::string displacedValue = slot->getValue();
    //     slot->setKey(currentKey);
    //     slot->setValue(currentValue);
    //
    //     currentKey = displacedKey;
    //     currentValue = displacedValue;
    //     currentTable = (currentTable == 1) ? 2 : 1;
    // }

    return false; // крайне редкий случай
}





std::string CuckooHashTable::search(int key) const {
    int pos1 = hash1(key, capacity);
    if (table1[pos1].isOccupied() && table1[pos1].getKey() == key) {
        return table1[pos1].getValue();
    }

    int pos2 = hash2(key, capacity);
    if (table2[pos2].isOccupied() && table2[pos2].getKey() == key) {
        return table2[pos2].getValue();
    }

    return "";
}

bool CuckooHashTable::remove(int key) {
    int pos1 = hash1(key, capacity);
    if (table1[pos1].isOccupied() && table1[pos1].getKey() == key) {
        table1[pos1].markEmpty();
        --size;
        return true;
    }

    int pos2 = hash2(key, capacity);
    if (table2[pos2].isOccupied() && table2[pos2].getKey() == key) {
        table2[pos2].markEmpty();
        --size;
        return true;
    }

    return false;
}

void CuckooHashTable::print() const {
    std::cout << "Table 1:\n";
    for (int i = 0; i < capacity; ++i) {
        std::cout << "[" << i << "]: ";
        if (table1[i].isOccupied()) {
            std::cout << table1[i].getKey() << " -> " << table1[i].getValue();
        } else {
            std::cout << "EMPTY";
        }
        std::cout << "\n";
    }

    std::cout << "Table 2:\n";
    for (int i = 0; i < capacity; ++i) {
        std::cout << "[" << i << "]: ";
        if (table2[i].isOccupied()) {
            std::cout << table2[i].getKey() << " -> " << table2[i].getValue();
        } else {
            std::cout << "EMPTY";
        }
        std::cout << "\n";
    }
}

void CuckooHashTable::clear() {
    for (int i = 0; i < capacity; ++i) {
        table1[i].markEmpty();
        table2[i].markEmpty();
    }
    size = 0;
}

void CuckooHashTable::readFromFile(const std::string& filename) {
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

void CuckooHashTable::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    for (int i = 0; i < capacity; ++i) {
        if (table1[i].isOccupied()) {
            out << table1[i].getKey() << " " << table1[i].getValue() << "\n";
        }
        if (table2[i].isOccupied()) {
            out << table2[i].getKey() << " " << table2[i].getValue() << "\n";
        }
    }
    out.close();
}

bool CuckooHashTable::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));

    for (int i = 0; i < capacity; ++i) {
        if (table1[i].isOccupied()) {
            int key = table1[i].getKey();
            out.write(reinterpret_cast<const char*>(&key), sizeof(key));

            const std::string& value = table1[i].getValue();
            int len = static_cast<int>(value.size());
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            if (len > 0) out.write(value.data(), len);
        }
        if (table2[i].isOccupied()) {
            int key = table2[i].getKey();
            out.write(reinterpret_cast<const char*>(&key), sizeof(key));

            const std::string& value = table2[i].getValue();
            int len = static_cast<int>(value.size());
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            if (len > 0) out.write(value.data(), len);
        }
    }

    out.close();
    return true;
}

bool CuckooHashTable::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;

    clear();
    delete[] table1;
    delete[] table2;

    int newSize, newCapacity;
    in.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));
    in.read(reinterpret_cast<char*>(&newCapacity), sizeof(newCapacity));

    size = 0;
    capacity = newCapacity;
    table1 = new CuckooNode[capacity];
    table2 = new CuckooNode[capacity];

    for (int i = 0; i < newSize; ++i) {
        int key;
        in.read(reinterpret_cast<char*>(&key), sizeof(key));

        int len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        std::string value;
        if (len > 0) {
            value.resize(len);
            in.read(&value[0], len);
        }

        // Прямое размещение (как в оригинале при сохранении)
        int pos1 = hash1(key, capacity);
        if (!table1[pos1].isOccupied()) {
            table1[pos1].setKey(key);
            table1[pos1].setValue(value);
            table1[pos1].markOccupied();
        } else {
            int pos2 = hash2(key, capacity);
            table2[pos2].setKey(key);
            table2[pos2].setValue(value);
            table2[pos2].markOccupied();
        }
        ++size;
    }

    in.close();
    return true;
}
