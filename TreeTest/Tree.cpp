// Tree.cpp
#include "Tree.h"
#include <stdexcept>
#include <queue>
#include <stack>

// AVLNode implementation
AVLNode::AVLNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}

void AVLNode::serializeBinary(std::ofstream& out) const {
    // Записываем ключ и высоту
    out.write(reinterpret_cast<const char*>(&key), sizeof(key));
    out.write(reinterpret_cast<const char*>(&height), sizeof(height));

    // Флаги наличия потомков
    bool hasLeft = (left != nullptr);
    bool hasRight = (right != nullptr);
    out.write(reinterpret_cast<const char*>(&hasLeft), sizeof(hasLeft));
    out.write(reinterpret_cast<const char*>(&hasRight), sizeof(hasRight));

    // Рекурсивно сериализуем потомков
    if (hasLeft) {
        left->serializeBinary(out);
    }
    if (hasRight) {
        right->serializeBinary(out);
    }
}

std::unique_ptr<AVLNode> AVLNode::deserializeBinary(std::ifstream& in) {
    int nodeKey, nodeHeight;
    bool hasLeft, hasRight;

    // Читаем данные узла
    in.read(reinterpret_cast<char*>(&nodeKey), sizeof(nodeKey));
    in.read(reinterpret_cast<char*>(&nodeHeight), sizeof(nodeHeight));
    in.read(reinterpret_cast<char*>(&hasLeft), sizeof(hasLeft));
    in.read(reinterpret_cast<char*>(&hasRight), sizeof(hasRight));

    if (!in) {
        throw std::runtime_error("Error reading binary data");
    }

    auto node = std::make_unique<AVLNode>(nodeKey);
    node->height = nodeHeight;

    // Десериализуем потомков
    if (hasLeft) {
        node->left = deserializeBinary(in);
    }
    if (hasRight) {
        node->right = deserializeBinary(in);
    }

    return node;
}

// AVLTree implementation
int AVLTree::getHeight(const AVLNode* node) const {
    return node ? node->height : 0;
}

int AVLTree::getBalance(const AVLNode* node) const {
    return node ? getHeight(node->left.get()) - getHeight(node->right.get()) : 0;
}

std::unique_ptr<AVLNode> AVLTree::rightRotate(std::unique_ptr<AVLNode> y) {
    auto x = std::move(y->left);
    auto T2 = std::move(x->right);

    // Выполняем поворот
    x->right = std::move(y);
    x->right->left = std::move(T2);

    // Обновляем высоты
    x->right->height = std::max(getHeight(x->right->left.get()),
                               getHeight(x->right->right.get())) + 1;
    x->height = std::max(getHeight(x->left.get()), getHeight(x->right.get())) + 1;

    return x;
}

std::unique_ptr<AVLNode> AVLTree::leftRotate(std::unique_ptr<AVLNode> x) {
    auto y = std::move(x->right);
    auto T2 = std::move(y->left);

    // Выполняем поворот
    y->left = std::move(x);
    y->left->right = std::move(T2);

    // Обновляем высоты
    y->left->height = std::max(getHeight(y->left->left.get()),
                              getHeight(y->left->right.get())) + 1;
    y->height = std::max(getHeight(y->left.get()), getHeight(y->right.get())) + 1;

    return y;
}

std::unique_ptr<AVLNode> AVLTree::insert(std::unique_ptr<AVLNode> node, int key) {
    if (!node) {
        return std::make_unique<AVLNode>(key);
    }

    if (key < node->key) {
        node->left = insert(std::move(node->left), key);
    } else if (key > node->key) {
        node->right = insert(std::move(node->right), key);
    } else {
        // Дубликаты не разрешены
        return node;
    }

    // Обновляем высоту
    node->height = 1 + std::max(getHeight(node->left.get()),
                               getHeight(node->right.get()));

    // Получаем баланс-фактор
    int balance = getBalance(node.get());

    // Балансировка
    if (balance > 1 && key < node->left->key) {
        return rightRotate(std::move(node));
    }
    if (balance < -1 && key > node->right->key) {
        return leftRotate(std::move(node));
    }
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(std::move(node->left));
        return rightRotate(std::move(node));
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(std::move(node->right));
        return leftRotate(std::move(node));
    }

    return node;
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left) {
        current = current->left.get();
    }
    return current;
}

std::unique_ptr<AVLNode> AVLTree::deleteNode(std::unique_ptr<AVLNode> node, int key) {
    if (!node) return nullptr;

    if (key < node->key) {
        node->left = deleteNode(std::move(node->left), key);
    } else if (key > node->key) {
        node->right = deleteNode(std::move(node->right), key);
    } else {
        // Узел для удаления найден
        if (!node->left || !node->right) {
            // Один или ноль потомков
            auto temp = std::move(node->left ? node->left : node->right);
            node.reset();
            return temp;
        } else {
            // Два потомка
            AVLNode* temp = minValueNode(node->right.get());
            node->key = temp->key;
            node->right = deleteNode(std::move(node->right), temp->key);
        }
    }

    if (!node) return nullptr;

    // Обновляем высоту
    node->height = 1 + std::max(getHeight(node->left.get()),
                               getHeight(node->right.get()));

    // Получаем баланс-фактор
    int balance = getBalance(node.get());

    // Балансировка
    if (balance > 1 && getBalance(node->left.get()) >= 0) {
        return rightRotate(std::move(node));
    }
    if (balance > 1 && getBalance(node->left.get()) < 0) {
        node->left = leftRotate(std::move(node->left));
        return rightRotate(std::move(node));
    }
    if (balance < -1 && getBalance(node->right.get()) <= 0) {
        return leftRotate(std::move(node));
    }
    if (balance < -1 && getBalance(node->right.get()) > 0) {
        node->right = rightRotate(std::move(node->right));
        return leftRotate(std::move(node));
    }

    return node;
}

void AVLTree::inorder(const AVLNode* node, std::vector<int>& result) const {
    if (node) {
        inorder(node->left.get(), result);
        result.push_back(node->key);
        inorder(node->right.get(), result);
    }
}

void AVLTree::preorder(const AVLNode* node, std::vector<int>& result) const {
    if (node) {
        result.push_back(node->key);
        preorder(node->left.get(), result);
        preorder(node->right.get(), result);
    }
}

void AVLTree::postorder(const AVLNode* node, std::vector<int>& result) const {
    if (node) {
        postorder(node->left.get(), result);
        postorder(node->right.get(), result);
        result.push_back(node->key);
    }
}

bool AVLTree::search(const AVLNode* node, int key) const {
    if (!node) return false;
    if (node->key == key) return true;
    return key < node->key ? search(node->left.get(), key) :
                             search(node->right.get(), key);
}

std::unique_ptr<AVLNode> AVLTree::copyTree(const AVLNode* node) {
    if (!node) return nullptr;

    auto newNode = std::make_unique<AVLNode>(node->key);
    newNode->height = node->height;
    newNode->left = copyTree(node->left.get());
    newNode->right = copyTree(node->right.get());

    return newNode;
}

bool AVLTree::isBalancedHelper(const AVLNode* node) const {
    if (!node) return true;

    int balance = getBalance(node);
    if (std::abs(balance) > 1) return false;

    return isBalancedHelper(node->left.get()) &&
           isBalancedHelper(node->right.get());
}

size_t AVLTree::sizeHelper(const AVLNode* node) const {
    return node ? 1 + sizeHelper(node->left.get()) + sizeHelper(node->right.get()) : 0;
}

// Конструктор копирования
AVLTree::AVLTree(const AVLTree& other) {
    root = copyTree(other.root.get());
}

// Оператор присваивания копированием
AVLTree& AVLTree::operator=(const AVLTree& other) {
    if (this != &other) {
        root = copyTree(other.root.get());
    }
    return *this;
}

void AVLTree::insert(int key) {
    root = insert(std::move(root), key);
}

void AVLTree::remove(int key) {
    root = deleteNode(std::move(root), key);
}

bool AVLTree::search(int key) const {
    return search(root.get(), key);
}
std::vector<int> AVLTree::inorder() const {
    std::vector<int> result;
    inorder(root.get(), result);
    return result;
}

std::vector<int> AVLTree::preorder() const {
    std::vector<int> result;
    preorder(root.get(), result);
    return result;
}

std::vector<int> AVLTree::postorder() const {
    std::vector<int> result;
    postorder(root.get(), result);
    return result;
}

void AVLTree::printInorder() const {
    std::cout << "Inorder: ";
    for (int key : inorder()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
}

void AVLTree::printPreorder() const {
    std::cout << "Preorder: ";
    for (int key : preorder()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
}

void AVLTree::printPostorder() const {
    std::cout << "Postorder: ";
    for (int key : postorder()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
}

int AVLTree::getHeight() const {
    return getHeight(root.get());
}

bool AVLTree::isBalanced() const {
    return isBalancedHelper(root.get());
}

size_t AVLTree::size() const {
    return sizeHelper(root.get());
}

int AVLTree::minValue() const {
    if (!root) throw std::runtime_error("Tree is empty");

    const AVLNode* current = root.get();
    while (current->left) {
        current = current->left.get();
    }
    return current->key;
}

int AVLTree::maxValue() const {
    if (!root) throw std::runtime_error("Tree is empty");

    const AVLNode* current = root.get();
    while (current->right) {
        current = current->right.get();
    }
    return current->key;
}

void AVLTree::clear() {
    root.reset();
}

// Текстовая сериализация
void AVLTree::exportToTextFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    auto keys = preorder();
    for (size_t i = 0; i < keys.size(); ++i) {
        out << keys[i];
        if (i != keys.size() - 1) out << " ";
    }
}

AVLTree AVLTree::importFromTextFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    AVLTree tree;
    int key;
    while (in >> key) {
        tree.insert(key);
    }

    return tree;
}

// Бинарная сериализация
void AVLTree::exportToBinaryFile(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open binary file: " + path);
    }

    // Записываем количество узлов (для проверки при чтении)
    size_t nodeCount = size();
    out.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

    // Сериализуем дерево
    if (root) {
        root->serializeBinary(out);
    }
}

AVLTree AVLTree::importFromBinaryFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open binary file: " + path);
    }

    AVLTree tree;

    // Читаем количество узлов
    size_t expectedNodeCount;
    in.read(reinterpret_cast<char*>(&expectedNodeCount), sizeof(expectedNodeCount));

    if (!in) {
        throw std::runtime_error("Error reading binary file header");
    }

    if (expectedNodeCount > 0) {
        tree.root = AVLNode::deserializeBinary(in);

        // Проверяем, что прочитали все узлы
        if (tree.size() != expectedNodeCount) {
            throw std::runtime_error("Binary file corrupted: node count mismatch");
        }
    }

    return tree;
}

// Валидация дерева
bool AVLTree::validateHelper(const AVLNode* node, int& height) const {
    if (!node) {
        height = 0;
        return true;
    }

    int leftHeight, rightHeight;
    bool leftValid = validateHelper(node->left.get(), leftHeight);
    bool rightValid = validateHelper(node->right.get(), rightHeight);

    height = 1 + std::max(leftHeight, rightHeight);

    // Проверяем баланс
    int balance = leftHeight - rightHeight;
    if (std::abs(balance) > 1) {
        return false;
    }

    // Проверяем свойства BST
    if (node->left && node->left->key >= node->key) {
        return false;
    }
    if (node->right && node->right->key <= node->key) {
        return false;
    }

    return leftValid && rightValid && (node->height == height);
}

bool AVLTree::validate() const {
    int height;
    return validateHelper(root.get(), height);
}
