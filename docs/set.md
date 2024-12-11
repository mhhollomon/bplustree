# BPT::set

A set container built on top of the BPlusTree class.

Follows the interface for std::set, though not all of methods are implemented.

```cpp
template<class Key>
class set;
```

The class doesn't currently give you access to change the fan out of the underlying
tree.

However, the default fan-out of 20 should give this class a significant lookup advantage
over `std::set` when there are 1000s of items in the set.

## Constructors

```cpp
set();

template<class InputIt>
set(InputIt first, InputIt last);

set(std::initializer_list<value_type> ilist);

set(const set & other);

set(set&& other);
```

## Assignment

```cpp
set& operator=(const set& other);
set& operator=( set&& other ) noexcept;
set& operator=( std::initializer_list<value_type> ilist );
```

### Other Methods

```cpp
std::size_t size() const;
bool empty() const;

// Note only const iterators are available
auto begin() const;
auto end() const;
auto cbegin() const;
auto cend() const;

std::pair<const_iterator, bool> insert(const value_type & value);

// Returns the number of records actually erased (similar to std::set)
// Thus, will only ever return 1 or 0.
std::size_t erase(const key_type & key);

const_iterator find(const key_type & key) const;

bool contains(const key_type & key) const;

std::size_t count(const key_type & key) const;
```

## example

```cpp

#include <bplusset.hpp>
#include <iostream>
#include <cmath>

void main() {
    BPT::set<int> numbers = {3,5,7}; // use init list

    auto results = numbers.insert(9);
    if (results.second) {
        std::cout << "We did NOT see 9 before\n";
    } else {
        std::cout << "We saw 9 before\n";
    }

    if (numbers.contains(9)) {
        std::cout << "We have seen 9\n";
    }

    auto found = numbers.find(3);
    if (found == numbers.end()) {
        std::cout << "3 is in the set\n";
    } else {
        std::cout << "3 is NOT in the set\n";
    }

    auto num_erased = numbers.erase(3);
    std::cout << "We erased" << num_erased << "key(s)\n";

    std::cout << "There are " << numbers.count(3) << " 3s in the set\n";

}

// OUTPUT
// We did NOT see 9 before
// We have seen 9
// 3 is in the set
// We erased 1 key(s)
// There are 0 3s in the set
```