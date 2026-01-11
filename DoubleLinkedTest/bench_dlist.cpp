#ifdef BENCH_BUILD
#define BOOST_TEST_MODULE DoubleLinkedListBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <chrono>
#include "DoubleLinked.h"
#include <iostream>

BOOST_AUTO_TEST_CASE(benchmark_add_head) {
    DoubleLinkedList list;
    const int OPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        list.addHead("val_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] addHead (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_add_tail) {
    DoubleLinkedList list;
    const int OPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        list.addTail("val_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] addTail (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_add_tail_vs_head) {
    DoubleLinkedList list1, list2;
    const int OPS = 50000;
    
    // addTail
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) list1.addTail("tail_" + std::to_string(i));
    auto mid = std::chrono::high_resolution_clock::now();
    
    // addHead  
    for (int i = 0; i < OPS; i++) list2.addHead("head_" + std::to_string(i));
    auto end = std::chrono::high_resolution_clock::now();
    
    auto tail_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto head_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] Сравнение addTail vs addHead (" << OPS << " ops):" << std::endl;
    std::cout << "  addTail: " << tail_time.count() << " ms" << std::endl;
    std::cout << "  addHead: " << head_time.count() << " ms" << std::endl;
    std::cout << "  Отношение: " << (double)tail_time.count() / head_time.count() << "x" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_find_middle) {
    DoubleLinkedList list;
    const int LIST_SIZE = 10000;
    const int SEARCH_OPS = 1000;
    
    // Заполняем список
    for (int i = 0; i < LIST_SIZE; i++) {
        list.addTail("item_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_OPS; i++) {
        list.findValue("item_" + std::to_string(LIST_SIZE / 2));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "[BENCH] findValue (середина, " << SEARCH_OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_find_nonexistent) {
    DoubleLinkedList list;
    const int LIST_SIZE = 10000;
    const int SEARCH_OPS = 1000;
    
    for (int i = 0; i < LIST_SIZE; i++) {
        list.addTail("item_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < SEARCH_OPS; i++) {
        list.findValue("NON_EXISTENT_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "[BENCH] findValue (несуществующий, " << SEARCH_OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_remove_head) {
    DoubleLinkedList list;
    const int OPS = 50000;
    
    // Заполняем
    for (int i = 0; i < OPS; i++) {
        list.addHead("to_remove_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    while (list.getHead()) {
        list.removeHead();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] removeHead (clear " << OPS << " items): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_remove_tail) {
    DoubleLinkedList list;
    const int OPS = 50000;
    
    for (int i = 0; i < OPS; i++) {
        list.addTail("to_remove_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    while (list.getTail()) {
        list.removeTail();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] removeTail (clear " << OPS << " items): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_add_remove_middle) {
    DoubleLinkedList list;
    const int BASE_SIZE = 1000;
    const int OPS = 5000;
    
    // Базовый список
    for (int i = 0; i < BASE_SIZE; i++) {
        list.addTail("base_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        // Добавляем в середину
        list.addAfter("base_500", "mid_add_" + std::to_string(i));
        // Удаляем из середины
        list.removeValue("mid_add_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] addAfter+removeValue в середине (" << OPS << " циклов): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_binary_serialization) {
    DoubleLinkedList list;
    const int SIZE = 50000;
    
    // Заполняем
    for (int i = 0; i < SIZE; i++) {
        list.addTail("serial_item_" + std::to_string(i));
    }
    
    // Сериализация
    auto start = std::chrono::high_resolution_clock::now();
    bool result = list.serializeToBinary("bench_dlist.bin");
    auto mid = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto serialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    
    // Десериализация
    DoubleLinkedList list2;
    result = list2.deserializeFromBinary("bench_dlist.bin");
    auto end = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto deserialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] Бинарная сериализация DoubleLinkedList:" << std::endl;
    std::cout << "  serialize (" << SIZE << " items): " << serialize_time.count() << " ms" << std::endl;
    std::cout << "  deserialize (" << SIZE << " items): " << deserialize_time.count() << " ms" << std::endl;
    
    std::remove("bench_dlist.bin");
}

BOOST_AUTO_TEST_CASE(benchmark_traversal_forward_vs_backward) {
    DoubleLinkedList list;
    const int SIZE = 100000;
    
    // Заполняем большой список
    for (int i = 0; i < SIZE; i++) {
        list.addTail("traverse_" + std::to_string(i));
    }
    
    // Прямой обход (head → tail)
    auto start = std::chrono::high_resolution_clock::now();
    DNode* cur = list.getHead();
    int count_forward = 0;
    while (cur) {
        cur = cur->getNext();
        count_forward++;
    }
    auto mid = std::chrono::high_resolution_clock::now();
    
    // Обратный обход (tail → head)
    cur = list.getTail();
    int count_backward = 0;
    while (cur) {
        cur = cur->getPrev();
        count_backward++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto forward_time = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
    auto backward_time = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);
    
    std::cout << "[BENCH] Обход DoubleLinkedList:" << std::endl;
    std::cout << "  Прямой (head→tail): " << forward_time.count() << " µs, элементов: " << count_forward << std::endl;
    std::cout << "  Обратный (tail→head): " << backward_time.count() << " µs, элементов: " << count_backward << std::endl;
    BOOST_CHECK(count_forward == SIZE && count_backward == SIZE);
}
#endif