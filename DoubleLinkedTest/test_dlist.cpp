#ifdef TEST_BUILD
#define BOOST_TEST_MODULE DoubleLinkedListTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "DoubleLinked.h"
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_CASE(test_constructor_destructor) {
    DoubleLinkedList list1;
    BOOST_CHECK(list1.getHead() == nullptr);
    BOOST_CHECK(list1.getTail() == nullptr);
    
    DoubleLinkedList* list2 = new DoubleLinkedList();
    list2->addHead("test");
    delete list2;
}

BOOST_AUTO_TEST_CASE(test_add_head_tail) {
    DoubleLinkedList list;
    
    list.addHead("first");
    BOOST_CHECK(list.getHead() != nullptr);
    BOOST_CHECK(list.getHead()->getValue() == "first");
    BOOST_CHECK(list.getTail()->getValue() == "first");
    
    list.addHead("second");
    BOOST_CHECK(list.getHead()->getValue() == "second");
    BOOST_CHECK(list.getHead()->getNext()->getValue() == "first");
    BOOST_CHECK(list.getTail()->getValue() == "first");
    BOOST_CHECK(list.getTail()->getPrev()->getValue() == "second");
    
    list.addTail("third");
    BOOST_CHECK(list.getTail()->getValue() == "third");
    BOOST_CHECK(list.getTail()->getPrev()->getValue() == "first");
    BOOST_CHECK(list.getHead()->getNext()->getNext()->getValue() == "third");
}

BOOST_AUTO_TEST_CASE(test_add_before) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.addBefore("B", "X") == true);
    DNode* node = list.getNode(1);
    BOOST_REQUIRE(node != nullptr);
    BOOST_CHECK(node->getValue() == "X");
    BOOST_CHECK(node->getPrev()->getValue() == "A");
    BOOST_CHECK(node->getNext()->getValue() == "B");
    
    BOOST_CHECK(list.addBefore("A", "Y") == true);
    BOOST_CHECK(list.getHead()->getValue() == "Y");
    
    BOOST_CHECK(list.addBefore("NOT_EXIST", "Z") == false);
}

BOOST_AUTO_TEST_CASE(test_add_after) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.addAfter("B", "X") == true);
    DNode* node = list.getNode(2);
    BOOST_REQUIRE(node != nullptr);
    BOOST_CHECK(node->getValue() == "X");
    BOOST_CHECK(node->getPrev()->getValue() == "B");
    BOOST_CHECK(node->getNext()->getValue() == "C");
    
    BOOST_CHECK(list.addAfter("C", "Y") == true);
    BOOST_CHECK(list.getTail()->getValue() == "Y");
    
    BOOST_CHECK(list.addAfter("NOT_EXIST", "Z") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_head_tail) {
    DoubleLinkedList list;
    
    BOOST_CHECK(list.removeHead() == false);
    BOOST_CHECK(list.removeTail() == false);
    
    list.addTail("A");
    BOOST_CHECK(list.removeHead() == true);
    BOOST_CHECK(list.getHead() == nullptr);
    BOOST_CHECK(list.getTail() == nullptr);
    
    list.addTail("A");
    list.addTail("B");
    BOOST_CHECK(list.removeTail() == true);
    BOOST_CHECK(list.getHead()->getValue() == "A");
    BOOST_CHECK(list.getTail()->getValue() == "A");
    BOOST_CHECK(list.getTail()->getNext() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_remove_value) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    list.addTail("D");
    
    BOOST_CHECK(list.removeValue("C") == true);
    BOOST_CHECK(list.getNode(0)->getValue() == "A");
    BOOST_CHECK(list.getNode(1)->getValue() == "B");
    BOOST_CHECK(list.getNode(2)->getValue() == "D");
    
    BOOST_CHECK(list.removeValue("A") == true);
    BOOST_CHECK(list.getHead()->getValue() == "B");
    
    BOOST_CHECK(list.removeValue("NOT_EXIST") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_before) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.removeBefore("B") == true);
    BOOST_CHECK(list.getHead()->getValue() == "B");
    BOOST_CHECK(list.getHead()->getNext()->getValue() == "C");
    BOOST_CHECK(list.getHead()->getPrev() == nullptr);
    
    BOOST_CHECK(list.removeBefore("B") == false);
    BOOST_CHECK(list.removeBefore("C") == true);
    BOOST_CHECK(list.getHead()->getValue() == "C");
    
    BOOST_CHECK(list.removeBefore("C") == false);
    BOOST_CHECK(list.removeBefore("NOT_EXIST") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_after) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    list.addTail("D");
    
    BOOST_CHECK(list.removeAfter("B") == true);
    BOOST_CHECK(list.getNode(0)->getValue() == "A");
    BOOST_CHECK(list.getNode(1)->getValue() == "B");
    BOOST_CHECK(list.getNode(2)->getValue() == "D");
    
    BOOST_CHECK(list.removeAfter("D") == false);
    BOOST_CHECK(list.removeAfter("NOT_EXIST") == false);
    
    BOOST_CHECK(list.removeAfter("A") == true);
    BOOST_CHECK(list.getNode(1)->getValue() == "D");
}

BOOST_AUTO_TEST_CASE(test_get_node_find) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.getNode(0)->getValue() == "A");
    BOOST_CHECK(list.getNode(1)->getValue() == "B");
    BOOST_CHECK(list.getNode(2)->getValue() == "C");
    BOOST_CHECK(list.getNode(3) == nullptr);
    BOOST_CHECK(list.getNode(-1) == nullptr);
    
    BOOST_CHECK(list.findValue("B") != nullptr);
    BOOST_CHECK(list.findValue("B")->getValue() == "B");
    BOOST_CHECK(list.findValue("NOT_EXIST") == nullptr);
}

BOOST_AUTO_TEST_CASE(test_file_operations) {
    DoubleLinkedList list1;
    list1.addTail("File");
    list1.addTail("Test");
    list1.addTail("Data");
    
    list1.writeToFile("test_file.txt");
    
    DoubleLinkedList list2;
    list2.readFromFile("test_file.txt");
    
    BOOST_CHECK(list2.getNode(0)->getValue() == "File");
    BOOST_CHECK(list2.getNode(1)->getValue() == "Test");
    BOOST_CHECK(list2.getNode(2)->getValue() == "Data");
    
    std::remove("test_file.txt");
    
    DoubleLinkedList list3;
    list3.readFromFile("non_existent.txt");
    BOOST_CHECK(list3.getHead() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_print_clear) {
    DoubleLinkedList list;
    list.addTail("Print");
    list.addTail("Test");
    
    list.printList();
    
    list.clear();
    BOOST_CHECK(list.getHead() == nullptr);
    BOOST_CHECK(list.getTail() == nullptr);
    
    list.clear();
    BOOST_CHECK(list.getHead() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_edge_cases) {
    DoubleLinkedList list;
    
    BOOST_CHECK(list.addBefore("X", "Y") == false);
    BOOST_CHECK(list.addAfter("X", "Y") == false);
    BOOST_CHECK(list.removeValue("X") == false);
    BOOST_CHECK(list.removeBefore("X") == false);
    BOOST_CHECK(list.removeAfter("X") == false);
    
    list.addHead("Single");
    BOOST_CHECK(list.removeBefore("Single") == false);
    BOOST_CHECK(list.removeAfter("Single") == false);
    BOOST_CHECK(list.removeValue("Single") == true);
}

BOOST_AUTO_TEST_CASE(test_binary_serialization_basic) {
    DoubleLinkedList original;
    original.addTail("first");
    original.addTail("second");
    original.addTail("third");
    
    BOOST_CHECK(original.serializeToBinary("test_bin.bin") == true);
    
    DoubleLinkedList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("test_bin.bin") == true);
    
    BOOST_CHECK(original.getNode(0)->getValue() == loaded.getNode(0)->getValue());
    BOOST_CHECK(original.getNode(1)->getValue() == loaded.getNode(1)->getValue());
    BOOST_CHECK(original.getNode(2)->getValue() == loaded.getNode(2)->getValue());
    
    std::remove("test_bin.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_serialization_empty) {
    DoubleLinkedList original;
    
    BOOST_CHECK(original.serializeToBinary("empty.bin") == true);
    
    DoubleLinkedList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("empty.bin") == true);
    
    BOOST_CHECK(loaded.getHead() == nullptr);
    BOOST_CHECK(loaded.getTail() == nullptr);
    
    std::remove("empty.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_serialization_large) {
    DoubleLinkedList original;
    for(int i = 0; i < 100; ++i) {
        original.addTail("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(original.serializeToBinary("large.bin") == true);
    
    DoubleLinkedList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("large.bin") == true);
    
    for(int i = 0; i < 100; ++i) {
        BOOST_CHECK(original.getNode(i)->getValue() == loaded.getNode(i)->getValue());
    }
    
    std::remove("large.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_deserialize_nonexistent) {
    DoubleLinkedList list;
    BOOST_CHECK(list.deserializeFromBinary("nonexistent_file.bin") == false);
}

BOOST_AUTO_TEST_CASE(test_integrity_check) {
    DoubleLinkedList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.checkIntegrity() == true);
    
    // После удаления тоже должна быть целостность
    list.removeHead();
    BOOST_CHECK(list.checkIntegrity() == true);
    
    list.clear();
    BOOST_CHECK(list.checkIntegrity() == true);
}
#endif