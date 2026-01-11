#ifdef BENCH_BUILD
#define BOOST_TEST_MODULE CuckooBench
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/chrono.hpp>
#include "CuckooHash.h"
#include <random>
#include <vector>

BOOST_AUTO_TEST_CASE(bench_insert_random) {
    CuckooHashTable ht;
    const int OPS = 50000;

    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);

    std::vector<int> keys(OPS);
    for (int i = 0; i < OPS; ++i) keys[i] = dis(gen);

    auto start = boost::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; ++i) {
        ht.insert(keys[i], "val");
    }
    auto end = boost::chrono::high_resolution_clock::now();
    auto dur = boost::chrono::duration_cast<boost::chrono::microseconds>(end - start);

    std::cout << "[BENCH] Insert random (" << OPS << " ops): " << dur.count() / 1000.0 << " ms\n";
    std::cout << "       Final size: " << ht.getSize() << ", Capacity: " << ht.getCapacity() << "\n";
}

BOOST_AUTO_TEST_CASE(bench_search) {
    CuckooHashTable ht;
    const int SIZE = 50000;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);

    for (int i = 0; i < SIZE; ++i) {
        int k = dis(gen);
        ht.insert(k, "val");
    }

    std::vector<int> search_keys(SIZE);
    for (int i = 0; i < SIZE; ++i) search_keys[i] = dis(gen);

    auto start = boost::chrono::high_resolution_clock::now();
    for (int i = 0; i < SIZE; ++i) {
        ht.search(search_keys[i]);
    }
    auto end = boost::chrono::high_resolution_clock::now();
    auto dur = boost::chrono::duration_cast<boost::chrono::microseconds>(end - start);

    std::cout << "[BENCH] Search random (" << SIZE << " ops): " << dur.count() / 1000.0 << " ms\n";
}

BOOST_AUTO_TEST_CASE(bench_remove) {
    CuckooHashTable ht;
    const int SIZE = 50000;
    std::vector<int> keys(SIZE);

    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);
    for (int i = 0; i < SIZE; ++i) {
        keys[i] = dis(gen);
        ht.insert(keys[i], "val");
    }

    auto start = boost::chrono::high_resolution_clock::now();
    for (int i = 0; i < SIZE / 2; ++i) {
        ht.remove(keys[i]);
    }
    auto end = boost::chrono::high_resolution_clock::now();
    auto dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(end - start);

    std::cout << "[BENCH] Remove random (" << SIZE / 2 << " ops): " << dur.count() << " ms\n";
    std::cout << "       Remaining size: " << ht.getSize() << "\n";
}

BOOST_AUTO_TEST_CASE(bench_mixed) {
    CuckooHashTable ht;
    const int OPS = 100000;
    std::mt19937 gen(123);
    std::uniform_int_distribution<> op_dis(0, 2);
    std::uniform_int_distribution<> key_dis(1, 10000);

    auto start = boost::chrono::high_resolution_clock::now();
    for (int i = 0; i < OPS; ++i) {
        int op = op_dis(gen);
        int key = key_dis(gen);
        if (op == 0) ht.insert(key, "val");
        else if (op == 1) ht.search(key);
        else ht.remove(key);
    }
    auto end = boost::chrono::high_resolution_clock::now();
    auto dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(end - start);

    std::cout << "[BENCH] Mixed operations (" << OPS << " ops): " << dur.count() << " ms\n";
    std::cout << "       Final size: " << ht.getSize() << "\n";
}
#endif