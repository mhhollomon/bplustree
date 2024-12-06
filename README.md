# bplustree

Implementation of a B+ Tree.

Requires C++20.

Will also implement the std::map and std::multimap interface at some
point.

MIT license.

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

 ### At

 ```cpp
 V & at(const K &key)
 ```

 If the key is not found, it throws a `std::out_of_range` exception.

 ## Sample
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

 emplyees.insert(5,  employee{5, "John"});
 emplyees.insert(10, employee{10, "Mary"});

 int eid = 10;

 std::cout << "employee id " << eid << " is " << employees.at[eid].name << "\n";
 ```

 ## TO DO
 
 ### Removing elements
 Plan is to set the nodes to a deleted status rather than actually removing them. Merging nodes as required by true removal makes locking hard.

 There will be a `rebuild` method that will take of reclaiming space.

 ### Concurency

 Use Optimistic Lock Coupling to allow concurrency.

 (c.f. http://sites.computer.org/debull/A19mar/p73.pdf)

 ### std::map interface
 This will probably be a new separate class that uses BPlusTree internally.

 I don't plan to support all the really gnarly bits about hinting and node_ptrs.

 ### std::multimap interface

 Similar to above.

 ### std::saet interface

 Similar to above.

 ### Optimizing
 - Intranode searching is currently linear. switch to binary search.
 - work on the splitting algorithm. It still does more data copies/moves than really necessary.