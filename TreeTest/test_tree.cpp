#ifdef TREETEST
#define BOOST_TEST_MODULE AVLTreeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Tree.h"
#include <random>
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_CASE(DefaultConstructor) {
    AVLTree tree;
    BOOST_CHECK(tree.isEmpty());
    BOOST_CHECK_EQUAL(tree.size(), 0);
    BOOST_CHECK_EQUAL(tree.getHeight(), 0);
    BOOST_CHECK(tree.isBalanced());
}

BOOST_AUTO_TEST_CASE(InsertAndSearch) {
    AVLTree tree;

    tree.insert(10);
    BOOST_CHECK_EQUAL(tree.size(), 1);
    BOOST_CHECK(tree.search(10));
    BOOST_CHECK(!tree.search(5));

    tree.insert(5);
    tree.insert(15);
    BOOST_CHECK_EQUAL(tree.size(), 3);
    BOOST_CHECK(tree.search(5));
    BOOST_CHECK(tree.search(15));

    // Дубликаты не добавляются
    tree.insert(10);
    BOOST_CHECK_EQUAL(tree.size(), 3);
}

BOOST_AUTO_TEST_CASE(Remove) {
    AVLTree tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(3);
    tree.insert(7);
    tree.insert(12);
    tree.insert(20);

    BOOST_CHECK_EQUAL(tree.size(), 7);

    // Удаление листа
    tree.remove(3);
    BOOST_CHECK_EQUAL(tree.size(), 6);
    BOOST_CHECK(!tree.search(3));

    // Удаление узла с одним потомком
    tree.remove(5);
    BOOST_CHECK_EQUAL(tree.size(), 5);
    BOOST_CHECK(!tree.search(5));

    // Удаление узла с двумя потомками
    tree.remove(15);
    BOOST_CHECK_EQUAL(tree.size(), 4);
    BOOST_CHECK(!tree.search(15));

    // Удаление несуществующего элемента
    tree.remove(100);
    BOOST_CHECK_EQUAL(tree.size(), 4);

    // Удаление всех элементов
    tree.remove(10);
    tree.remove(7);
    tree.remove(12);
    tree.remove(20);
    BOOST_CHECK(tree.isEmpty());
}

BOOST_AUTO_TEST_CASE(Traversals) {
    AVLTree tree;
    tree.insert(4);
    tree.insert(2);
    tree.insert(6);
    tree.insert(1);
    tree.insert(3);
    tree.insert(5);
    tree.insert(7);

    auto inorder = tree.inorder();
    std::vector<int> expectedInorder = {1, 2, 3, 4, 5, 6, 7};
    BOOST_CHECK_EQUAL_COLLECTIONS(inorder.begin(), inorder.end(),
                                 expectedInorder.begin(), expectedInorder.end());

    auto preorder = tree.preorder();
    // Preorder должен начинаться с корня
    BOOST_CHECK_EQUAL(preorder[0], 4);
    BOOST_CHECK_EQUAL(preorder.size(), 7);

    auto postorder = tree.postorder();
    // Postorder должен заканчиваться корнем
    BOOST_CHECK_EQUAL(postorder[6], 4);
    BOOST_CHECK_EQUAL(postorder.size(), 7);
}

BOOST_AUTO_TEST_CASE(Balance) {
    AVLTree tree;

    // Создаем несбалансированное дерево (вставка по порядку)
    for (int i = 1; i <= 10; ++i) {
        tree.insert(i);
        BOOST_CHECK(tree.isBalanced()); // AVL должно оставаться сбалансированным
    }

    BOOST_CHECK_EQUAL(tree.size(), 10);
    BOOST_CHECK(tree.validate());
}

BOOST_AUTO_TEST_CASE(MinMaxValues) {
    AVLTree tree;

    // Пустое дерево
    BOOST_CHECK_THROW(tree.minValue(), std::runtime_error);
    BOOST_CHECK_THROW(tree.maxValue(), std::runtime_error);

    tree.insert(5);
    BOOST_CHECK_EQUAL(tree.minValue(), 5);
    BOOST_CHECK_EQUAL(tree.maxValue(), 5);

    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(9);

    BOOST_CHECK_EQUAL(tree.minValue(), 1);
    BOOST_CHECK_EQUAL(tree.maxValue(), 9);
}

BOOST_AUTO_TEST_CASE(CopyConstructor) {
    AVLTree tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);

    AVLTree tree2(tree1);
    BOOST_CHECK_EQUAL(tree2.size(), 3);
    BOOST_CHECK(tree2.search(5));
    BOOST_CHECK(tree2.search(3));
    BOOST_CHECK(tree2.search(7));

    // Проверяем, что это глубокие копии
    tree1.insert(10);
    BOOST_CHECK_EQUAL(tree1.size(), 4);
    BOOST_CHECK_EQUAL(tree2.size(), 3);
    BOOST_CHECK(!tree2.search(10));
}

BOOST_AUTO_TEST_CASE(AssignmentOperator) {
    AVLTree tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);

    AVLTree tree2;
    tree2 = tree1;

    BOOST_CHECK_EQUAL(tree2.size(), 3);
    BOOST_CHECK(tree2.search(5));
    BOOST_CHECK(tree2.search(3));
    BOOST_CHECK(tree2.search(7));

    // Самоназначение
    tree2 = tree2;
    BOOST_CHECK_EQUAL(tree2.size(), 3);
}

BOOST_AUTO_TEST_CASE(Clear) {
    AVLTree tree;
    for (int i = 0; i < 10; ++i) {
        tree.insert(i);
    }

    BOOST_CHECK_EQUAL(tree.size(), 10);
    tree.clear();
    BOOST_CHECK(tree.isEmpty());
    BOOST_CHECK_EQUAL(tree.size(), 0);
}

BOOST_AUTO_TEST_CASE(TextSerialization) {
    const std::string filename = "test_tree.txt";

    AVLTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    // Экспорт
    tree.exportToTextFile(filename);

    // Импорт
    AVLTree imported = AVLTree::importFromTextFile(filename);

    // Проверка
    BOOST_CHECK_EQUAL(imported.size(), tree.size());

    auto originalInorder = tree.inorder();
    auto importedInorder = imported.inorder();
    BOOST_CHECK_EQUAL_COLLECTIONS(originalInorder.begin(), originalInorder.end(),
                                 importedInorder.begin(), importedInorder.end());

    // Проверка целостности
    BOOST_CHECK(imported.isBalanced());
    BOOST_CHECK(imported.validate());

    // Очистка
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(BinarySerialization) {
    const std::string filename = "test_tree.bin";

    AVLTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    // Экспорт
    tree.exportToBinaryFile(filename);

    // Проверяем размер файла
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    size_t fileSize = in.tellg();
    in.close();
    BOOST_CHECK(fileSize > 0);

    // Импорт
    AVLTree imported = AVLTree::importFromBinaryFile(filename);

    // Проверка
    BOOST_CHECK_EQUAL(imported.size(), tree.size());

    auto originalInorder = tree.inorder();
    auto importedInorder = imported.inorder();
    BOOST_CHECK_EQUAL_COLLECTIONS(originalInorder.begin(), originalInorder.end(),
                                 importedInorder.begin(), importedInorder.end());

    // Проверка целостности
    BOOST_CHECK(imported.isBalanced());
    BOOST_CHECK(imported.validate());

    // Очистка
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(BinarySerializationEmptyTree) {
    const std::string filename = "empty_tree.bin";

    AVLTree tree;
    tree.exportToBinaryFile(filename);

    AVLTree imported = AVLTree::importFromBinaryFile(filename);
    BOOST_CHECK(imported.isEmpty());

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(BinarySerializationLargeTree) {
    const std::string filename = "large_tree.bin";

    AVLTree tree;
    for (int i = 0; i < 100; ++i) {
        tree.insert(i * 2); // Четные числа
    }

    tree.exportToBinaryFile(filename);
    AVLTree imported = AVLTree::importFromBinaryFile(filename);

    BOOST_CHECK_EQUAL(imported.size(), 100);
    BOOST_CHECK(imported.isBalanced());
    BOOST_CHECK(imported.validate());

    for (int i = 0; i < 100; ++i) {
        BOOST_CHECK(imported.search(i * 2));
        BOOST_CHECK(!imported.search(i * 2 + 1));
    }

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(ValidateAVLProperties) {
    AVLTree tree;

    // Создаем корректное AVL дерево
    for (int i = 0; i < 20; ++i) {
        tree.insert(i * 3 % 20); // Неупорядоченная вставка
        BOOST_CHECK(tree.validate());
    }

    // Проверяем все свойства
    BOOST_CHECK(tree.isBalanced());
    BOOST_CHECK(tree.validate());

    // Проверяем, что inorder дает отсортированный результат
    auto traversal = tree.inorder();
    BOOST_CHECK(std::is_sorted(traversal.begin(), traversal.end()));

    // Проверяем, что нет дубликатов
    auto it = std::unique(traversal.begin(), traversal.end());
    BOOST_CHECK(it == traversal.end());
}

BOOST_AUTO_TEST_CASE(StressTest) {
    AVLTree tree;
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(1, 10000);

    // Вставка 1000 случайных элементов
    for (int i = 0; i < 1000; ++i) {
        tree.insert(dist(rng));
        BOOST_CHECK(tree.isBalanced());
    }

    BOOST_CHECK(tree.validate());
    BOOST_CHECK(tree.size() <= 1000); // Могут быть дубликаты

    // Удаление случайных элементов
    for (int i = 0; i < 500; ++i) {
        tree.remove(dist(rng));
        BOOST_CHECK(tree.isBalanced());
    }

    BOOST_CHECK(tree.validate());
}
#endif