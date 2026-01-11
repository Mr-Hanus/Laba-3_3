#ifdef TREEBENCHMARK
#define BOOST_TEST_MODULE AVLTreeBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Tree.h"
#include <random>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <set>
#include <algorithm>

// Генерация случайных уникальных ключей
std::vector<int> generateUniqueKeys(size_t count, int minVal = 1, int maxVal = 1000000) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minVal, maxVal);

    std::set<int> uniqueKeys;
    while (uniqueKeys.size() < count) {
        uniqueKeys.insert(dist(rng));
    }

    return std::vector<int>(uniqueKeys.begin(), uniqueKeys.end());
}

BOOST_AUTO_TEST_CASE(BenchmarkInsertRandom) {
    const size_t SIZES[] = {100, 1000, 10000, 50000};

    for (size_t size : SIZES) {
        auto keys = generateUniqueKeys(size);

        auto start = std::chrono::high_resolution_clock::now();

        AVLTree tree;
        for (int key : keys) {
            tree.insert(key);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        BOOST_TEST_MESSAGE("Insert " << size << " random elements: "
                          << duration.count() << " ms");
        BOOST_CHECK_EQUAL(tree.size(), size);
        BOOST_CHECK(tree.isBalanced());
        BOOST_CHECK(tree.validate());
    }
}

BOOST_AUTO_TEST_CASE(BenchmarkSearch) {
    const size_t SIZE = 10000;
    auto keys = generateUniqueKeys(SIZE);

    AVLTree tree;
    for (int key : keys) {
        tree.insert(key);
    }

    // Поиск существующих элементов
    auto start = std::chrono::high_resolution_clock::now();

    int foundCount = 0;
    for (int i = 0; i < 1000; ++i) {
        int index = i % keys.size();
        if (tree.search(keys[index])) {
            ++foundCount;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    BOOST_TEST_MESSAGE("Search 1000 existing elements: "
                      << duration.count() << " µs ("
                      << duration.count() / 1000.0 << " µs per search)");
    BOOST_CHECK_EQUAL(foundCount, 1000);

    // Поиск несуществующих элементов
    start = std::chrono::high_resolution_clock::now();

    int notFoundCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (!tree.search(-i - 1)) { // Отрицательные числа точно не в дереве
            ++notFoundCount;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    BOOST_TEST_MESSAGE("Search 1000 non-existing elements: "
                      << duration.count() << " µs ("
                      << duration.count() / 1000.0 << " µs per search)");
    BOOST_CHECK_EQUAL(notFoundCount, 1000);
}

BOOST_AUTO_TEST_CASE(BenchmarkRemove) {
    const size_t SIZE = 10000;
    auto keys = generateUniqueKeys(SIZE);

    AVLTree tree;
    for (int key : keys) {
        tree.insert(key);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // Удаляем каждое десятое число
    size_t removedCount = 0;
    for (size_t i = 0; i < keys.size(); i += 10) {
        tree.remove(keys[i]);
        ++removedCount;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    BOOST_TEST_MESSAGE("Remove " << removedCount << " elements from tree of size "
                      << SIZE << ": " << duration.count() << " ms");
    BOOST_CHECK_EQUAL(tree.size(), SIZE - removedCount);
    BOOST_CHECK(tree.isBalanced());
    BOOST_CHECK(tree.validate());
}

BOOST_AUTO_TEST_CASE(BenchmarkTraversals) {
    const size_t SIZE = 50000;
    auto keys = generateUniqueKeys(SIZE);

    AVLTree tree;
    for (int key : keys) {
        tree.insert(key);
    }

    // Inorder traversal
    auto start = std::chrono::high_resolution_clock::now();
    auto inorderResult = tree.inorder();
    auto end = std::chrono::high_resolution_clock::now();
    auto inorderTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    BOOST_TEST_MESSAGE("Inorder traversal of " << SIZE << " elements: "
                      << inorderTime.count() << " µs");
    BOOST_CHECK(std::is_sorted(inorderResult.begin(), inorderResult.end()));

    // Preorder traversal
    start = std::chrono::high_resolution_clock::now();
    auto preorderResult = tree.preorder();
    end = std::chrono::high_resolution_clock::now();
    auto preorderTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    BOOST_TEST_MESSAGE("Preorder traversal of " << SIZE << " elements: "
                      << preorderTime.count() << " µs");
    BOOST_CHECK_EQUAL(preorderResult.size(), SIZE);

    // Postorder traversal
    start = std::chrono::high_resolution_clock::now();
    auto postorderResult = tree.postorder();
    end = std::chrono::high_resolution_clock::now();
    auto postorderTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    BOOST_TEST_MESSAGE("Postorder traversal of " << SIZE << " elements: "
                      << postorderTime.count() << " µs");
    BOOST_CHECK_EQUAL(postorderResult.size(), SIZE);
}

BOOST_AUTO_TEST_CASE(BenchmarkTextSerialization) {
    const size_t SIZE = 10000;
    const std::string filename = "benchmark_tree.txt";

    AVLTree tree;
    auto keys = generateUniqueKeys(SIZE);
    for (int key : keys) {
        tree.insert(key);
    }

    // Экспорт
    auto startExport = std::chrono::high_resolution_clock::now();
    tree.exportToTextFile(filename);
    auto endExport = std::chrono::high_resolution_clock::now();
    auto exportTime = std::chrono::duration_cast<std::chrono::milliseconds>(endExport - startExport);

    // Импорт
    auto startImport = std::chrono::high_resolution_clock::now();
    AVLTree imported = AVLTree::importFromTextFile(filename);
    auto endImport = std::chrono::high_resolution_clock::now();
    auto importTime = std::chrono::duration_cast<std::chrono::milliseconds>(endImport - startImport);

    // Проверка размера файла
    std::ifstream in(filename, std::ios::ate | std::ios::binary);
    size_t fileSize = in.tellg();
    in.close();

    BOOST_TEST_MESSAGE("Text serialization of " << SIZE << " elements:");
    BOOST_TEST_MESSAGE("  Export: " << exportTime.count() << " ms");
    BOOST_TEST_MESSAGE("  Import: " << importTime.count() << " ms");
    BOOST_TEST_MESSAGE("  File size: " << fileSize << " bytes");

    BOOST_CHECK_EQUAL(imported.size(), SIZE);
    BOOST_CHECK(imported.validate());

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(BenchmarkBinarySerialization) {
    const size_t SIZE = 10000;
    const std::string filename = "benchmark_tree.bin";

    AVLTree tree;
    auto keys = generateUniqueKeys(SIZE);
    for (int key : keys) {
        tree.insert(key);
    }

    // Экспорт
    auto startExport = std::chrono::high_resolution_clock::now();
    tree.exportToBinaryFile(filename);
    auto endExport = std::chrono::high_resolution_clock::now();
    auto exportTime = std::chrono::duration_cast<std::chrono::milliseconds>(endExport - startExport);

    // Импорт
    auto startImport = std::chrono::high_resolution_clock::now();
    AVLTree imported = AVLTree::importFromBinaryFile(filename);
    auto endImport = std::chrono::high_resolution_clock::now();
    auto importTime = std::chrono::duration_cast<std::chrono::milliseconds>(endImport - startImport);

    // Проверка размера файла
    std::ifstream in(filename, std::ios::ate | std::ios::binary);
    size_t fileSize = in.tellg();
    in.close();

    BOOST_TEST_MESSAGE("Binary serialization of " << SIZE << " elements:");
    BOOST_TEST_MESSAGE("  Export: " << exportTime.count() << " ms");
    BOOST_TEST_MESSAGE("  Import: " << importTime.count() << " ms");
    BOOST_TEST_MESSAGE("  File size: " << fileSize << " bytes");

    BOOST_CHECK_EQUAL(imported.size(), SIZE);
    BOOST_CHECK(imported.validate());

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(BenchmarkSerializationComparison) {
    const size_t SIZES[] = {100, 1000, 5000, 10000};

    BOOST_TEST_MESSAGE("Serialization comparison (Text vs Binary):");
    BOOST_TEST_MESSAGE("Size\tText Export\tText Import\tText Size\tBin Export\tBin Import\tBin Size");

    for (size_t size : SIZES) {
        AVLTree tree;
        auto keys = generateUniqueKeys(size, 1, size * 10);
        for (int key : keys) {
            tree.insert(key);
        }

        const std::string textFile = "compare_text_" + std::to_string(size) + ".txt";
        const std::string binFile = "compare_bin_" + std::to_string(size) + ".bin";

        // Текстовый формат
        auto start = std::chrono::high_resolution_clock::now();
        tree.exportToTextFile(textFile);
        auto end = std::chrono::high_resolution_clock::now();
        auto textExportTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        start = std::chrono::high_resolution_clock::now();
        AVLTree fromText = AVLTree::importFromTextFile(textFile);
        end = std::chrono::high_resolution_clock::now();
        auto textImportTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::ifstream textIn(textFile, std::ios::ate | std::ios::binary);
        size_t textSize = textIn.tellg();
        textIn.close();

        // Бинарный формат
        start = std::chrono::high_resolution_clock::now();
        tree.exportToBinaryFile(binFile);
        end = std::chrono::high_resolution_clock::now();
        auto binExportTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        start = std::chrono::high_resolution_clock::now();
        AVLTree fromBinary = AVLTree::importFromBinaryFile(binFile);
        end = std::chrono::high_resolution_clock::now();
        auto binImportTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::ifstream binIn(binFile, std::ios::ate | std::ios::binary);
        size_t binSize = binIn.tellg();
        binIn.close();

        BOOST_TEST_MESSAGE(size << "\t"
                          << textExportTime.count() << "µs\t"
                          << textImportTime.count() << "µs\t"
                          << textSize << "B\t"
                          << binExportTime.count() << "µs\t"
                          << binImportTime.count() << "µs\t"
                          << binSize << "B");

        // Проверка целостности
        BOOST_CHECK_EQUAL(fromText.size(), size);
        BOOST_CHECK_EQUAL(fromBinary.size(), size);

        std::remove(textFile.c_str());
        std::remove(binFile.c_str());
    }
}

BOOST_AUTO_TEST_CASE(BenchmarkCopyAndAssignment) {
    const size_t SIZE = 50000;

    // Создаем большое дерево
    AVLTree original;
    auto keys = generateUniqueKeys(SIZE);
    for (int key : keys) {
        original.insert(key);
    }

    // Копирование конструктором
    auto startCopy = std::chrono::high_resolution_clock::now();
    AVLTree copied(original);
    auto endCopy = std::chrono::high_resolution_clock::now();
    auto copyTime = std::chrono::duration_cast<std::chrono::milliseconds>(endCopy - startCopy);

    BOOST_TEST_MESSAGE("Copy constructor of " << SIZE << " elements: "
                      << copyTime.count() << " ms");
    BOOST_CHECK_EQUAL(copied.size(), SIZE);
    BOOST_CHECK(copied.validate());

    // Оператор присваивания
    AVLTree assigned;
    auto startAssign = std::chrono::high_resolution_clock::now();
    assigned = original;
    auto endAssign = std::chrono::high_resolution_clock::now();
    auto assignTime = std::chrono::duration_cast<std::chrono::milliseconds>(endAssign - startAssign);

    BOOST_TEST_MESSAGE("Assignment operator of " << SIZE << " elements: "
                      << assignTime.count() << " ms");
    BOOST_CHECK_EQUAL(assigned.size(), SIZE);
    BOOST_CHECK(assigned.validate());
}

#endif