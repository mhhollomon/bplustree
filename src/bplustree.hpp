#pragma once

#ifndef _bplustree_include_guard__
#define _bplustree_include_guard__

#include <cstddef>
#include <memory>
#include <array>
#include <variant>
#include <format>
#include <cassert>


namespace BPT {
/**************************************/

#include "include/_value_wrapper.hpp"
#include "include/_tree_node.hpp"

constexpr static std::size_t DEFAULT_FAN_OUT = 5;

template<class K, class V, std::size_t FO = DEFAULT_FAN_OUT>
class BPlusTree {

public :

    using key_type = K;
    using value_type = V;
    constexpr static std::size_t fan_out = FO;
    using tree_node_type = TreeNode<K, V, fan_out>;
    using value_wrapper_type = ValueWrapper<K, V>;

    BPlusTree() : root_node_(new tree_node_type()) {
        root_node_->ntype = LeafNode;
    }

    ~BPlusTree() {

        _clear_tree(root_node_);
        root_node_ = nullptr;

        if (values_) {
            auto *current = values_;

            while (current) {
                auto *next = current->next;
                delete current;
                current = next;
            }

            values_ = nullptr;
        }
    }

    tree_node_type &get_root() const { return *root_node_; }
    tree_node_type *get_root_ptr() const { return root_node_; }

    value_wrapper_type *get_values() const { return values_; }

    struct FindResults {
        bool found;
        tree_node_type *node;
        std::size_t index;

        FindResults(bool f, tree_node_type *ptr, std::size_t i = 0) :
            found(f),node(ptr), index(i)
            {}

    };

    /**********************************
     * _find
     * Returns the leaf the new key should be inserted to.
     **********************************/
    FindResults _find(key_type key) {

        auto *current_node_ptr = get_root_ptr();
        bool found = false;
        int found_index = -1;

        while (1) {
            if (current_node_ptr->ntype == LeafNode) {

                for (size_t index = 0; index < current_node_ptr->num_keys; ++index) {
                    if (current_node_ptr->keys[index] == key) {
                        found = true;
                        found_index = index;
                        break; // out of for-loop
                    }
                }
                // We are at a leaf so no further recursion possible.
                // either we found it in the loop or we did not.
                break; // out of while-loop

            } else if (current_node_ptr->ntype == InternalNode) {

                bool found_next_node = false;
                for (size_t index = 0; index < current_node_ptr->num_keys; ++index) {
                    if (key < current_node_ptr->keys[index]) {
                        current_node_ptr = (tree_node_type *)(current_node_ptr->child_ptrs[index]);
                        found_next_node = true;
                        break; // out of for-loop
                    }
                }
                if (not found_next_node) {
                    current_node_ptr = (tree_node_type *)(current_node_ptr->child_ptrs[current_node_ptr->num_keys]);
                }

            } else {
                throw std::runtime_error(std::format("Unknown node type {}", int(current_node_ptr->ntype)));
            }
        }

        return FindResults(found, current_node_ptr, found_index);
    }

    /**********************************
     * _clear_tree
     * Only clears the tree structure not the value list.
     **********************************/
    void _clear_tree(tree_node_type *node) {

        if (node->is_internal()) {
            int max = node->num_keys;
            for(int i = 0; i < max; ++i) {
                _clear_tree((tree_node_type *)node->child_ptrs[i]);
            }
        }

        delete node;
    }


    /**********************************
     * _split_node
     **********************************/
    void _split_node(tree_node_type *old_node, key_type &new_key, void *child_ptr ) {

        auto *new_node = new tree_node_type(old_node->ntype);

        // copy over half the key/values from the old leaf
        int new_index = 0;
        const int split_index = (tree_node_type::key_limit/2);
        for (int old_index = split_index; 
                old_index < tree_node_type::key_limit; 
                ++old_index, ++new_index) {
            // can I use swap_range instead of a loop?
            std::swap(new_node->keys[new_index],old_node->keys[old_index]);
            std::swap(new_node->child_ptrs[new_index],old_node->child_ptrs[old_index]);
        }

        new_node->num_keys = tree_node_type::key_limit - split_index;
        old_node->num_keys = split_index;

        if (old_node->is_internal()) {
            // fix up terminal pointers
            old_node->child_ptrs[old_node->num_keys] = new_node->child_ptrs[0];
            new_node->child_ptrs[new_node->num_keys] = old_node->child_ptrs[old_node->child_ptrs.size()-1];
        }


        // need to integrate this into the loop above. this does more shuffling.

        if (new_key < old_node->max_key()) {
            _insert_into_node(old_node, new_key, child_ptr);
        } else {
            _insert_into_node(new_node, new_key, child_ptr);
        }

        if (old_node->parent) {
            auto min_key = new_node->min_key();
            if (not old_node->parent->is_full()) {
                _insert_into_node(old_node->parent, min_key, new_node);
            } else {
                _split_node(old_node->parent, min_key, new_node);
            }
            new_node->parent = old_node->parent;
        } else {
            // Must be at root, so create fresh node and jam lowest key in new
            // leaf into it.
            auto * new_parent = new tree_node_type ();

            new_parent->keys[0] = new_node->keys[0];
            new_parent->child_ptrs[0] = old_node;
            new_parent->child_ptrs[1] = new_node;
            new_parent->num_keys = 1;

            old_node->parent = new_node->parent = new_parent;
            root_node_ = new_parent;
        }

    }

    /**********************************
     * _insert_into_node
     **********************************/

    void _insert_into_node(tree_node_type *node, key_type &new_key, void* new_child ) {
        assert(not node->is_full());

        int insert_index = node->num_keys;

        auto * keys_ptr = node->keys.data();
        auto ** child_ptr = node->child_ptrs.data();
        

        for (int check_index = insert_index-1;
                insert_index >= 0;
                --check_index, --insert_index) {
            if (new_key > keys_ptr[check_index]) {
                keys_ptr[insert_index] = new_key;

                if (node->is_internal()) {
                    child_ptr[insert_index+2] = child_ptr[insert_index+1];
                    child_ptr[insert_index+1] = new_child;
                } else {
                    child_ptr[insert_index] = new_child;
                    auto *new_value_ptr = (value_wrapper_type *)new_child;
                    auto ** value_ptr = (value_wrapper_type **)child_ptr;
                    if (check_index >= 0) {

                        new_value_ptr->next = value_ptr[check_index]->next;
                        value_ptr[check_index]->next = new_value_ptr;
                        new_value_ptr->previous = value_ptr[check_index];
                    } else {
                        // inserting into the beginning of the list
                        new_value_ptr->next = value_ptr[insert_index+1];

                        if (not values_ or values_ == new_value_ptr->next) {
                            values_ = new_value_ptr;
                        }
                    }
  
                }

                break; // for loop

            } else {
                // shove the current resident up one.
                keys_ptr[insert_index] = keys_ptr[check_index];

                if (node->is_internal()) {
                    child_ptr[insert_index+2] = child_ptr[insert_index+1];
                } else {
                    child_ptr[insert_index] = child_ptr[check_index];

                }
            }
        }
        node->num_keys += 1;

    }


    /**********************************
     * INSERT
     **********************************/
    bool insert(key_type key, value_type value) {

        auto find_results = _find(key);

        if (find_results.found) {
            // future to do - support multimap
            // Support insert_or_assign
            return false;
        }

        auto *leaf_ptr = find_results.node;

        if (leaf_ptr->num_keys == tree_node_type::key_limit ) {
            _split_node(leaf_ptr, key, new value_wrapper_type(key, value));
        } else {
            _insert_into_node(leaf_ptr, key, new value_wrapper_type(key, value));

        }


        return true;
    }


    /*******************************************************
     * INTERFACE for C++20
     ******************************************************/
    /**********************************
     * at
     **********************************/
    value_type & at(const key_type &key) {
        auto results = _find(key);

        if (results.found) {
            auto * value_ptr = (value_wrapper_type *)results.node->child_ptrs[results.index];
            return value_ptr->value;
        } else {
            throw std::out_of_range("Could not find key");
        }
    }

private :
    tree_node_type* root_node_;
    value_wrapper_type* values_ = nullptr;

};


/**************************************/
}

#endif

