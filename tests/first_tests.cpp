#include <bplustree.hpp>
#include <catch2/catch_all.hpp>

#include <iostream>
#include <queue>


template<class K, class V>
class tree_printer {
    using node_ptr_type = BPT::BPlusTree<K, V>::tree_node_type *;

    const BPT::BPlusTree<K, V> & tree_;
    std::queue<std::pair<int, node_ptr_type>> queue_;

    std::map<void *, int> ptr_map_;
    int numbers = 0;


public:
    tree_printer(const BPT::BPlusTree<K, V> & tree) : tree_{tree} {}

    void print() {
        queue_.push(std::make_pair(0, tree_.get_root_ptr()));

        int current_level = -1;
        std::cout << "TREE -- ";

        while (not queue_.empty()) {
            auto curr_pair = queue_.front();
            queue_.pop();
            int level = curr_pair.first;
            if (level != current_level) {
                std::cout << "\n" << level << ": ";
                current_level = level;
            } else {
                std::cout << ' ';
            }
            print_node(current_level+1, curr_pair.second);
        }

        std::cout << "\n";

        std::cout << "VALUES -- \n";
        auto * value_ptr = tree_.get_values();

        while (value_ptr != nullptr) {
            std::cout << "(" << get_alias(value_ptr) << ") " << value_ptr->key << ", " << value_ptr->value << "\n";
            value_ptr = value_ptr->next;
        }
/* 
        for (auto const &it : ptr_map_ ) {
            std::cout << it.first << " => " << it.second << "\n";
        } 
*/


    }

    int get_alias(void * node_ptr) {
        int alias;

        if (auto search = ptr_map_.find(node_ptr); search != ptr_map_.end()) {
            alias = search->second;
        } else {
            alias = numbers++;
            ptr_map_.insert(std::make_pair(node_ptr, alias));
        }

        return alias;

    }

    void print_node(int child_level, node_ptr_type node_ptr) {
        int alias = get_alias(node_ptr);

        std::cout << "<" << alias << ">[" << (node_ptr->is_internal() ? 'I' : 'L');

        for (int i = 0; i < node_ptr->num_keys; ++i) {
            if (node_ptr->is_internal()) {
                std::cout << " (" << get_alias(node_ptr->child_ptrs[i]) << ")"  << ' ' << node_ptr->keys[i];
            } else {
                std::cout << ' ' << node_ptr->keys[i] << "/" << get_alias(node_ptr->child_ptrs[i]);
            }
        }

        if (node_ptr->is_internal()) {
            std::cout << " (" << get_alias(node_ptr->child_ptrs[node_ptr->num_keys]) << ")";

        }

        std::cout << " ]";

        if (node_ptr->is_internal()) {
            for (int i = 0; i < node_ptr->num_keys+1; ++i) {
                queue_.push(std::make_pair(child_level, 
                    (node_ptr_type)node_ptr->child_ptrs[i]));
            }                
        }

    }


};


template <class K, class V>
void print_tree(const BPT::BPlusTree<K, V> & tree) {

    auto * node_ptr = tree.get_root_ptr();

    print_tree_node(node_ptr);
    std::cout << "\n";
}


TEST_CASE("Basic Test", "[basic]") {
    BPT::BPlusTree<int, int> tree;

    REQUIRE(sizeof(tree) == 16);
}

TEST_CASE("One insert", "[basic]") {
    BPT::BPlusTree<int, int> tree;

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
    BPT::BPlusTree<int, int> tree;

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

    tree_printer(tree).print();


}

TEST_CASE("need to split", "[basic]") {
    BPT::BPlusTree<int, int> tree;

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

    tree_printer(tree).print();


}

TEST_CASE("more split", "[basic]") {
    BPT::BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.insert(3, 30) == true);
    REQUIRE(tree.insert(1, 10) == true);
    REQUIRE(tree.insert(10, 100) == true);
    REQUIRE(tree.insert(9, 90) == true);
    REQUIRE(tree.insert(11, 110) == true);
    REQUIRE(tree.insert(4, 40) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(20, 200) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(8, 80) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(21, 210) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(15, 150) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(30, 300) == true);
    tree_printer(tree).print();
    REQUIRE(tree.insert(25, 250) == true);
    tree_printer(tree).print();

    REQUIRE(tree.insert(40, 400) == true);
    tree_printer(tree).print();
    REQUIRE(tree.insert(35, 350) == true);
    tree_printer(tree).print();
}

TEST_CASE("at", "[basic]") {
    BPT::BPlusTree<int, int> tree;

    REQUIRE(tree.insert(5, 50) == true);
    REQUIRE(tree.at(5) == 50);


}
