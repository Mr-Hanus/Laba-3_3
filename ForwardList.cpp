#include "ForwardList.h"
#include <iostream>

Node::Node(const std::string& val, Node* nxt) : value(val), next(nxt) {}

std::string Node::getValue() const { return value; }
Node* Node::getNext() const { return next; }
void Node::setValue(const std::string& val) { value = val; }
void Node::setNext(Node* nxt) { next = nxt; }

// ForwardList
ForwardList::ForwardList() : head(nullptr) {}


ForwardList::~ForwardList() { clear(); }

void ForwardList::addHead(const std::string& val) {
    head = new Node(val, head);
}

void ForwardList::addTail(const std::string& val) {
    if (!head) {
        head = new Node(val);
        return;
    }
    
    Node* cur = head;
    while (cur->getNext()) cur = cur->getNext();
    cur->setNext(new Node(val));
}

bool ForwardList::addBefore(const std::string& target, const std::string& val) {
    if (!head) return false;
    if (head->getValue() == target) {
        addHead(val);
        return true;
    }
    
    Node* cur = head;
    while (cur->getNext() && cur->getNext()->getValue() != target) 
        cur = cur->getNext();
    
    if (!cur->getNext()) return false;
    cur->setNext(new Node(val, cur->getNext()));
    return true;
}

bool ForwardList::addAfter(const std::string& target, const std::string& val) {
    Node* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur) return false;
    
    cur->setNext(new Node(val, cur->getNext()));
    return true;
}

bool ForwardList::removeHead() {
    if (!head) return false;
    
    Node* tmp = head;
    head = head->getNext();
    delete tmp;
    return true;
}

bool ForwardList::removeTail() {
    if (!head) return false;
    if (!head->getNext()) {
        delete head;
        head = nullptr;
        return true;
    }
    
    Node* cur = head;
    while (cur->getNext()->getNext()) cur = cur->getNext();
    delete cur->getNext();
    cur->setNext(nullptr);
    return true;
}

bool ForwardList::removeValue(const std::string& val) {
    if (!head) return false;
    if (head->getValue() == val) return removeHead();
    
    Node* cur = head;
    while (cur->getNext() && cur->getNext()->getValue() != val) 
        cur = cur->getNext();
    
    if (!cur->getNext()) return false;
    Node* tmp = cur->getNext();
    cur->setNext(cur->getNext()->getNext());
    delete tmp;
    return true;
}

bool ForwardList::removeBefore(const std::string& target) {
    if (!head || head->getValue() == target) return false;
    if (head->getNext() && head->getNext()->getValue() == target) 
        return removeHead();
    
    Node* cur = head;
    while (cur->getNext() && cur->getNext()->getNext() && 
           cur->getNext()->getNext()->getValue() != target) 
        cur = cur->getNext();
    
    if (!cur->getNext() || !cur->getNext()->getNext()) return false;
    Node* tmp = cur->getNext();
    cur->setNext(cur->getNext()->getNext());
    delete tmp;
    return true;
}

bool ForwardList::removeAfter(const std::string& target) {
    Node* cur = head;
    while (cur && cur->getValue() != target) cur = cur->getNext();
    if (!cur || !cur->getNext()) return false;
    
    Node* tmp = cur->getNext();
    cur->setNext(cur->getNext()->getNext());
    delete tmp;
    return true;
}

Node* ForwardList::getNode(int index) const {
    Node* cur = head;
    int i = 0;
    while (cur && i < index) {
        cur = cur->getNext();
        i++;
    }
    return (i == index ? cur : nullptr);
}

Node* ForwardList::findValue(const std::string& val) const {
    Node* cur = head;
    while (cur) {
        if (cur->getValue() == val) return cur;
        cur = cur->getNext();
    }
    return nullptr;
}

void ForwardList::printList() const {
    Node* cur = head;
    while (cur) {
        std::cout << cur->getValue();
        if (cur->getNext()) std::cout << " ";
        cur = cur->getNext();
    }
    std::cout << std::endl;
}

void ForwardList::readFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;
    
    std::string val;
    while (in >> val) addTail(val);
    in.close();
}

void ForwardList::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    Node* cur = head;
    while (cur) {
        out << cur->getValue();
        if (cur->getNext()) out << " ";
        cur = cur->getNext();
    }
    out.close();
}

bool ForwardList::serializeToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    
    // Сначала записываем количество элементов
    Node* current = head;
    int count = 0;
    while (current) {
        count++;
        current = current->getNext();
    }
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // Затем записываем каждый элемент
    current = head;
    while (current) {
        // Записываем длину строки
        size_t len = current->getValue().size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        
        // Записываем саму строку
        out.write(current->getValue().c_str(), len);
        
        current = current->getNext();
    }
    
    out.close();
    return true;
}

// Бинарная десериализация
bool ForwardList::deserializeFromBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    
    // Очищаем текущий список
    clear();
    
    // Читаем количество элементов
    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    // Читаем каждый элемент
    for (int i = 0; i < count; i++) {
        // Читаем длину строки
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        // Читаем строку
        std::string value(len, '\0');
        in.read(&value[0], len);
        
        // Добавляем в список
        addTail(value);
    }
    
    in.close();
    return true;
}


void ForwardList::clear() {
    while (head) removeHead();
}

Node* ForwardList::getHead() const { return head; }
