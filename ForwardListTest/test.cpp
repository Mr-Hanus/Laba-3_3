#ifdef BUILD_TESTS
#define BOOST_TEST_MODULE ForwardListTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "ForwardList.h"
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_CASE(test_constructor_destructor) {
    ForwardList list1;
    BOOST_CHECK(list1.getHead() == nullptr);
    BOOST_CHECK(list1.getNode(0) == nullptr);
    
    ForwardList* list2 = new ForwardList();
    list2->addHead("test");
    delete list2;
}

BOOST_AUTO_TEST_CASE(test_add_head_tail) {
    ForwardList list;
    
    list.addHead("head1");
    BOOST_CHECK(list.getHead() != nullptr);
    BOOST_CHECK(list.getHead()->getValue() == "head1");
    
    list.addHead("head2");
    BOOST_CHECK(list.getHead()->getValue() == "head2");
    BOOST_CHECK(list.getNode(1)->getValue() == "head1");
    
    list.addTail("tail1");
    BOOST_CHECK(list.getNode(2)->getValue() == "tail1");
    
    list.addTail("tail2");
    BOOST_CHECK(list.getNode(3)->getValue() == "tail2");
}

BOOST_AUTO_TEST_CASE(test_add_before) {
    ForwardList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.addBefore("B", "X") == true);
    BOOST_CHECK(list.getNode(0)->getValue() == "A");
    BOOST_CHECK(list.getNode(1)->getValue() == "X");
    BOOST_CHECK(list.getNode(2)->getValue() == "B");
    BOOST_CHECK(list.getNode(3)->getValue() == "C");
    
    BOOST_CHECK(list.addBefore("A", "Y") == true);
    BOOST_CHECK(list.getHead()->getValue() == "Y");
    
    BOOST_CHECK(list.addBefore("NOT_EXIST", "Z") == false);
}

BOOST_AUTO_TEST_CASE(test_add_after) {
    ForwardList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.addAfter("B", "X") == true);
    BOOST_CHECK(list.getNode(0)->getValue() == "A");
    BOOST_CHECK(list.getNode(1)->getValue() == "B");
    BOOST_CHECK(list.getNode(2)->getValue() == "X");
    BOOST_CHECK(list.getNode(3)->getValue() == "C");
    
    BOOST_CHECK(list.addAfter("C", "Y") == true);
    BOOST_CHECK(list.getNode(4)->getValue() == "Y");
    
    BOOST_CHECK(list.addAfter("NOT_EXIST", "Z") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_head_tail) {
    ForwardList list;
    
    BOOST_CHECK(list.removeHead() == false);
    BOOST_CHECK(list.removeTail() == false);
    
    list.addTail("A");
    BOOST_CHECK(list.removeHead() == true);
    BOOST_CHECK(list.getHead() == nullptr);
    
    list.addTail("A");
    list.addTail("B");
    BOOST_CHECK(list.removeTail() == true);
    BOOST_CHECK(list.getHead()->getValue() == "A");
    BOOST_CHECK(list.getHead()->getNext() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_remove_value) {
    ForwardList list;
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
    ForwardList list;
    list.addTail("A");
    list.addTail("B");
    list.addTail("C");
    
    BOOST_CHECK(list.removeBefore("B") == true);
    BOOST_CHECK(list.getHead()->getValue() == "B");
    BOOST_CHECK(list.getNode(1)->getValue() == "C");
    
    BOOST_CHECK(list.removeBefore("B") == false);
    BOOST_CHECK(list.removeBefore("C") == true);
    BOOST_CHECK(list.getHead()->getValue() == "C");
    
    BOOST_CHECK(list.removeBefore("C") == false);
    BOOST_CHECK(list.removeBefore("NOT_EXIST") == false);
}

BOOST_AUTO_TEST_CASE(test_remove_after) {
    ForwardList list;
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
    ForwardList list;
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
    ForwardList list1;
    list1.addTail("File");
    list1.addTail("Test");
    list1.addTail("Data");
    
    list1.writeToFile("test_file.txt");
    
    ForwardList list2;
    list2.readFromFile("test_file.txt");
    
    BOOST_CHECK(list2.getNode(0)->getValue() == "File");
    BOOST_CHECK(list2.getNode(1)->getValue() == "Test");
    BOOST_CHECK(list2.getNode(2)->getValue() == "Data");
    
    std::remove("test_file.txt");
    
    ForwardList list3;
    list3.readFromFile("non_existent.txt");
    BOOST_CHECK(list3.getHead() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_print_clear) {
    ForwardList list;
    list.addTail("Print");
    list.addTail("Test");
    
    list.printList();
    
    list.clear();
    BOOST_CHECK(list.getHead() == nullptr);
    
    list.clear();
    BOOST_CHECK(list.getHead() == nullptr);
}

BOOST_AUTO_TEST_CASE(test_edge_cases) {
    ForwardList list;
    
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
    ForwardList original;
    original.addTail("first");
    original.addTail("second");
    original.addTail("third");
    
    BOOST_CHECK(original.serializeToBinary("test_bin.bin") == true);
    
    ForwardList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("test_bin.bin") == true);
    
    BOOST_CHECK(original.getNode(0)->getValue() == loaded.getNode(0)->getValue());
    BOOST_CHECK(original.getNode(1)->getValue() == loaded.getNode(1)->getValue());
    BOOST_CHECK(original.getNode(2)->getValue() == loaded.getNode(2)->getValue());
    
    std::remove("test_bin.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_serialization_empty) {
    ForwardList original;
    
    BOOST_CHECK(original.serializeToBinary("empty.bin") == true);
    
    ForwardList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("empty.bin") == true);
    
    BOOST_CHECK(loaded.getHead() == nullptr);
    
    std::remove("empty.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_serialization_large) {
    ForwardList original;
    for(int i = 0; i < 100; ++i) {
        original.addTail("item_" + std::to_string(i));
    }
    
    BOOST_CHECK(original.serializeToBinary("large.bin") == true);
    
    ForwardList loaded;
    BOOST_CHECK(loaded.deserializeFromBinary("large.bin") == true);
    
    for(int i = 0; i < 100; ++i) {
        BOOST_CHECK(original.getNode(i)->getValue() == loaded.getNode(i)->getValue());
    }
    
    std::remove("large.bin");
}

BOOST_AUTO_TEST_CASE(test_binary_deserialize_nonexistent) {
    ForwardList list;
    BOOST_CHECK(list.deserializeFromBinary("nonexistent_file.bin") == false);
}

BOOST_AUTO_TEST_CASE(test_binary_integrity) {
    ForwardList original;
    original.addTail("test");
    original.addTail("data");
    original.addTail("123");
    
    original.serializeToBinary("integrity.bin");
    
    ForwardList loaded;
    loaded.deserializeFromBinary("integrity.bin");
    
    Node* origNode = original.getHead();
    Node* loadNode = loaded.getHead();
    int count = 0;
    
    while(origNode && loadNode) {
        BOOST_CHECK(origNode->getValue() == loadNode->getValue());
        origNode = origNode->getNext();
        loadNode = loadNode->getNext();
        count++;
    }
    
    BOOST_CHECK(origNode == nullptr);
    BOOST_CHECK(loadNode == nullptr);
    BOOST_CHECK(count == 3);
    
    std::remove("integrity.bin");
}
#endif