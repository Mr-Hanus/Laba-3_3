#ifdef TEST_BUILD
#define BOOST_TEST_MODULE CuckooHashTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "CuckooHash.h"
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

BOOST_AUTO_TEST_CASE(test_basic_operations) {
    CuckooHashTable ht;

    BOOST_CHECK(ht.insert(10, "ten"));
    BOOST_CHECK(ht.insert(20, "twenty"));
    BOOST_CHECK(ht.insert(30, "thirty"));

    BOOST_CHECK(ht.search(10) == "ten");
    BOOST_CHECK(ht.search(20) == "twenty");
    BOOST_CHECK(ht.search(30) == "thirty");
    BOOST_CHECK(ht.search(999) == "");

    BOOST_CHECK(ht.getSize() == 3);
    BOOST_CHECK(!ht.isEmpty());
}

BOOST_AUTO_TEST_CASE(test_duplicate_insert) {
    CuckooHashTable ht;
    BOOST_CHECK(ht.insert(42, "first"));
    BOOST_CHECK(ht.search(42) == "first");

    BOOST_CHECK(!ht.insert(42, "second")); // дубликат — false
    BOOST_CHECK(ht.search(42) == "first"); // значение не изменилось
    BOOST_CHECK(ht.getSize() == 1);
}

BOOST_AUTO_TEST_CASE(test_remove) {
    CuckooHashTable ht;
    ht.insert(1, "one");
    ht.insert(2, "two");
    ht.insert(3, "three");

    BOOST_CHECK(ht.remove(2) == true);
    BOOST_CHECK(ht.search(2) == "");
    BOOST_CHECK(ht.getSize() == 2);

    BOOST_CHECK(ht.remove(999) == false);
    BOOST_CHECK(ht.getSize() == 2);
}

BOOST_AUTO_TEST_CASE(test_rehash) {
    CuckooHashTable ht(5); // маленькая, чтобы рехэш сработал быстро

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
    CuckooHashTable ht;
    ht.insert(10, "ten");
    ht.insert(20, "twenty");

    ht.clear();
    BOOST_CHECK(ht.isEmpty());
    BOOST_CHECK(ht.getSize() == 0);
    BOOST_CHECK(ht.search(10) == "");

    ht.insert(30, "thirty");
    BOOST_CHECK(ht.search(30) == "thirty");
}

BOOST_AUTO_TEST_CASE(test_print) {
    CuckooHashTable ht;
    ht.insert(10, "ten");
    ht.insert(20, "twenty");

    std::string output = captureOutput([&]() { ht.print(); });
    BOOST_CHECK(output.find("10 -> ten") != std::string::npos);
    BOOST_CHECK(output.find("20 -> twenty") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_text_file_io) {
    CuckooHashTable ht;
    ht.insert(100, "hundred");
    ht.insert(200, "two hundred");

    const std::string filename = "cuckoo_test.txt";
    ht.writeToFile(filename);

    CuckooHashTable ht2;
    ht2.readFromFile(filename);

    BOOST_CHECK(ht2.search(100) == "hundred");
    BOOST_CHECK(ht2.search(200) == "two hundred");
    BOOST_CHECK(ht2.getSize() == 2);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_binary_serialization) {
    CuckooHashTable original;
    original.insert(42, "answer");
    original.insert(123, "test value");
    original.insert(0, "");

    const std::string binfile = "cuckoo_bin.bin";
    BOOST_CHECK(original.serializeToBinary(binfile));

    CuckooHashTable loaded;
    BOOST_CHECK(loaded.deserializeFromBinary(binfile));

    BOOST_CHECK(loaded.search(42) == "answer");
    BOOST_CHECK(loaded.search(123) == "test value");
    BOOST_CHECK(loaded.search(0) == "");
    BOOST_CHECK(loaded.getSize() == 3);

    std::remove(binfile.c_str());
}

BOOST_AUTO_TEST_CASE(test_binary_empty) {
    CuckooHashTable empty;
    const std::string emptybin = "cuckoo_empty.bin";
    BOOST_CHECK(empty.serializeToBinary(emptybin));

    CuckooHashTable loaded;
    BOOST_CHECK(loaded.deserializeFromBinary(emptybin));
    BOOST_CHECK(loaded.isEmpty());

    std::remove(emptybin.c_str());
}




BOOST_AUTO_TEST_CASE(test_high_collision) {
    CuckooHashTable ht(11); // маленькая таблица для быстрых коллизий

    std::vector<int> successfully_inserted;
    // Ключи, которые дают много коллизий (все % 11 == 0 или близко)
    std::vector<int> collision_keys = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 110, 121, 132, 143, 154, 165, 176, 187, 198, 209};

    for (int key : collision_keys) {
        if (ht.insert(key, "collide" + std::to_string(key))) {
            successfully_inserted.push_back(key);
        }
    }

    // Размер совпадает с успешно вставленными
    BOOST_CHECK(static_cast<size_t>(ht.getSize()) == successfully_inserted.size());

    // Все успешно вставленные находятся
    for (int key : successfully_inserted) {
        BOOST_CHECK(ht.search(key) != "");
    }

    // Рехэш сработал (capacity выросла)
    BOOST_CHECK(ht.getCapacity() > 11);

    // Удаляем половину
    for (size_t i = 0; i < successfully_inserted.size() / 2; ++i) {
        ht.remove(successfully_inserted[i]);
    }

    // Оставшиеся всё ещё находятся
    for (size_t i = successfully_inserted.size() / 2; i < successfully_inserted.size(); ++i) {
        BOOST_CHECK(ht.search(successfully_inserted[i]) != "");
    }
}

BOOST_AUTO_TEST_CASE(test_rehash_behavior) {
    CuckooHashTable ht(11); // маленькая начальная capacity

    std::vector<int> inserted_keys;

    // Вставляем достаточно элементов, чтобы сработал рехэш (load factor >= 0.5 от 22 слотов = 11 элементов)
    for (int i = 1; i <= 15; ++i) { // 15 > 11, рехэш точно сработает
        int key = i * 10; // разные ключи, чтобы избежать дубликатов
        if (ht.insert(key, "value" + std::to_string(key))) {
            inserted_keys.push_back(key);
        }
    }

    int old_capacity = 11;
    BOOST_CHECK(ht.getCapacity() > old_capacity); // рехэш сработал, capacity выросла

    // Все успешно вставленные элементы всё ещё находятся после рехэша
    for (int key : inserted_keys) {
        BOOST_CHECK(ht.search(key) != "");
    }

    // Размер совпадает с количеством успешно вставленных
    BOOST_CHECK(static_cast<size_t>(ht.getSize()) == inserted_keys.size());

    // Дополнительно: вставляем ещё один — рехэш может сработать снова
    int extra_key = 999;
    ht.insert(extra_key, "extra");
    BOOST_CHECK(ht.search(extra_key) != "");
}
#endif