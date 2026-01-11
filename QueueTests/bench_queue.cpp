#ifdef BUILD_BENCH
#define BOOST_TEST_MODULE QueueBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <chrono>
#include "Queue.h"
#include <iostream>

BOOST_AUTO_TEST_CASE(benchmark_enqueue) {
    Queue q;
    const int OPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        q.enqueue("value_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] enqueue (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    std::cout << "       Final size: " << q.getSize() 
              << ", capacity: " << q.getCapacity() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_enqueue_dequeue_alternating) {
    Queue q;
    const int OPS = 50000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        q.enqueue("val_" + std::to_string(i));
        if (i % 2 == 0) {
            q.dequeue();
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] enqueue/dequeue alternating (" << OPS << " ops): " 
              << dur.count() << " ms" << std::endl;
    std::cout << "       Remaining size: " << q.getSize() << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_enqueue_then_dequeue_all) {
    Queue q;
    const int OPS = 100000;
    
    // Enqueue всех
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        q.enqueue("to_dequeue_" + std::to_string(i));
    }
    auto mid = std::chrono::high_resolution_clock::now();
    
    // Dequeue всех
    while (!q.isEmpty()) {
        q.dequeue();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto enqueue_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto dequeue_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] enqueue then dequeue all (" << OPS << " items):" << std::endl;
    std::cout << "  enqueue: " << enqueue_time.count() << " ms" << std::endl;
    std::cout << "  dequeue: " << dequeue_time.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_peek) {
    Queue q;
    const int OPS = 1000000; // Много, т.к. операция быстрая
    
    // Заполняем
    q.enqueue("base");
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; i++) {
        std::string val = q.peek();
        (void)val; // избегаем warning
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "[BENCH] peek (" << OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_circular_buffer_performance) {
    Queue q(100);  // Маленький буфер
    
    const int OPS = 100000;
    
    std::cout << "[BENCH] Circular buffer test (" << OPS << " enqueue/dequeue pairs):" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < OPS; i++) {
        q.enqueue("circular_" + std::to_string(i));
        q.dequeue();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Time: " << dur.count() << " ms" << std::endl;
    std::cout << "  Final size: " << q.getSize() << " (should be 0)" << std::endl;
    std::cout << "  Capacity resizes: " << "see below" << std::endl;
    
    // Дополнительный тест с частичным заполнением
    Queue q2(10);
    int resize_count = 0;
    int last_capacity = q2.getCapacity();
    
    for (int i = 0; i < 1000; i++) {
        q2.enqueue("test");
        if (q2.getCapacity() != last_capacity) {
            resize_count++;
            last_capacity = q2.getCapacity();
        }
        if (i % 3 == 0) {
            q2.dequeue();
        }
    }
    
    std::cout << "  With partial dequeue: " << resize_count << " resizes" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_binary_serialization) {
    Queue q;
    const int SIZE = 50000;
    
    // Заполняем
    for (int i = 0; i < SIZE; i++) {
        q.enqueue("serial_item_" + std::to_string(i));
    }
    
    // Сериализация
    auto start = std::chrono::high_resolution_clock::now();
    bool result = q.serializeToBinary("bench_queue.bin");
    auto mid = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto serialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    
    // Десериализация
    Queue q2;
    result = q2.deserializeFromBinary("bench_queue.bin");
    auto end = std::chrono::high_resolution_clock::now();
    
    BOOST_CHECK(result == true);
    auto deserialize_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "[BENCH] Binary serialization Queue:" << std::endl;
    std::cout << "  serialize (" << SIZE << " items): " << serialize_time.count() << " ms" << std::endl;
    std::cout << "  deserialize (" << SIZE << " items): " << deserialize_time.count() << " ms" << std::endl;
    
    // Проверяем целостность
    BOOST_CHECK(q2.getSize() == SIZE);
    for (int i = 0; i < SIZE; i++) {
        BOOST_CHECK(q2.dequeue() == "serial_item_" + std::to_string(i));
    }
    
    std::remove("bench_queue.bin");
}

BOOST_AUTO_TEST_CASE(benchmark_stress_test_with_circular_wrap) {
    Queue q(50);  // Маленькая capacity для частого вращения
    
    const int CYCLES = 1000;
    const int OPS_PER_CYCLE = 40;
    
    std::cout << "[BENCH] Stress test with circular wrap (" 
              << CYCLES << " cycles of " << OPS_PER_CYCLE << " ops):" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int cycle = 0; cycle < CYCLES; cycle++) {
        // Enqueue серия
        for (int i = 0; i < OPS_PER_CYCLE; i++) {
            q.enqueue("stress_" + std::to_string(cycle) + "_" + std::to_string(i));
        }
        
        // Dequeue серия
        for (int i = 0; i < OPS_PER_CYCLE; i++) {
            q.dequeue();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Total time: " << dur.count() << " ms" << std::endl;
    std::cout << "  Final size: " << q.getSize() << " (should be 0)" << std::endl;
    std::cout << "  Final capacity: " << q.getCapacity() << std::endl;
    
    BOOST_CHECK(q.isEmpty() == true);
    BOOST_CHECK(q.checkIntegrity() == true);
}

BOOST_AUTO_TEST_CASE(benchmark_queue_vs_stack_scenario) {
    Queue queue;
    const int OPS = 50000;
    
    // Тестируем типичный сценарий работы очереди
    // (более реалистичный чем чистые enqueue/dequeue)
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Фаза 1: Быстрое наполнение
    for (int i = 0; i < OPS * 0.7; i++) {
        queue.enqueue("job_" + std::to_string(i));
    }
    
    // Фаза 2: Одновременно добавляем и удаляем
    for (int i = 0; i < OPS * 0.3; i++) {
        queue.enqueue("new_job_" + std::to_string(i));
        queue.dequeue();
    }
    
    // Фаза 3: Очистка оставшегося
    while (!queue.isEmpty()) {
        queue.dequeue();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[BENCH] Realistic queue scenario (" << OPS << " total ops): " 
              << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}
#endif