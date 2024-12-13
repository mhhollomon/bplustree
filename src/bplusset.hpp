#pragma once

#include "bplustree.hpp"
#include <utility>

namespace BPT {


template<class Key>
class set {

public :
    using key_type = Key;
    using value_type = Key;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = value_type &;
    using const_reference = value_type const &;

    set() = default;

    template<class InputIt>
    set ( InputIt first, InputIt last) {
        while (first != last) {
            if (tree_.insert(*first, *first).second) {
                size_ += 1;
            }
            ++first;
        }
    }
    set( std::initializer_list<value_type> ilist) : set(ilist.begin(), ilist.end())
    {}

    set(const set & other) {
        operator=(other);
    }

    set(set&& other) {
        operator=(std::move(other));
    }

    set &operator=(const set& other) {
        tree_ = other.tree_;
        size_ = other.size_;

        return *this;
    }

    set& operator=( set&& other ) noexcept {
        swap(tree_, other.tree_);
        std::swap(size_, other.size_);

        return *this;
    }

    set& operator=( std::initializer_list<value_type> ilist ) {
        tree_.clear();
        size_ = 0;

        for (int k : ilist) {
            if (tree_.insert(k, k).second) {
                size_ += 1;
            }
        }

        return *this;

    }

    std::size_t size() const { return size_; }

    bool empty() const { return (size_ == 0); }

    struct const_iterator {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using value_type = value_type;
        using pointer = value_type const *;
        using reference = value_type const &;

        const_iterator(BPlusTree<Key, Key>::const_iterator iter) : tree_iter_{iter} {
        }

        reference operator*() const {
            return tree_iter_->key;
        }

        pointer operator->() const { return &(tree_iter_->key); }

        const_iterator & operator++() { tree_iter_++; return *this; }

        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.tree_iter_ == b.tree_iter_; };
        friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.tree_iter_ != b.tree_iter_; };

    private :
        BPlusTree<Key, Key>::const_iterator tree_iter_;

    };

    auto begin() const {
        return const_iterator(tree_.begin());
    }

    auto end() const {
        return const_iterator(tree_.end());
    }

    auto cbegin() const {
        return const_iterator(tree_.cbegin());
    }

    auto cend() const {
        return const_iterator(tree_.cend());
    }


    std::pair<const_iterator, bool> insert(const value_type & value) {
        auto retval = tree_.insert(value, value);
        if (retval.second) size_ += 1;
        return {{retval.first}, retval.second};
    }

    void clear() {
        tree_.clear();
        size_ = 0;
    }

    std::size_t erase (const key_type & key) {
        auto retval = tree_.remove(key);
        if (retval) size_ -= 1;
        return retval;
    }

    const_iterator find(const key_type & key) const {
        return const_iterator{tree_.find(key)};
    }

    bool contains(const key_type & key) const {
        return tree_.contains(key);
    }

    std::size_t count(const key_type & key) const {
        return contains(key);
    }


private:


    BPlusTree<Key, Key> tree_;
    std::size_t size_ = 0;

    void compute_size() {
        size_ = tree_.compute_size();
    }

};


// END namespace --------------------
}