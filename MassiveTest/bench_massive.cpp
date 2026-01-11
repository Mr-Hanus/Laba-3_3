#ifdef BENCH_BUILD
#define BOOST_TEST_MODULE DynamicArrayBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <chrono>
#include "Massive.h"
#include <iostream>

BOOST_AUTO_TEST_CASE(benchmark_add_end) {
    Massive mas;
    const int OPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        mas.addEnd("value_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] addEnd (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    std::cout << "       Final size: " << mas.getSize() << ", capacity: " << mas.getCapacity() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_add_at_beginning) {
    Massive mas;
    const int OPS = 5000; // Меньше, потому что O(n) операция
    
    // Сначала добавляем несколько элементов
    for (int i = 0; i < 100; i++) {
        mas.addEnd("base_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        mas.addAt(0, "insert_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] addAt(0) (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_remove_from_end) {
    Massive mas;
    const int OPS = 50000;
    
    // Заполняем массив
    for (int i = 0; i < OPS; i++) {
        mas.addEnd("item_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS / 2; i++) {
        mas.removeAt(mas.getSize() - 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] removeAt(end) (" << OPS/2 << " ops): " << dur.count() << " ms" << std::endl;
    std::cout << "       Remaining size: " << mas.getSize() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_get_random_access) {
    Massive mas;
    const int SIZE = 100000;
    const int ACCESS_OPS = 100000;
    
    // Заполняем большой массив
    for (int i = 0; i < SIZE; i++) {
        mas.addEnd("data_" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ACCESS_OPS; i++) {
        // Доступ к случайным элементам
        int index = i % SIZE;
        std::string value = mas.get(index);
        (void)value; // Чтобы избежать warning
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "[BENCH] get(random) (" << ACCESS_OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_binary_serialization) {
    Massive mas;
    const int SIZE = 50000;
    
    // Заполняем массив
    for (int i = 0; i < SIZE; i++) {
        mas.addEnd("serialize_item_" + std::to_string(i));
    }
    
    // Бенчмарк сериализации
    auto start = std::chrono::high_resolution_clock::now();
    bool result = mas.serializeToBinary("bench_massive.bin");
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    BOOST_CHECK(result == true);
    std::cout << "[BENCH] Binary serialize (" << SIZE << " items): " << dur.count() << " ms" << std::endl;
    
    // Бенчмарк десериализации
    Massive mas2;
    start = std::chrono::high_resolution_clock::now();
    result = mas2.deserializeFromBinary("bench_massive.bin");
    end = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    BOOST_CHECK(result == true);
    std::cout << "[BENCH] Binary deserialize (" << SIZE << " items): " << dur.count() << " ms" << std::endl;
    
    std::remove("bench_massive.bin");
}

BOOST_AUTO_TEST_CASE(benchmark_capacity_growth) {
    Massive mas;
    const int OPS = 100000;
    
    std::cout << "[BENCH] Capacity growth during " << OPS << " addEnd operations:" << std::endl;
    std::cout << "  Initial capacity: " << mas.getCapacity() << std::endl;
    
    int resize_count = 0;
    int last_capacity = mas.getCapacity();
    
    for (int i = 0; i < OPS; i++) {
        mas.addEnd("test");
        if (mas.getCapacity() != last_capacity) {
            resize_count++;
            std::cout << "  Resize #" << resize_count << ": capacity " << last_capacity 
                      << " -> " << mas.getCapacity() << " (size=" << mas.getSize() << ")" << std::endl;
            last_capacity = mas.getCapacity();
        }
    }
    
    std::cout << "  Final: size=" << mas.getSize() << ", capacity=" << mas.getCapacity()
              << ", resizes=" << resize_count << std::endl;
    BOOST_CHECK(true);
}
#endif