#ifdef STACKTEST
#define BOOST_TEST_MODULE StackTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Stack.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(test_constructor_destructor) {
    Stack stack1;
    BOOST_CHECK(stack1.getSize() == 0);
    BOOST_CHECK(stack1.getCapacity() >= 4);
    BOOST_CHECK(stack1.isEmpty() == true);
    BOOST_CHECK(stack1.checkIntegrity() == true);
    
    Stack stack2(50);
    BOOST_CHECK(stack2.getCapacity() >= 50);
    BOOST_CHECK(stack2.getSize() == 0);
}

BOOST_AUTO_TEST_CASE(test_push_pop) {
    Stack stack;
    
    stack.push("first");
    BOOST_CHECK(stack.getSize() == 1);
    BOOST_CHECK(stack.isEmpty() == false);
    
    stack.push("second");
    BOOST_CHECK(stack.getSize() == 2);
    
    std::string val = stack.pop();
    BOOST_CHECK(val == "second");
    BOOST_CHECK(stack.getSize() == 1);
    
    val = stack.pop();
    BOOST_CHECK(val == "first");
    BOOST_CHECK(stack.getSize() == 0);
    BOOST_CHECK(stack.isEmpty() == true);
}

BOOST_AUTO_TEST_CASE(test_pop_empty) {
    Stack stack;
    BOOST_CHECK_THROW(stack.pop(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_top) {
    Stack stack;
    
    stack.push("A");
    BOOST_CHECK(stack.top() == "A");
    
    stack.push("B");
    BOOST_CHECK(stack.top() == "B");
    
    stack.pop();
    BOOST_CHECK(stack.top() == "A");
}

BOOST_AUTO_TEST_CASE(test_top_empty) {
    Stack stack;
    BOOST_CHECK_THROW(stack.top(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_capacity_growth) {
    Stack stack(5);
    int initial_capacity = stack.getCapacity();
    
    // Заполняем больше начальной capacity
    for (int i = 0; i < 20; i++) {
        stack.push("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(stack.getSize() == 20);
    BOOST_CHECK(stack.getCapacity() > initial_capacity);
    BOOST_CHECK(stack.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(test_capacity_shrink) {
    Stack stack;
    
    // Заполняем много
    for (int i = 0; i < 100; i++) {
        stack.push("item_" + std::to_string(i));
    }
    
    int full_capacity = stack.getCapacity();
    
    // Удаляем много
    for (int i = 0; i < 80; i++) {
        stack.pop();
    }
    
    BOOST_CHECK(stack.getCapacity() < full_capacity);
    BOOST_CHECK(stack.getCapacity() >= 4); // MIN_CAPACITY
    BOOST_CHECK(stack.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(test_clear) {
    Stack stack;
    
    for (int i = 0; i < 10; i++) {
        stack.push("item_" + std::to_string(i));
    }
    
    stack.clear();
    BOOST_CHECK(stack.getSize() == 0);
    BOOST_CHECK(stack.isEmpty() == true);
    BOOST_CHECK(stack.getCapacity() >= 4);
    BOOST_CHECK(stack.checkIntegrity() == true);
    
    // После clear можно снова использовать
    stack.push("new");
    BOOST_CHECK(stack.getSize() == 1);
    BOOST_CHECK(stack.top() == "new");
}

BOOST_AUTO_TEST_CASE(test_file_operations) {
    Stack stack1;
    stack1.push("File");
    stack1.push("Test");
    stack1.push("Data");
    
    stack1.writeToFile("test_stack.txt");
    
    Stack stack2;
    stack2.readFromFile("test_stack.txt");
    
    BOOST_CHECK(stack2.getSize() == 3);
    BOOST_CHECK(stack2.pop() == "Data");
    BOOST_CHECK(stack2.pop() == "Test");
    BOOST_CHECK(stack2.pop() == "File");
    
    std::remove("test_stack.txt");
    
    Stack stack3;
    stack3.readFromFile("nonexistent.txt");
    BOOST_CHECK(stack3.getSize() == 0);
}

BOOST_AUTO_TEST_CASE(test_binary_serialization) {
    Stack original;
    original.push("first");
    original.push("second");
    original.push("third");
    
    BOOST_CHECK(original.serializeToBinary("test_bin.bin") == true);
    
    Stack loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("test_bin.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 3);
    BOOST_CHECK(loaded.pop() == "third");
    BOOST_CHECK(loaded.pop() == "second");
    BOOST_CHECK(loaded.pop() == "first");
    
    std::remove("test_bin.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_empty) {
    Stack original;
    BOOST_CHECK(original.serializeToBinary("empty.bin") == true);
    
    Stack loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("empty.bin") == true);
    BOOST_CHECK(loaded.getSize() == 0);
    
    std::remove("empty.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_large) {
    Stack original;
    for (int i = 0; i < 1000; i++) {
        original.push("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(original.serializeToBinary("large.bin") == true);
    
    Stack loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("large.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 1000);
    for (int i = 999; i >= 0; i--) {
        BOOST_CHECK(loaded.pop() == "item_" + std::to_string(i));
    }
    
    std::remove("large.bin");
}

BOOST_AUTO_TEST_CASE(test_print) {
    Stack stack;
    stack.push("A");
    stack.push("B");
    stack.push("C");
    
    // Проверяем что print не падает
    stack.print();
    
    Stack empty;
    empty.print();
    
    BOOST_CHECK(true);
}
#endif