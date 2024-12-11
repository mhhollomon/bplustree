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

//---------------------------------------------------

TEST_CASE("Multilevel split", "[stress]") {

    const int num_inserts = 125;

    std::vector<int> keys;
    int total_inserts = num_inserts;
    int total_removes = 0;

    BPlusTree<int, int, 5> tree;
    auto tp = tree_printer{tree};
    keys.reserve(num_inserts);


    for (int i = 0; i < num_inserts; ++i) {
        keys.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());

    // Shuffle the array
    std::shuffle(keys.begin(), keys.end(), g); 

    // load a bunch
    for (int index = 0; index < keys.size(); ++index) {
        int i = keys.at(index);
        //std::cout << "INSERT " << index << "(" << i << ") ------------------------------------\n";
        REQUIRE(tree.insert(i, i*10).second == true);
        //tp.print(false);
        check_tree(tree.get_root_ptr(), nullptr);
    }

}
