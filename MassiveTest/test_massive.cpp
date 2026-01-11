#ifdef TEST_BUILD
#define BOOST_TEST_MODULE DynamicArrayTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Massive.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(test_constructor_destructor) {
    Massive mas1;
    BOOST_CHECK(mas1.getSize() == 0);
    BOOST_CHECK(mas1.getCapacity() >= 10);
    BOOST_CHECK(mas1.checkIntegrity() == true);
    
    Massive mas2(5);
    BOOST_CHECK(mas2.getCapacity() >= 10); // MIN_CAPACITY
    
    Massive mas3(50);
    BOOST_CHECK(mas3.getCapacity() == 50);
}

BOOST_AUTO_TEST_CASE(test_add_end) {
    Massive mas;
    
    mas.addEnd("first");
    BOOST_CHECK(mas.getSize() == 1);
    BOOST_CHECK(mas.get(0) == "first");
    
    mas.addEnd("second");
    BOOST_CHECK(mas.getSize() == 2);
    BOOST_CHECK(mas.get(0) == "first");
    BOOST_CHECK(mas.get(1) == "second");
    
    // Проверка расширения capacity
    for (int i = 0; i < 20; i++) {
        mas.addEnd("item_" + std::to_string(i));
    }
    BOOST_CHECK(mas.getSize() == 22);
    BOOST_CHECK(mas.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(test_add_at) {
    Massive mas;
    
    BOOST_CHECK(mas.addAt(0, "first") == true);
    BOOST_CHECK(mas.getSize() == 1);
    BOOST_CHECK(mas.get(0) == "first");
    
    BOOST_CHECK(mas.addAt(0, "new_first") == true);
    BOOST_CHECK(mas.getSize() == 2);
    BOOST_CHECK(mas.get(0) == "new_first");
    BOOST_CHECK(mas.get(1) == "first");
    
    BOOST_CHECK(mas.addAt(2, "last") == true);
    BOOST_CHECK(mas.get(2) == "last");
    
    BOOST_CHECK(mas.addAt(5, "invalid") == false);
    BOOST_CHECK(mas.addAt(-1, "invalid") == false);
}

BOOST_AUTO_TEST_CASE(test_get_set) {
    Massive mas;
    mas.addEnd("A");
    mas.addEnd("B");
    mas.addEnd("C");
    
    BOOST_CHECK(mas.get(0) == "A");
    BOOST_CHECK(mas.get(1) == "B");
    BOOST_CHECK(mas.get(2) == "C");
    
    BOOST_CHECK_THROW(mas.get(-1), std::out_of_range);
    BOOST_CHECK_THROW(mas.get(3), std::out_of_range);
    
    BOOST_CHECK(mas.set(1, "X") == true);
    BOOST_CHECK(mas.get(1) == "X");
    
    BOOST_CHECK(mas.set(-1, "Y") == false);
    BOOST_CHECK(mas.set(3, "Y") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_at) {
    Massive mas;
    mas.addEnd("A");
    mas.addEnd("B");
    mas.addEnd("C");
    mas.addEnd("D");
    
    BOOST_CHECK(mas.removeAt(1) == true);
    BOOST_CHECK(mas.getSize() == 3);
    BOOST_CHECK(mas.get(0) == "A");
    BOOST_CHECK(mas.get(1) == "C");
    BOOST_CHECK(mas.get(2) == "D");
    
    BOOST_CHECK(mas.removeAt(0) == true);
    BOOST_CHECK(mas.getSize() == 2);
    BOOST_CHECK(mas.get(0) == "C");
    
    BOOST_CHECK(mas.removeAt(5) == false);
    BOOST_CHECK(mas.removeAt(-1) == false);
    
    // Проверка shrink
    for (int i = 0; i < 100; i++) mas.addEnd("temp");
    int cap_before = mas.getCapacity();
    for (int i = 0; i < 90; i++) mas.removeAt(0);
    BOOST_CHECK(mas.getCapacity() < cap_before);
}

BOOST_AUTO_TEST_CASE(test_clear) {
    Massive mas;
    for (int i = 0; i < 50; i++) mas.addEnd("item");
    
    mas.clear();
    BOOST_CHECK(mas.getSize() == 0);
    BOOST_CHECK(mas.getCapacity() >= 10);
    BOOST_CHECK(mas.checkIntegrity() == true);
    
    // После clear можно снова добавлять
    mas.addEnd("new");
    BOOST_CHECK(mas.getSize() == 1);
    BOOST_CHECK(mas.get(0) == "new");
}

BOOST_AUTO_TEST_CASE(test_file_operations) {
    Massive mas1;
    mas1.addEnd("File");
    mas1.addEnd("Test");
    mas1.addEnd("Data");
    
    mas1.writeToFile("test_massive.txt");
    
    Massive mas2;
    mas2.readFromFile("test_massive.txt");
    
    BOOST_CHECK(mas2.getSize() == 3);
    BOOST_CHECK(mas2.get(0) == "File");
    BOOST_CHECK(mas2.get(1) == "Test");
    BOOST_CHECK(mas2.get(2) == "Data");
    
    std::remove("test_massive.txt");
    
    Massive mas3;
    mas3.readFromFile("nonexistent.txt");
    BOOST_CHECK(mas3.getSize() == 0);
}

BOOST_AUTO_TEST_CASE(test_binary_serialization) {
    Massive original;
    original.addEnd("first");
    original.addEnd("second");
    original.addEnd("third");
    
    BOOST_CHECK(original.serializeToBinary("test_bin.bin") == true);
    
    Massive loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("test_bin.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 3);
    BOOST_CHECK(loaded.get(0) == "first");
    BOOST_CHECK(loaded.get(1) == "second");
    BOOST_CHECK(loaded.get(2) == "third");
    
    std::remove("test_bin.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_empty) {
    Massive original;
    BOOST_CHECK(original.serializeToBinary("empty.bin") == true);
    
    Massive loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("empty.bin") == true);
    BOOST_CHECK(loaded.getSize() == 0);
    
    std::remove("empty.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_large) {
    Massive original;
    for (int i = 0; i < 1000; i++) {
        original.addEnd("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(original.serializeToBinary("large.bin") == true);
    
    Massive loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("large.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 1000);
    for (int i = 0; i < 1000; i++) {
        BOOST_CHECK(loaded.get(i) == "item_" + std::to_string(i));
    }
    
    std::remove("large.bin");
}
#endif