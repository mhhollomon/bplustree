#include <bplustree.hpp>
#include <treeprinter.hpp>

#include <catch2/catch_all.hpp>

using namespace BPT;


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
    REQUIRE(values->kv.key == 5);
    REQUIRE(values->kv.value == 50);

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
    REQUIRE(values->kv.key == 5);
    REQUIRE(values->kv.value == 50);

    values = values->next;
    REQUIRE(values->next == nullptr);
    REQUIRE(values->kv.key == 6);
    REQUIRE(values->kv.value == 60);


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
    REQUIRE(values->kv.key == 3);
    REQUIRE(values->kv.value == 30);

    values = values->next;
    REQUIRE(values->next == nullptr);
    REQUIRE(values->kv.key == 5);
    REQUIRE(values->kv.value == 50);

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
    REQUIRE(values->kv.key == 1);
    REQUIRE(values->kv.value == 10);

    values = values->next;
    REQUIRE(values->next != nullptr);
    REQUIRE(values->kv.key == 3);
    REQUIRE(values->kv.value == 30);

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



/* TEST_CASE("iterator", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(6, 60) == true);

    REQUIRE(tree.remove(5) == true);

    auto it = tree.begin();
    REQUIRE(it != tree.end());
    REQUIRE(it->key == 6);
    REQUIRE(it->value == 60);

    it++;

    REQUIRE(it == tree.end());

}
 */

TEST_CASE("const iterator", "[basic]") {
    BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(6, 60) == true);

    REQUIRE(tree.remove(5) == true);

    auto it = tree.cbegin();
    REQUIRE(it != tree.cend());
    REQUIRE(it->key == 6);
    REQUIRE(it->value == 60);

    it++;

    REQUIRE(it == tree.cend());

}


