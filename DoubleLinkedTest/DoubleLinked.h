#pragma once
#include <string>
#include <fstream>

class DNode {
private:
    std::string value;
    DNode* prev;
    DNode* next;

public:
    DNode(const std::string& val = "", DNode* p = nullptr, DNode* n = nullptr);
    
    std::string getValue() const;
    DNode* getPrev() const;
    DNode* getNext() const;
    
    void setValue(const std::string& val);
    void setPrev(DNode* p);
    void setNext(DNode* n);
    
    friend class DoubleLinkedList;
};

class DoubleLinkedList {
private:
    DNode* head;
    DNode* tail;

public:
    DoubleLinkedList();
    ~DoubleLinkedList();
    
    void addHead(const std::string& val);
    void addTail(const std::string& val);
    bool addBefore(const std::string& target, const std::string& val);
    bool addAfter(const std::string& target, const std::string& val);
    
    bool removeHead();
    bool removeTail();
    bool removeValue(const std::string& val);
    bool removeBefore(const std::string& target);
    bool removeAfter(const std::string& target);
    
    DNode* getNode(int index) const;
    DNode* getHead() const;
    DNode* getTail() const;
    DNode* findValue(const std::string& val) const;
    
    void printList() const;
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);
    
    void clear();
    
    // Проверка целостности списка
    bool checkIntegrity() const;
};
