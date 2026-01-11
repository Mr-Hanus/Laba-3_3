#include "DoubleLinked.h"
#include <iostream>
#include <fstream>

DNode::DNode(const std::string& val, DNode* p, DNode* n) 
    : value(val), prev(p), next(n) {}

std::string DNode::getValue() const { return value; }
DNode* DNode::getPrev() const { return prev; }
DNode* DNode::getNext() const { return next; }

void DNode::setValue(const std::string& val) { value = val; }
void DNode::setPrev(DNode* p) { prev = p; }
void DNode::setNext(DNode* n) { next = n; }

DoubleLinkedList::DoubleLinkedList() : head(nullptr), tail(nullptr) {}

DoubleLinkedList::~DoubleLinkedList() { clear(); }

void DoubleLinkedList::addHead(const std::string& val) {
    DNode* node = new DNode(val, nullptr, head);
    if (head) head->setPrev(node);
    head = node;
    if (!tail) tail = node;
}

void DoubleLinkedList::addTail(const std::string& val) {
    DNode* node = new DNode(val, tail, nullptr);
    if (tail) tail->setNext(node);
    tail = node;
    if (!head) head = node;
}

bool DoubleLinkedList::addBefore(const std::string& target, const std::string& val) {
    DNode* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur) return false;
    
    DNode* node = new DNode(val, cur->getPrev(), cur);
    if (cur->getPrev()) cur->getPrev()->setNext(node);
    else head = node;
    cur->setPrev(node);
    return true;
}

bool DoubleLinkedList::addAfter(const std::string& target, const std::string& val) {
    DNode* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur) return false;
    
    DNode* node = new DNode(val, cur, cur->getNext());
    if (cur->getNext()) cur->getNext()->setPrev(node);
    else tail = node;
    cur->setNext(node);
    return true;
}

bool DoubleLinkedList::removeHead() {
    if (!head) return false;
    
    DNode* temp = head;
    head = head->getNext();
    if (head) head->setPrev(nullptr);
    else tail = nullptr;
    
    delete temp;
    return true;
}

bool DoubleLinkedList::removeTail() {
    if (!tail) return false;
    
    DNode* temp = tail;
    tail = tail->getPrev();
    if (tail) tail->setNext(nullptr);
    else head = nullptr;
    
    delete temp;
    return true;
}

bool DoubleLinkedList::removeValue(const std::string& val) {
    DNode* cur = head;
    while (cur && cur->getValue() != val) cur = cur->getNext();
    if (!cur) return false;
    
    if (cur->getPrev()) cur->getPrev()->setNext(cur->getNext());
    else head = cur->getNext();
    
    if (cur->getNext()) cur->getNext()->setPrev(cur->getPrev());
    else tail = cur->getPrev();
    
    delete cur;
    return true;
}

bool DoubleLinkedList::removeBefore(const std::string& target) {
    DNode* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur || !cur->getPrev()) return false;
    
    DNode* toDelete = cur->getPrev();
    if (toDelete->getPrev()) toDelete->getPrev()->setNext(cur);
    else head = cur;
    
    cur->setPrev(toDelete->getPrev());
    delete toDelete;
    return true;
}

bool DoubleLinkedList::removeAfter(const std::string& target) {
    DNode* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur || !cur->getNext()) return false;
    
    DNode* toDelete = cur->getNext();
    if (toDelete->getNext()) toDelete->getNext()->setPrev(cur);
    else tail = cur;
    
    cur->setNext(toDelete->getNext());
    delete toDelete;
    return true;
}

DNode* DoubleLinkedList::getNode(int index) const {
    if (index < 0) return nullptr;  
    DNode* cur = head;
    for (int i = 0; cur && i < index; ++i) cur = cur->getNext();
    return cur;
}

DNode* DoubleLinkedList::getHead() const { return head; }
DNode* DoubleLinkedList::getTail() const { return tail; }

DNode* DoubleLinkedList::findValue(const std::string& val) const {
    DNode* cur = head;
    while (cur && cur->getValue() != val) cur = cur->getNext();
    return cur;
}

void DoubleLinkedList::printList() const {
    DNode* cur = head;
    bool first = true;
    while (cur) {
        if (!first) std::cout << " ";
        std::cout << cur->getValue();
        first = false;
        cur = cur->getNext();
    }
    std::cout << std::endl;
}

void DoubleLinkedList::readFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;
    
    clear();
    std::string val;
    while (in >> val) addTail(val);
    in.close();
}

void DoubleLinkedList::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    DNode* cur = head;
    while (cur) {
        out << cur->getValue();
        if (cur->getNext()) out << " ";
        cur = cur->getNext();
    }
    out.close();
}

bool DoubleLinkedList::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    
    int count = 0;
    for (DNode* cur = head; cur; cur = cur->getNext()) count++;
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (DNode* cur = head; cur; cur = cur->getNext()) {
        size_t len = cur->getValue().size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(cur->getValue().c_str(), len);
    }
    
    out.close();
    return true;
}

bool DoubleLinkedList::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (int i = 0; i < count; i++) {
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string value(len, '\0');
        in.read(&value[0], len);
        
        addTail(value);
    }
    
    in.close();
    return true;
}

void DoubleLinkedList::clear() {
    while (head) removeHead();
}

bool DoubleLinkedList::checkIntegrity() const {
    if (!head && !tail) return true;
    if ((!head && tail) || (head && !tail)) return false;
    
    // Проверка forward links
    DNode* cur = head;
    int count = 0;
    while (cur) {
        if (cur->getNext() && cur->getNext()->getPrev() != cur) return false;
        cur = cur->getNext();
        count++;
        if (count > 10000) return false; // Защита от циклов
    }
    
    // Проверка backward links
    cur = tail;
    while (cur) {
        if (cur->getPrev() && cur->getPrev()->getNext() != cur) return false;
        cur = cur->getPrev();
    }
    
    return true;
}
