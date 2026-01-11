#ifdef TEST_BUILD
#define BOOST_TEST_MODULE HashTableTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "HashTable.h"
#include <fstream>
#include <cstdio>
#include <vector>
#include <random>
#include <sstream>

std::string captureOutput(std::function<void()> func) {
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    func();
    std::cout.rdbuf(old);
    return oss.str();
}

BOOST_AUTO_TEST_CASE(test_constructor) {
    HashTable ht;
    BOOST_CHECK(ht.isEmpty());
    BOOST_CHECK(ht.getSize() == 0);
    BOOST_CHECK(ht.getCapacity() >= 11); 
}

BOOST_AUTO_TEST_CASE(test_insert_and_search) {
    HashTable ht;
    BOOST_CHECK(ht.insert(10, "value10"));
    BOOST_CHECK(ht.insert(20, "value20"));
    BOOST_CHECK(ht.insert(30, "value30"));

    BOOST_CHECK(ht.search(10) == "value10");
    BOOST_CHECK(ht.search(20) == "value20");
    BOOST_CHECK(ht.search(30) == "value30");
    BOOST_CHECK(ht.search(999) == "");

    BOOST_CHECK(ht.getSize() == 3);
    BOOST_CHECK(!ht.isEmpty());
}

BOOST_AUTO_TEST_CASE(test_insert_duplicate_key) {
    HashTable ht;
    BOOST_CHECK(ht.insert(42, "first"));
    BOOST_CHECK(ht.search(42) == "first");

    BOOST_CHECK(ht.insert(42, "second")); // обновление
    BOOST_CHECK(ht.search(42) == "second");
    BOOST_CHECK(ht.getSize() == 1); // размер не вырос
}

BOOST_AUTO_TEST_CASE(test_remove) {
    HashTable ht;
    ht.insert(1, "one");
    ht.insert(2, "two");
    ht.insert(3, "three");

    BOOST_CHECK(ht.remove(2) == true);
    BOOST_CHECK(ht.search(2) == "");
    BOOST_CHECK(ht.getSize() == 2);

    BOOST_CHECK(ht.remove(999) == false); // несуществующий
    BOOST_CHECK(ht.getSize() == 2);
}

BOOST_AUTO_TEST_CASE(test_rehash_on_load_factor) {
    HashTable ht(5); // маленькая начальная ёмкость для быстрого рехэша

    // Заполняем до load factor > 0.7
    ht.insert(1, "a");
    ht.insert(2, "b");
    ht.insert(3, "c");
    ht.insert(4, "d");

    int oldCapacity = ht.getCapacity();
    ht.insert(5, "e"); // должно вызвать рехэш

    BOOST_CHECK(ht.getCapacity() > oldCapacity);
    BOOST_CHECK(ht.getSize() == 5);
    BOOST_CHECK(ht.search(1) == "a");
    BOOST_CHECK(ht.search(5) == "e");
}

BOOST_AUTO_TEST_CASE(test_clear) {
    HashTable ht;
    ht.insert(10, "ten");
    ht.insert(20, "twenty");

    ht.clear();
    BOOST_CHECK(ht.isEmpty());
    BOOST_CHECK(ht.getSize() == 0);
    BOOST_CHECK(ht.search(10) == "");

    // Можно вставлять снова
    ht.insert(30, "thirty");
    BOOST_CHECK(ht.search(30) == "thirty");
}

BOOST_AUTO_TEST_CASE(test_print) {
    HashTable ht;
    ht.insert(10, "ten");
    ht.insert(20, "twenty");

    std::string output = captureOutput([&]() { ht.print(); });
    BOOST_CHECK(output.find("10 -> ten") != std::string::npos);
    BOOST_CHECK(output.find("20 -> twenty") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_text_file_io) {
    HashTable ht;
    ht.insert(100, "hundred");
    ht.insert(200, "two hundred");

    const std::string filename = "hash_test.txt";
    ht.writeToFile(filename);

    HashTable ht2;
    ht2.readFromFile(filename);

    BOOST_CHECK(ht2.search(100) == "hundred");
    BOOST_CHECK(ht2.search(200) == "two hundred");
    BOOST_CHECK(ht2.getSize() == 2);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_binary_serialization) {
    HashTable original;
    original.insert(42, "answer");
    original.insert(123, "test value with space");
    original.insert(0, "");

    const std::string binfile = "hash_bin.bin";
    BOOST_CHECK(original.serializeToBinary(binfile));

    HashTable loaded;
    BOOST_CHECK(loaded.deserializeFromBinary(binfile));

    BOOST_CHECK(loaded.search(42) == "answer");
    BOOST_CHECK(loaded.search(123) == "test value with space");
    BOOST_CHECK(loaded.search(0) == "");
    BOOST_CHECK(loaded.getSize() == 3);

    std::remove(binfile.c_str());
}

BOOST_AUTO_TEST_CASE(test_binary_empty_and_fail) {
    HashTable empty;
    const std::string emptybin = "empty_hash.bin";
    BOOST_CHECK(empty.serializeToBinary(emptybin));

    HashTable loaded;
    BOOST_CHECK(loaded.deserializeFromBinary(emptybin));
    BOOST_CHECK(loaded.isEmpty());

    std::remove(emptybin.c_str());

    // Файл не открывается
    HashTable ht;
    BOOST_CHECK(!ht.serializeToBinary("/invalid/path/file.bin"));
}
#endif
