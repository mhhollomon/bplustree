#pragma once

#ifndef _bplustree_include_guard__
#define _bplustree_include_guard__

#include <cstddef>
#include <memory>
#include <array>
#include <variant>
#include <format>
#include <cassert>
#include <bitset>
#include <iterator>

//#include <iostream>



namespace BPT {
/**************************************/

#include "include/_value_wrapper.hpp"
#include "include/_tree_node.hpp"

constexpr static std::size_t DEFAULT_FAN_OUT = 5;

template<class Key>
class set;

template<class K, class V, std::size_t FO = DEFAULT_FAN_OUT>
class BPlusTree {

public :

    using key_type = K;
    using mapped_type = V;
    constexpr static std::size_t fan_out = FO;

    using value_wrapper_type = ValueWrapper<key_type, mapped_type>;
    using value_type = typename value_wrapper_type::kvpair;
    using tree_node_type = TreeNode<key_type, mapped_type, fan_out>;

    BPlusTree() : root_node_(new tree_node_type(LeafNode)) {
    }

    BPlusTree(BPlusTree &other) : root_node_(new tree_node_type(LeafNode)) {
        operator=(other);
    }


    void swap(BPlusTree &other) {
        std::swap(root_node_, other.root_node_);
        std::swap(values_, other.values_);
    }

    BPlusTree(BPlusTree &&other) : root_node_(new tree_node_type(LeafNode)) {
        swap(other);
    }

    BPlusTree &operator=(BPlusTree const &other) {
        clear();
        for (auto const & it : std::as_const(other)) {
            insert(it.key, it.value);
        }

        return *this;
    }

    BPlusTree &operator=(BPlusTree &&other) {
        swap(other);
    }

    ~BPlusTree() {
        _clear_all(true, false);
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

private:

    /*********************************************************************
     * Private interface
     ********************************************************************/
    struct const_iterator {

        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;

        // hopefully these works.
        using value_type = value_type;
        using value_wrapper_type = value_wrapper_type;

        using pointer = value_type const *;
        using reference = value_type const &;

        const_iterator(value_wrapper_type const *ptr) : ptr_{ptr} {

            while (ptr_ and ptr_->deleted) {
                ptr_ = ptr_->next;
            }
        }

        reference operator*() const {
            return ptr_->kv;
        }

        pointer operator->() const { return &(ptr_->kv); }

        // Prefix increment
        const_iterator & operator++() {
            // according to the standard
            // incrementing past the end() is "undefined"
            // So don't bother trying to catch anything.
            do {
                ptr_ = ptr_->next;
            } while(ptr_ and ptr_->deleted );

            return *this;
        }

        // Postfix increment
        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }


        friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.ptr_ == b.ptr_; };
        friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.ptr_ != b.ptr_; };

    private :
        value_wrapper_type const * ptr_;

    };

    struct iterator {

        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;

        // hopefully these works.
        using value_type = value_type;
        using value_wrapper_type = value_wrapper_type;

        using pointer = value_type*;
        using reference = value_type&;

        iterator(value_wrapper_type *ptr) : ptr_{ptr} {

            while (ptr_ and ptr_->deleted) {
                ptr_ = ptr_->next;
            }
        }

        reference operator*() const {
            return ptr_->kv;
        }

        pointer operator->() const { return &(ptr_->kv); }

        // Prefix increment
        iterator & operator++() {
            // according to the standard
            // incrementing past the end() is "undefined"
            // So don't bother trying to catch anything.
            do {
                ptr_ = ptr_->next;
            } while(ptr_ and ptr_->deleted );

            return *this;
        }

        // Postfix increment
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }


        friend bool operator== (const iterator& a, const iterator& b) { return a.ptr_ == b.ptr_; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr_ != b.ptr_; };

    private :
        value_wrapper_type *ptr_;

    };


    /**********************************
     * _find
     * Returns the leaf the new key should be inserted to.
     * Note : returns "true" even if the key has been deleted.
     * It is up to the caller to decide if that is good or bad.
     **********************************/
    FindResults _find(key_type key) const {

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
     * _clear_all
     **********************************/
    void _clear_all(bool clear_values = true, bool new_root = true) {

        if (new_root == false or root_node_->is_internal() or root_node_->num_keys > 0 ) { 
            _clear_tree(root_node_);
            if (new_root) {
                root_node_ = new tree_node_type(LeafNode);
            } else {
                root_node_ = nullptr;
            }
        }

        if (clear_values and values_) {
            auto *current = values_;

            while (current) {
                auto *next = current->next;
                delete current;
                current = next;
            }

            values_ = nullptr;
        }

    }


    /**********************************
     * _clear_tree
     * Only clears the tree structure not the value list.
     **********************************/
    void _clear_tree(tree_node_type *node) {

        if (node->is_internal()) {
            int max = node->num_keys;
            for(int i = 0; i <= max; ++i) {
                _clear_tree((tree_node_type *)node->child_ptrs[i]);
            }
        }

        delete node;
    }

    /**********************************
     * _split_internal
     * Returns the new node that is created.
     **********************************/
    tree_node_type * _split_internal(tree_node_type *old_node, const key_type &new_key, tree_node_type *new_child ) {
        assert(old_node != nullptr);
        assert(new_child != nullptr);
        assert(old_node->is_internal());
        assert(old_node->is_full());

        //std::cout << std::format("_split_internal : start new_key = {}\n", new_key);

        std::size_t promoted_index = old_node->key_limit / 2 - 1;
        key_type promoted_key = old_node->keys[promoted_index];
        bool new_key_promoted = false;
        //std::cout << std::format("_split_internal : (1) promoted_key = {} promoted_index = {}\n", promoted_key, promoted_index);


        if (new_key > promoted_key) {
            promoted_index += 1;
            promoted_key = old_node->keys[promoted_index];

            //std::cout << std::format("_split_internal : (2) promoted_key = {} promoted_index = {}\n", promoted_key, promoted_index);

            if (new_key < promoted_key) {
                promoted_key = new_key;
                new_key_promoted = true;
            }

        }

        auto *new_node = new tree_node_type(old_node->ntype);

        // if new_key_promoted then the promoted_index must be copied
        // to the new node.
        // If NOT new_key_promoted, then the position to the right of
        // promoted_index needs to be copied.
        std::size_t copy_min = promoted_index + 1 - new_key_promoted;
        std::size_t new_index = 0;
        std::size_t old_index = copy_min;

        //std::cout << std::format("_split_internal : (3) promoted_key = {} promoted_index = {} copy_min = {}\n", promoted_key, promoted_index, copy_min);


        for (; 
                old_index < old_node->keys.size(); 
                ++old_index, ++new_index) {

            new_node->keys[new_index] = old_node->keys[old_index];
            new_node->child_ptrs[new_index] = old_node->child_ptrs[old_index];

            ((tree_node_type *)(new_node->child_ptrs[new_index]))->parent = new_node;

        }

        new_node->child_ptrs[new_index] = old_node->child_ptrs[old_index];
        ((tree_node_type *)(new_node->child_ptrs[new_index]))->parent = new_node;

        new_node->num_keys = tree_node_type::key_limit - copy_min;
        if (new_key_promoted) {
            old_node->num_keys = copy_min;

            new_node->child_ptrs[0] = new_child;
            new_child->parent = new_node;
            // switch back
            ((tree_node_type *)(old_node->child_ptrs[copy_min]))->parent = old_node;

        } else {
            old_node->num_keys = copy_min - 1;

            if (new_key < promoted_key) {
                _insert_into_node(old_node, new_key, new_child);
                new_child->parent = old_node;
            } else {
                _insert_into_node(new_node, new_key, new_child);
                new_child->parent = new_node;
            }

        }
        
        
        if (old_node->parent) {
            if (not old_node->parent->is_full()) {
                _insert_into_node(old_node->parent, promoted_key, new_node);
                new_node->parent = old_node->parent;
            } else {
                _split_node(old_node->parent, promoted_key, new_node);
            }
        } else {
            // Must be at root, so create fresh node and jam lowest key in new
            // leaf into it.
            auto * new_parent = new tree_node_type(InternalNode);

            new_parent->keys[0] = promoted_key;
            new_parent->child_ptrs[0] = old_node;
            new_parent->child_ptrs[1] = new_node;
            new_parent->num_keys = 1;

            old_node->parent = new_node->parent = new_parent;
            root_node_ = new_parent;
        }

        return new_node;

    }

    /**********************************
     * _split_node
     * Returns the new node that is created.
     **********************************/
    tree_node_type * _split_node(tree_node_type *old_node, const key_type &new_key, void *child_ptr ) {
        assert(old_node != nullptr);
        assert(child_ptr != nullptr);

        if (old_node->is_internal()) {
            return _split_internal(old_node, new_key, reinterpret_cast<tree_node_type *>(child_ptr));
        }

        //std::cout << "splitting : " << old_node->is_full() << "\n";

        auto *new_node = new tree_node_type(old_node->ntype);

        // copy over half the key/values from the old leaf
        int new_index = 0;
        const int split_index = (tree_node_type::key_limit/2);
        for (int old_index = split_index; 
                old_index < tree_node_type::key_limit; 
                ++old_index, ++new_index) {
            new_node->keys[new_index] = old_node->keys[old_index];
            new_node->child_ptrs[new_index] = old_node->child_ptrs[old_index];
        }


        //std::cout << "copy done\n";

        new_node->num_keys = tree_node_type::key_limit - split_index;
        old_node->num_keys = split_index;


        // need to integrate this into the loop above. this does more shuffling.

        if (new_key < new_node->min_key()) {
            //std::cout << "split : inserting into old_node\n";
            _insert_into_node(old_node, new_key, child_ptr);
        } else {
            //std::cout << "split : inserting into new_node\n";
            _insert_into_node(new_node, new_key, child_ptr);
        }

        if (old_node->parent) {
            auto min_key = new_node->min_key();
            if (not old_node->parent->is_full()) {
                _insert_into_node(old_node->parent, min_key, new_node);
                new_node->parent = old_node->parent;
            } else {
                _split_node(old_node->parent, min_key, new_node);
            }
        } else {
            // Must be at root, so create fresh node and jam lowest key in new
            // leaf into it.
            auto * new_parent = new tree_node_type(InternalNode);

            new_parent->keys[0] = new_node->keys[0];
            new_parent->child_ptrs[0] = old_node;
            new_parent->child_ptrs[1] = new_node;
            new_parent->num_keys = 1;

            old_node->parent = new_node->parent = new_parent;
            root_node_ = new_parent;
        }

        return new_node;

    }

    /**********************************
     * _insert_into_node
     **********************************/

    void _insert_into_node(tree_node_type *node, const key_type &new_key, void* new_child ) {
        assert(node != nullptr);
        assert(new_child != nullptr);
        assert(not node->is_full());

        //std::cout << "_insert : (" << node->is_full() << "), key = " << new_key << "\n";

        int insert_index = node->num_keys;

        auto * keys_ptr = node->keys.data();
        auto ** child_ptr = node->child_ptrs.data();

        if (node->is_empty()) {
            //std::cout << "_insert : node is empty\n";

            // Only happens if this is the first insert into the tree.
            // the node will be a leaf node.
            assert(node->is_leaf());
            assert(values_ == nullptr);

            child_ptr[0] = new_child;
            keys_ptr[0] = new_key;
            node->num_keys = 1;

            values_ = (value_wrapper_type *)new_child;
    
            return;

        }
        

        for (int check_index = insert_index-1;
                insert_index >= 0;
                --check_index, --insert_index) {
            
            //std::cout << std::format("_insert : loop - check_index = {}, insert_index = {}\n", check_index, insert_index);
            if (check_index < 0 or new_key > keys_ptr[check_index]) {
                keys_ptr[insert_index] = new_key;

                if (node->is_internal()) {
                    child_ptr[insert_index+2] = child_ptr[insert_index+1];
                    child_ptr[insert_index+1] = new_child;
                } else {
                    node->deleted[insert_index]  = false;
                    child_ptr[insert_index] = new_child;
                    auto *new_value_ptr = (value_wrapper_type *)new_child;
                    auto ** value_ptr = (value_wrapper_type **)child_ptr;
                    if (check_index >= 0) {
                        auto *new_next_ptr = new_value_ptr->next = value_ptr[check_index]->next;
                        value_ptr[check_index]->next = new_value_ptr;

                        new_value_ptr->previous = value_ptr[check_index];
                        if (new_next_ptr) {
                            new_next_ptr->previous = new_value_ptr;
                        }
                    } else {
                        // look to the right
                        new_value_ptr->next = value_ptr[insert_index + 1];
                        new_value_ptr->previous = value_ptr[insert_index + 1]->previous;
                        value_ptr[insert_index + 1]->previous = new_value_ptr;

                        if (new_value_ptr->previous == nullptr) {
                            values_ = new_value_ptr;
                        } else {
                            new_value_ptr->previous->next = new_value_ptr;
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
                    node->deleted[insert_index] = node->deleted[check_index];

                }
            }
        }

        //std::cout << "_insert : updating num_keys\n";
        node->num_keys += 1;

    }

public:
    /*********************************************************************
     * Public interface
     ********************************************************************/

    /**********************************
     * INSERT
     **********************************/
    std::pair<const_iterator, bool> insert(const key_type &key, mapped_type value) {

        auto find_results = _find(key);

        //std::cout << "insert : find " << key << "(" << find_results.found << ")\n";

        if (find_results.found) {
            auto * value_ptr = find_results.node->get_value_ptr(find_results.index);
            if (find_results.node->deleted[find_results.index]) {
                // deleted - update the value and "undelete"
                value_ptr->kv.value = value;
                value_ptr->deleted = false;
                find_results.node->deleted[find_results.index] = false;
                return {{value_ptr}, true};

            } else {
                // future to do - support multimap
                // Support insert_or_assign
                return {{value_ptr}, false};
            }
        }

        auto *leaf_ptr = find_results.node;

        //std::cout << "insert : just before action\n";

        auto *value_ptr = new value_wrapper_type(key, value);

        if (leaf_ptr->num_keys == tree_node_type::key_limit ) {
            _split_node(leaf_ptr, key, value_ptr);
        } else {
            _insert_into_node(leaf_ptr, key, value_ptr);

        }


        return {{value_ptr}, true};
    }


    /**********************************
     * REMOVE
     **********************************/
    bool remove(const key_type &key) {
        auto results = _find(key);

        if (results.found) {
            if (results.node->deleted[results.index]) {
                return false;
            }
            results.node->deleted.set(results.index, true);
            auto * value_ptr = results.node->get_value_ptr(results.index);
            value_ptr->deleted = true;
        }

        return results.found;
    }

    /*********************************
     * FIND
     *********************************/
    const_iterator find(const key_type &key) const {

        auto results = _find(key);

        if (results.found) {
            if (results.node->deleted[results.index]) {
                return cend();
            } else {
                auto * value_ptr = (value_wrapper_type *)results.node->child_ptrs[results.index];
                return const_iterator{value_ptr};
            }
        }

        return cend();

    }
    /*********************************
     * CLEAR
     *********************************/
    
    void clear() {
        _clear_all();
    }

    /*********************************
     * BEGIN
     *********************************/
    //iterator begin() { return iterator(values_); }

    /*********************************
     * END
     *********************************/
    //iterator end() { return iterator(nullptr); }


    /*********************************
     * CBEGIN
     *********************************/
    const_iterator cbegin() const { return const_iterator(values_); }
    const_iterator begin() const { return const_iterator(values_); }

    /*********************************
     * CEND
     *********************************/
    const_iterator cend() const { return const_iterator(nullptr); }
    const_iterator end() const { return const_iterator(nullptr); }


    /*********************************
     * COMPUTE_SIZE
     * 
     * Adding a size attribute to the tree would create a contention hotspot
     * when we start adding support for concurrency.
     *********************************/
    std::size_t compute_size() const {
        std::size_t size = 0;

        for (auto const & it : *this) {
            size += 1;
        }

        return size;
    }


    /*******************************************************
     * INTERFACE for C++20
     ******************************************************/
    /**********************************
     * at
     **********************************/
    mapped_type & at(const key_type &key) {
        auto results = _find(key);

        if (results.found) {
            auto * value_ptr = (value_wrapper_type *)results.node->child_ptrs[results.index];
            return value_ptr->kv.value;
        } else {
            throw std::out_of_range("Could not find key");
        }
    }

private :
    tree_node_type* root_node_;
    value_wrapper_type* values_ = nullptr;

    friend class set<K>;


};


template<class K, class V, std::size_t FO>
void swap(BPlusTree<K, V, FO> &a, BPlusTree<K, V, FO> &b) {
    a.swap(b);
}

/**************************************/
}

#endif

