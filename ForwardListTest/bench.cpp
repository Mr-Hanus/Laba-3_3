#ifdef BUILD_BENCHMARKS
#define BOOST_TEST_MODULE ForwardListBenchmark
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/chrono.hpp>
#include "ForwardList.h"
#include <iostream>
#include <chrono>

BOOST_AUTO_TEST_CASE(benchmark_add_head) {
    ForwardList list;
    const int OPS = 10000;

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < OPS; ++i) {
        list.addHead("val_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[BENCH] addHead (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_add_tail) {
    ForwardList list;
    const int OPS = 10000;

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < OPS; ++i) {
        list.addTail("val_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[BENCH] addTail (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_find_value) {
    ForwardList list;
    const int LIST_SIZE = 5000;
    const int SEARCH_OPS = 1000;

    for(int i = 0; i < LIST_SIZE; ++i) {
        list.addTail("item_" + std::to_string(i));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < SEARCH_OPS; ++i) {
        list.findValue("item_" + std::to_string(LIST_SIZE / 2));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[BENCH] findValue (" << SEARCH_OPS << " ops): " << dur.count() << " µs" << std::endl;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(benchmark_remove_head) {
    ForwardList list;
    const int OPS = 5000;

    for(int i = 0; i < OPS; ++i) {
        list.addHead("val_" + std::to_string(i));
    }

    auto start = std::chrono::high_resolution_clock::now();
    while(list.getHead()) {
        list.removeHead();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[BENCH] removeHead (clear " << OPS << " items): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}

// Если хочешь использовать boost::chrono, замени std::chrono на boost::chrono
BOOST_AUTO_TEST_CASE(benchmark_add_head_boost) {
    ForwardList list;
    const int OPS = 5000;

    auto start = boost::chrono::high_resolution_clock::now();
    for(int i = 0; i < OPS; ++i) {
        list.addHead("boost_" + std::to_string(i));
    }
    auto end = boost::chrono::high_resolution_clock::now();
    auto dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(end - start);
    std::cout << "[BOOST BENCH] addHead (" << OPS << " ops): " << dur.count() << " ms" << std::endl;
    BOOST_CHECK(true);
}
#endif
