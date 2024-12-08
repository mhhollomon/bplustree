#include <bplustree.hpp>
#include <treeprinter.hpp>

#include <catch2/catch_all.hpp>

#include <vector>
#include <algorithm>
#include <random>

using namespace BPT;

//template<class K, class V, std::size_t FO>
template<template<class, class, std::size_t> class T, class K, class V, std::size_t FO>
void check_tree(T<K, V, FO> *node_ptr,
                void *parent) {

    using node_ptr_type = BPT::TreeNode<K, V, FO> *;

    REQUIRE(node_ptr != nullptr);
    REQUIRE(node_ptr->parent == parent);

    if (node_ptr->is_leaf()) return;

    for (int index = 0 ; index < node_ptr->num_keys; ++index ) {
        auto curr_key = node_ptr->keys[index];

        auto *child_ptr = node_ptr_type(node_ptr->child_ptrs[index]);

        for(int index = 0 ; index < child_ptr->num_keys; ++index) {
            REQUIRE(curr_key > child_ptr->keys[index]);

        }

        check_tree(child_ptr, node_ptr);

        child_ptr = node_ptr_type(node_ptr->child_ptrs[index + 1]);

        for(int index = 0 ; index < child_ptr->num_keys; ++index) {
            REQUIRE(curr_key <= child_ptr->keys[index]);

        }

        check_tree(child_ptr, node_ptr);

    }

    
}


TEST_CASE("Basic Test", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(sizeof(tree) == 16);
}

TEST_CASE("One insert", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);

    auto * root = tree.get_root_ptr();

    REQUIRE(root->num_keys == 1);
    REQUIRE(root->keys[0] == 5);

    auto * values = tree.get_values();

    REQUIRE(values != nullptr);
    REQUIRE(values->previous == nullptr);
    REQUIRE(values->next == nullptr);
    REQUIRE(values->key == 5);
    REQUIRE(values->value == 50);

}

TEST_CASE("Two inserts", "[basic]") {
    BPlusTree<int, int> tree;

    tree.insert(5, 50);
    tree.insert(6, 60);

    auto * root = tree.get_root_ptr();

    REQUIRE(root->num_keys == 2);
    REQUIRE(root->keys[0] == 5);
    REQUIRE(root->keys[1] == 6);

    auto * values = tree.get_values();

    REQUIRE(values != nullptr);
    REQUIRE(values->previous == nullptr);
    REQUIRE(values->next != nullptr);
    REQUIRE(values->key == 5);
    REQUIRE(values->value == 50);

    values = values->next;
    REQUIRE(values->next == nullptr);
    REQUIRE(values->key == 6);
    REQUIRE(values->value == 60);


}

TEST_CASE("lower insert", "[basic]") {
    BPT::BPlusTree<int, int> tree;

    tree.insert(5, 50);
    tree.insert(3, 30);

    auto * root = tree.get_root_ptr();

    REQUIRE(root->num_keys == 2);
    REQUIRE(root->keys[0] == 3);
    REQUIRE(root->keys[1] == 5);

    auto * values = tree.get_values();

    REQUIRE(values != nullptr);
    REQUIRE(values->previous == nullptr);
    REQUIRE(values->next != nullptr);
    REQUIRE(values->key == 3);
    REQUIRE(values->value == 30);

    values = values->next;
    REQUIRE(values->next == nullptr);
    REQUIRE(values->key == 5);
    REQUIRE(values->value == 50);

    //tree_printer(tree).print();


}

TEST_CASE("need to split", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(3, 30) == true);
    REQUIRE(tree.insert(1, 10) == true);
    REQUIRE(tree.insert(10, 100) == true);
    REQUIRE(tree.insert(9, 90) == true);
    

    auto * root = tree.get_root_ptr();

    REQUIRE(root->num_keys == 1);
    REQUIRE(root->keys[0] == 5);

    auto * values = tree.get_values();

    REQUIRE(values != nullptr);
    REQUIRE(values->previous == nullptr);
    REQUIRE(values->next != nullptr);
    REQUIRE(values->key == 1);
    REQUIRE(values->value == 10);

    values = values->next;
    REQUIRE(values->next != nullptr);
    REQUIRE(values->key == 3);
    REQUIRE(values->value == 30);

    //tree_printer(tree).print();


}

 TEST_CASE("more split", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(3, 30) == true);
    REQUIRE(tree.insert(1, 10) == true);
    REQUIRE(tree.insert(10, 100) == true);
    REQUIRE(tree.insert(9, 90) == true);
    REQUIRE(tree.insert(11, 110) == true);
    REQUIRE(tree.insert(4, 40) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(20, 200) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(8, 80) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(21, 210) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(15, 150) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(30, 300) == true);
    //tree_printer(tree).print();
    REQUIRE(tree.insert(25, 250) == true);
    //tree_printer(tree).print();

    REQUIRE(tree.insert(40, 400) == true);
    //tree_printer(tree).print();
    REQUIRE(tree.insert(35, 350) == true);
    //tree_printer(tree).print();
}

TEST_CASE("at", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.at(5) == 50);


}

TEST_CASE("remove", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(6, 60) == true);

    REQUIRE(tree.remove(20) == false);

    REQUIRE(tree.remove(5) == true);

    {
        auto find_result = tree.find(5);
        REQUIRE(find_result.first == false);
    }

    {
        auto find_result = tree.find(6);
        REQUIRE(find_result.first == true);
        REQUIRE(find_result.second == 60);
    }

    //tree_printer(tree).print();

    REQUIRE(tree.insert(3, 30) == true);
    //tree_printer(tree).print();
    {
        auto find_result = tree.find(3);
        REQUIRE(find_result.first == true);
    }

    REQUIRE(tree.insert(5, 5000) == true);
    //tree_printer(tree).print();
    {
        auto find_result = tree.find(5);
        REQUIRE(find_result.first == true);
        REQUIRE(find_result.second == 5000);
    }

    REQUIRE(tree.remove(5) == true);
    REQUIRE(tree.insert(10, 100) == true);
    REQUIRE(tree.insert(11, 110) == true);
    //tree_printer(tree).print();
    {
        auto find_result = tree.find(10);
        REQUIRE(find_result.first == true);
        REQUIRE(find_result.second == 100);
    }

}

TEST_CASE("Multilevel split", "[split]") {

    const int num_inserts = 125;

    std::vector<int> keys;
    int total_inserts = num_inserts;
    int total_removes = 0;

    BPlusTree<int, int> tree;
    auto tp = tree_printer{tree};
    keys.reserve(num_inserts);


    for (int i = 0; i < num_inserts; ++i) {
        keys.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());

    // inclusive
    std::uniform_int_distribution<int> shuffle_dist(0, num_inserts-1);

    // Shuffle the array
    std::shuffle(keys.begin(), keys.end(), g); 

    // load a bunch
    for (int index = 0; index < keys.size(); ++index) {
        int i = keys.at(index);
        //std::cout << "INSERT " << index << "(" << i << ") ------------------------------------\n";
        REQUIRE(tree.insert(i, i*10) == true);
        //tp.print(false);
        check_tree(tree.get_root_ptr(), nullptr);
    }

}
