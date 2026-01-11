#pragma once
#include <string>
#include <fstream>

class Node {
private:
    std::string value;
    Node* next;

public:
    Node(const std::string& val = "", Node* nxt = nullptr);
    
    std::string getValue() const;
    Node* getNext() const;
    
    void setValue(const std::string& val);
    void setNext(Node* nxt);
    
    friend class ForwardList;
};

class ForwardList {
private:
    Node* head;

public:
    ForwardList();
    ~ForwardList();
    
    void addHead(const std::string& val);
    void addTail(const std::string& val);
    bool addBefore(const std::string& target, const std::string& val);
    bool addAfter(const std::string& target, const std::string& val);
    
    bool removeHead();
    bool removeTail();
    bool removeValue(const std::string& val);
    bool removeBefore(const std::string& target);
    bool removeAfter(const std::string& target);
    
    Node* getNode(int index) const;
    Node* getHead() const;
    Node* findValue(const std::string& val) const;
    
    void printList() const;
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    //Bin serialization & deserialization
    bool serializeToBinary(const std::string& filename) const;
    bool deserializeFromBinary(const std::string& filename);

    void clear();
};
