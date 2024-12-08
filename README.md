# bplustree

Implementation of a B+ Tree.

Requires C++20.

Will also implement the std::map and std::multimap interface at some
point.

MIT license.

## Concurrency

This is not concurrency safe without additional locking.
The tree methods can be called without problems from different threads. However,
two threads may not call methods at the same time - unless they are both reads.

Caveat Scriptor

## Interface (so far)

### Constructor
```cpp
BPT::BPlusTree<class K, class V, std::size_t FO = 5>();
```
- `K` is the key type.
- `V` is the value type.
- `FO` is the Fanout - maximum number of nodes per level.

### Insert
```cpp
bool insert(K key, V value);
```

Returns `true` if it did the insert. If the key already exists, returns 
`false`.

### Remove
```cpp
bool remove(const key_type &key);
```

Returns `true` if it actually needed to remove the key.

### Find
```cpp
std::pair<bool, const value_type &> find(const key_type &key);
```

### Clear
```cpp
void clear();
```
Removes all keys from the tree.

### Iterators
Note that only const iterators are provide at the moment.
Allowing the user to actually change keys/values through the iterator will
destroy the tree.

Also, the iterator classes are private.

The only operation that invalidates iterators is `clear()`.

```cpp
const_iterator cbegin();
const_iterator cend();
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

## TO DO
 
### Removing elements

There will be a `rebuild` method that will take of reclaiming space.

### Concurency

Use Optimistic Lock Coupling to allow concurrency.

(c.f. http://sites.computer.org/debull/A19mar/p73.pdf)

### std::map interface
This will probably be a new separate class that uses BPlusTree internally.

I don't plan to support all the really gnarly bits about hinting and node_ptrs.

### std::multimap interface

Similar to above.

### std::set interface

Similar to above.

### Optimizing
- Intranode searching is currently linear. switch to binary search.
- work on the splitting algorithm. It still does more data copies/moves than really necessary.

## Technology
- [Catch2](https://github.com/catchorg/Catch2) for testing framework.
- [CPM](https://github.com/cpm-cmake/CPM.cmake) for managing dependencies.