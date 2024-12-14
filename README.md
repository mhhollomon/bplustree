# bplustree

Implementation of a B+ Tree.

This works best with a large number of records and a large fan-out.

Requires C++20.

MIT license.

**Note** `main` branch is usually a work in progress. You will be much
better served using the latest release.

## Benchmark

The benchmark code in `examples/set-benchmark.cpp` was run and the following
results were obtained as an average of 5 runs.


|Structure        |build   |probe   |
|-----------------|--------|--------|
|std::set\<int>   |967ms|608ms|
|std::map<int,int>|956ms|587ms|
|BPT::set\<int>   |725ms|272ms|
|BPT::BPlusTree<int, int>|701ms|272ms|
|BPT::BPlusTree<int, int, 100>|764ms|210ms|

## Concurrency

This is not concurrency safe without additional locking.
The tree methods can be called without problems from different threads. However,
two threads may not call methods at the same time - unless they are both reads.

Caveat Scriptor

## Interface

See [B+ API](docs/api.md)

## Set
A replacement for `std::set` is available.

C.f. [Set documentation](docs/set.md)

## Using in a project

### Direct copy

This is a header only project. All that would be needed is to copy the contents
of the `src` directory along with its subdirectories to some suitable place
and point your compiler there as an include  directory.

### using CMake and CPM

An easier way, if you use cmake, is to use [CPM](https://github.com/cpm-cmake/CPM.cmake).

Something like the following :

```cmake
include(CPM)

...

CPMAddPackage("gh:mhhollomon/bplustree@0.2.0")

target_link_libraries(my_project PRIVATE bplustree)
```
You can control the creation of the tests by setting
`BPLUSTREE_BUILD_TESTS`. it is on by default.

## TO DO
 
### Removing elements

There will be a `rebuild` method that will take care of reclaiming space.

### Concurency

Use Optimistic Lock Coupling to allow concurrency.

(c.f. http://sites.computer.org/debull/A19mar/p73.pdf)

### std::map interface
This will probably be a new separate class that uses BPlusTree internally.

I don't plan to support all the really gnarly bits about hinting and node_ptrs.

### std::multimap interface

Similar to above.

### Optimizing
- work on the splitting algorithm. It still does more data copies/moves than really necessary.

### Iterators

Provide reverse iterators.

## Technology
- [Catch2](https://github.com/catchorg/Catch2) for testing framework.
- [CPM](https://github.com/cpm-cmake/CPM.cmake) for managing dependencies.