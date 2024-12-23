# BPT::BPlusTree

### Constructor
```cpp
// default constructor
BPT::BPlusTree<class K, class V, std::size_t FO = 20, class COMPARE = std::less<K>>();
// copy constructor
BPT::BPlusTree(const BPT::BPlusTree &other);
// move constructor
BPT::BPlusTree(BPT::BPlusTree &&other);
```
- `K` is the key type.
- `V` is the value type.
- `FO` is the Fanout - maximum number of nodes per level.
- `COMPARE` is the function object to use to compare keys.

**Note that FO must be larger than 3.**

### Assignment
```cpp
// copy assigment
BPlusTree &operator=(BPlusTree const &other);
//move assignment
BPlusTree &operator=(BPlusTree &&other)
``

### Insert
```cpp
std::pair<const_iterator, bool> insert(const key_type &key, mapped_type value);
```

Returns `true` if it did the insert. If the key already exists, returns 
`false`.

The iterator will point to either the newly insert item or the item that blocked
the insertion.

### Remove
```cpp
bool remove(const key_type &key);
```

Returns `true` if it actually needed to remove the key.

### Find
```cpp
const_iterator find(const key_type &key) const;
```

### Clear
```cpp
void clear();
```
Removes all keys from the tree.

### Compute_size
```cpp
std::size_t compute_size() const;
```

### Iterators
Note that only const iterators are provide at the moment.
Allowing the user to actually change keys/values through the iterator will
destroy the tree.

Also, the iterator classes are private.

The only operation that invalidates iterators is `clear()`.

```cpp
const_iterator cbegin() const;
const_iterator cend()const;
reverse_iterator crbegin() const;
reverse_iterator crend()const;
```

### At

```cpp
V & at(const K &key)
```

If the key is not found, it throws a `std::out_of_range` exception.

### treeprinter
 
Small class that dumps the tree in a compact ascii format.
Almost unreadable with high fan-out. But better than nothing.
```cpp
#include "bplustree.hpp"
#include "treeprinter.hpp"

auto tree = BPT::BPlusTree<int, int>();
tree.insert(1,1);
// ... etc

treeprinter(tree).print();
```
```
# example output

TREE --
0: <1>[0:1:I (2) 6 (3) ]
1: <2>[1:2:L 3/3 5D/4 ] <3>[1:3:L 6/5 10/6 11/7 ]
VALUES --
(3)  3, 30
(4) D5, 5000
(5)  6, 60
(6)  10, 100
(7)  11, 110
```
Pointers are aliased to small integers to make the tree structure visible.

Each node is wrapped in brackets (`[]`). Before the node is the pointer
reference wrapped in angle brackets (`<>`).

Just after the opening bracket is "_parent_: _key-count_ :_node-type_"

In an Internal Node, the subtree pointers are wrapped in parentheses (`()`). The
 keys are left "plain".

In a Leaf Node, each entry is "_key_ / _value pointer_".

- 'D' means the entry has been marked as deleted.
- 'I' means the node is an Internal Node.
- 'L' means the node is a Leaf Node.


## Sample Code
```cpp
#include "bplustree.hpp"
#include <string>
#include <iostream>

using namespace BPT;

struct employee {
   int eid;
   std::string name;
}

auto employees = BPlusTree<int, employee, 50>();
employees.insert(5,  employee{5, "John"});
employees.insert(10, employee{10, "Mary"});

int eid = 10;

std::cout << "employee id " << eid << " is " << employees.at[eid].name << "\n";
```
