#ifdef BUILD_TEST
#define BOOST_TEST_MODULE QueueTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Queue.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(test_constructor_destructor) {
    Queue q1;
    BOOST_CHECK(q1.getSize() == 0);
    BOOST_CHECK(q1.getCapacity() >= 4);
    BOOST_CHECK(q1.isEmpty() == true);
    BOOST_CHECK(q1.checkIntegrity() == true);
    
    Queue q2(50);
    BOOST_CHECK(q2.getCapacity() >= 50);
    BOOST_CHECK(q2.getSize() == 0);
}

BOOST_AUTO_TEST_CASE(test_enqueue_dequeue) {
    Queue q;
    
    q.enqueue("first");
    BOOST_CHECK(q.getSize() == 1);
    BOOST_CHECK(q.isEmpty() == false);
    
    q.enqueue("second");
    BOOST_CHECK(q.getSize() == 2);
    
    std::string val = q.dequeue();
    BOOST_CHECK(val == "first");
    BOOST_CHECK(q.getSize() == 1);
    
    val = q.dequeue();
    BOOST_CHECK(val == "second");
    BOOST_CHECK(q.getSize() == 0);
    BOOST_CHECK(q.isEmpty() == true);
}

BOOST_AUTO_TEST_CASE(test_dequeue_empty) {
    Queue q;
    BOOST_CHECK_THROW(q.dequeue(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_peek) {
    Queue q;
    
    q.enqueue("A");
    BOOST_CHECK(q.peek() == "A");
    
    q.enqueue("B");
    BOOST_CHECK(q.peek() == "A");  // Front остаётся "A"
    
    q.dequeue();
    BOOST_CHECK(q.peek() == "B");
}

BOOST_AUTO_TEST_CASE(test_peek_empty) {
    Queue q;
    BOOST_CHECK_THROW(q.peek(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_circular_buffer) {
    Queue q(4);  // Маленькая capacity для теста
    
    // Заполняем полностью
    q.enqueue("A");
    q.enqueue("B");
    q.enqueue("C");
    q.enqueue("D");  // capacity=4, size=4
    
    // Удаляем два элемента
    BOOST_CHECK(q.dequeue() == "A");  // front сдвигается
    BOOST_CHECK(q.dequeue() == "B");
    
    // Добавляем ещё - должен использовать начало массива
    q.enqueue("E");
    q.enqueue("F");
    
    BOOST_CHECK(q.getSize() == 4);
    BOOST_CHECK(q.checkIntegrity() == true);
    
    // Проверяем порядок
    BOOST_CHECK(q.dequeue() == "C");
    BOOST_CHECK(q.dequeue() == "D");
    BOOST_CHECK(q.dequeue() == "E");
    BOOST_CHECK(q.dequeue() == "F");
}

BOOST_AUTO_TEST_CASE(test_capacity_growth) {
    Queue q(5);
    int initial_capacity = q.getCapacity();
    
    // Заполняем больше начальной capacity
    for (int i = 0; i < 20; i++) {
        q.enqueue("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(q.getSize() == 20);
    BOOST_CHECK(q.getCapacity() > initial_capacity);
    BOOST_CHECK(q.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(test_capacity_shrink) {
    Queue q;
    
    // Заполняем много
    for (int i = 0; i < 100; i++) {
        q.enqueue("item_" + std::to_string(i));
    }
    
    int full_capacity = q.getCapacity();
    
    // Удаляем много
    for (int i = 0; i < 80; i++) {
        q.dequeue();
    }
    
    BOOST_CHECK(q.getCapacity() < full_capacity);
    BOOST_CHECK(q.getCapacity() >= 4); // MIN_CAPACITY
    BOOST_CHECK(q.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(test_clear) {
    Queue q;
    
    for (int i = 0; i < 10; i++) {
        q.enqueue("item_" + std::to_string(i));
    }
    
    q.clear();
    BOOST_CHECK(q.getSize() == 0);
    BOOST_CHECK(q.isEmpty() == true);
    BOOST_CHECK(q.getCapacity() >= 4);
    BOOST_CHECK(q.checkIntegrity() == true);
    
    // После clear можно снова использовать
    q.enqueue("new");
    BOOST_CHECK(q.getSize() == 1);
    BOOST_CHECK(q.peek() == "new");
}

BOOST_AUTO_TEST_CASE(test_file_operations) {
    Queue q1;
    q1.enqueue("File");
    q1.enqueue("Test");
    q1.enqueue("Data");
    
    q1.writeToFile("test_queue.txt");
    
    Queue q2;
    q2.readFromFile("test_queue.txt");
    
    BOOST_CHECK(q2.getSize() == 3);
    BOOST_CHECK(q2.dequeue() == "File");
    BOOST_CHECK(q2.dequeue() == "Test");
    BOOST_CHECK(q2.dequeue() == "Data");
    
    std::remove("test_queue.txt");
    
    Queue q3;
    q3.readFromFile("nonexistent.txt");
    BOOST_CHECK(q3.getSize() == 0);
}

BOOST_AUTO_TEST_CASE(test_binary_serialization) {
    Queue original;
    original.enqueue("first");
    original.enqueue("second");
    original.enqueue("third");
    
    BOOST_CHECK(original.serializeToBinary("test_bin.bin") == true);
    
    Queue loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("test_bin.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 3);
    BOOST_CHECK(loaded.dequeue() == "first");
    BOOST_CHECK(loaded.dequeue() == "second");
    BOOST_CHECK(loaded.dequeue() == "third");
    
    std::remove("test_bin.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_empty) {
    Queue original;
    BOOST_CHECK(original.serializeToBinary("empty.bin") == true);
    
    Queue loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("empty.bin") == true);
    BOOST_CHECK(loaded.getSize() == 0);
    
    std::remove("empty.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_large) {
    Queue original;
    for (int i = 0; i < 1000; i++) {
        original.enqueue("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(original.serializeToBinary("large.bin") == true);
    
    Queue loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("large.bin") == true);
    
    BOOST_CHECK(loaded.getSize() == 1000);
    for (int i = 0; i < 1000; i++) {
        BOOST_CHECK(loaded.dequeue() == "item_" + std::to_string(i));
    }
    
    std::remove("large.bin");
}
#endif