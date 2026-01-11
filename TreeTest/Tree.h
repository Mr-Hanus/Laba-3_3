// Tree.h
#pragma once

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <cstring>

// Узел AVL дерева
class AVLNode {
public:
    int key;
    std::unique_ptr<AVLNode> left;
    std::unique_ptr<AVLNode> right;
    int height;

    AVLNode(int k);

    // Методы для сериализации
    void serializeBinary(std::ofstream& out) const;
    static std::unique_ptr<AVLNode> deserializeBinary(std::ifstream& in);
};

class AVLTree {
private:
    std::unique_ptr<AVLNode> root;

    // Получить высоту узла
    int getHeight(const AVLNode* node) const;

    // Получить баланс-фактор узла
    int getBalance(const AVLNode* node) const;

    // Правый поворот
    std::unique_ptr<AVLNode> rightRotate(std::unique_ptr<AVLNode> y);

    // Левый поворот
    std::unique_ptr<AVLNode> leftRotate(std::unique_ptr<AVLNode> x);

    // Рекурсивная вставка
    std::unique_ptr<AVLNode> insert(std::unique_ptr<AVLNode> node, int key);

    // Найти узел с минимальным ключом
    AVLNode* minValueNode(AVLNode* node);

    // Рекурсивное удаление
    std::unique_ptr<AVLNode> deleteNode(std::unique_ptr<AVLNode> node, int key);

    // Обход inorder (сортированный вывод)
    void inorder(const AVLNode* node, std::vector<int>& result) const;

    // Обход preorder
    void preorder(const AVLNode* node, std::vector<int>& result) const;

    // Обход postorder
    void postorder(const AVLNode* node, std::vector<int>& result) const;

    // Поиск элемента
    bool search(const AVLNode* node, int key) const;

    // Рекурсивное копирование дерева
    std::unique_ptr<AVLNode> copyTree(const AVLNode* node);

    // Проверка баланса
    bool isBalancedHelper(const AVLNode* node) const;

    // Получение размера дерева
    size_t sizeHelper(const AVLNode* node) const;

public:
    AVLTree() = default;
    AVLTree(const AVLTree& other);
    AVLTree(AVLTree&& other) noexcept = default;
    ~AVLTree() = default;

    AVLTree& operator=(const AVLTree& other);
    AVLTree& operator=(AVLTree&& other) noexcept = default;

    // Публичные методы
    void insert(int key);
    void remove(int key);
    bool search(int key) const;
    bool contains(int key) const { return search(key); }

    // Обходы
    std::vector<int> inorder() const;
    std::vector<int> preorder() const;
    std::vector<int> postorder() const;

    // Вывод
    void printInorder() const;
    void printPreorder() const;
    void printPostorder() const;

    // Информация о дереве
    int getHeight() const;
    bool isBalanced() const;
    bool isEmpty() const { return root == nullptr; }
    size_t size() const;
    int minValue() const;
    int maxValue() const;

    // Очистка
    void clear();

    // Текстовая сериализация
    void exportToTextFile(const std::string& path) const;
    static AVLTree importFromTextFile(const std::string& path);

    // Бинарная сериализация
    void exportToBinaryFile(const std::string& path) const;
    static AVLTree importFromBinaryFile(const std::string& path);

    // Валидация дерева (проверка свойств AVL)
    bool validate() const;

private:
    bool validateHelper(const AVLNode* node, int& height) const;
};
