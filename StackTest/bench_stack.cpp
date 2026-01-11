#ifdef STACKBENCH
#define BOOST_TEST_MODULE StackBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <chrono>
#include "Stack.h"
#include <iostream>

BOOST_AUTO_TEST_CASE(benchmark_push) {
    Stack stack;
    const int OPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        stack.push("value_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] push (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    std::cout << "       Final size: " << stack.getSize() 
              << ", capacity: " << stack.getCapacity() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_push_pop_alternating) {
    Stack stack;
    const int OPS = 50000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        stack.push("val_" + std::to_string(i));
        if (i % 2 == 0) {
            stack.pop();
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] push/pop alternating (" << OPS << " ops): " 
              << dur.count() << " ms" << std::endl;
    std::cout << "       Remaining size: " << stack.getSize() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_push_then_pop_all) {
    Stack stack;
    const int OPS = 100000;
    
    // Push всех
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        stack.push("to_pop_" + std::to_string(i));
    }
    auto mid = std::chrono::high_resolution_clock::now();
    
    // Pop всех
    while (!stack.isEmpty()) {
        stack.pop();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto push_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto pop_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] push then pop all (" << OPS << " items):" << std::endl;
    std::cout << "  push: " << push_time.count() << " ms" << std::endl;
    std::cout << "  pop:  " << pop_time.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_top) {
    Stack stack;
    const int OPS = 1000000; // Много, т.к. операция быстрая
    
    // Заполняем
    for (int i = 0; i < 100; i++) {
        stack.push("base");
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        std::string val = stack.top();
        (void)val; // избегаем warning
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "[BENCH] top (" << OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_capacity_growth_pattern) {
    Stack stack;
    const int OPS = 100000;
    
    std::cout << "[BENCH] Capacity growth during " << OPS << " push operations:" << std::endl;
    std::cout << "  Initial capacity: " << stack.getCapacity() << std::endl;
    
    int resize_count = 0;
    int last_capacity = stack.getCapacity();
    
    for (int i = 0; i < OPS; i++) {
        stack.push("growth_test");
        if (stack.getCapacity() != last_capacity) {
            resize_count++;
            std::cout << "  Resize #" << resize_count << ": capacity " << last_capacity 
                      << " -> " << stack.getCapacity() << " (size=" << stack.getSize() << ")" << std::endl;
            last_capacity = stack.getCapacity();
        }
    }
    
    std::cout << "  Final: size=" << stack.getSize() << ", capacity=" << stack.getCapacity() 
              << ", resizes=" << resize_count << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_capacity_shrink_pattern) {
    Stack stack;
    const int INITIAL_OPS = 100000;
    const int REMOVE_OPS = 90000;
    
    // Заполняем
    for (int i = 0; i < INITIAL_OPS; i++) {
        stack.push("shrink_test_" + std::to_string(i));
    }
    
    std::cout << "[BENCH] Capacity shrink during " << REMOVE_OPS << " pop operations:" << std::endl;
    std::cout << "  Initial: size=" << stack.getSize() << ", capacity=" << stack.getCapacity() << std::endl;
    
    int shrink_count = 0;
    int last_capacity = stack.getCapacity();
    
    for (int i = 0; i < REMOVE_OPS; i++) {
        stack.pop();
        if (stack.getCapacity() != last_capacity) {
            shrink_count++;
            std::cout << "  Shrink #" << shrink_count << ": capacity " << last_capacity 
                      << " -> " << stack.getCapacity() << " (size=" << stack.getSize() << ")" << std::endl;
            last_capacity = stack.getCapacity();
        }
    }
    
    std::cout << "  Final: size=" << stack.getSize() << ", capacity=" << stack.getCapacity() 
              << ", shrinks=" << shrink_count << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_binary_serialization) {
    Stack stack;
    const int SIZE = 50000;
    
    // Заполняем
    for (int i = 0; i < SIZE; i++) {
        stack.push("serial_item_" + std::to_string(i));
    }
    
    // Сериализация
    auto start = std::chrono::high_resolution_clock::now();
    bool result = stack.serializeToBinary("bench_stack.bin");
    auto mid = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto serialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    
    // Десериализация
    Stack stack2;
    result = stack2.deserializeFromBinary("bench_stack.bin");
    auto end = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto deserialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] Binary serialization Stack:" << std::endl;
    std::cout << "  serialize (" << SIZE << " items): " << serialize_time.count() << " ms" << std::endl;
    std::cout << "  deserialize (" << SIZE << " items): " << deserialize_time.count() << " ms" << std::endl;
    
    // Проверяем целостность
    BOOST_CHECK(stack2.getSize() == SIZE);
    for (int i = SIZE - 1; i >= 0; i--) {
        BOOST_CHECK(stack2.pop() == "serial_item_" + std::to_string(i));
    }
    
    std::remove("bench_stack.bin");
}

BOOST_AUTO_TEST_CASE(benchmark_stress_test) {
    Stack stack;
    const int CYCLES = 1000;
    const int OPS_PER_CYCLE = 100;
    
    std::cout << "[BENCH] Stress test (" << CYCLES << " cycles of " << OPS_PER_CYCLE << " push/pop):" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int cycle = 0; cycle < CYCLES; cycle++) {
        // Push серия
        for (int i = 0; i < OPS_PER_CYCLE; i++) {
            stack.push("stress_" + std::to_string(cycle) + "_" + std::to_string(i));
        }
        
        // Pop серия
        for (int i = 0; i < OPS_PER_CYCLE; i++) {
            stack.pop();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Total time: " << dur.count() << " ms" << std::endl;
    std::cout << "  Final size: " << stack.getSize() << " (should be 0)" << std::endl;
    std::cout << "  Final capacity: " << stack.getCapacity() << std::endl;
    
    BOOST_CHECK(stack.isEmpty() == true);
    BOOST_CHECK(stack.checkIntegrity() == true);
}
#endif