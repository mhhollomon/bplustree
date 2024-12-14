#include "bplusset.hpp"

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std::chrono;


constexpr int key_count = 2'000'000;
constexpr int probe_count = 1'000'000;

template<class T, char MODE >
void time_it(T &object, std::string const &name, std::vector<int> const &load_keys, std::vector<int> const &probe_keys) {

    // Load
    auto start = high_resolution_clock::now();
    for (int i : load_keys) {
        if constexpr (MODE == 'P') {
            object.insert({i, i});
        } else if constexpr (MODE == 'T') {
            object.insert(i, i);        
        } else {
            object.insert(i);
        }
    }
    auto end = high_resolution_clock::now();
    auto load_duration = duration_cast<microseconds>(end - start).count();

    // probe
    int count = 0;
    start = high_resolution_clock::now();
    for (int i = 0; i < probe_count; ++i) {
        auto r = object.contains(probe_keys[i % key_count ]);
        count += r;
    }

    end = high_resolution_clock::now();
    auto probe_duration = duration_cast<microseconds>(end - start).count();

    if (count != probe_count) {
        std::cout << "Missing probes : " << count << std::endl;
    }

    std::cout << '"' << name << "\"," << load_duration << "," << probe_duration << std::endl;


}

int main() {

    std::vector<int> keys;

    keys.reserve(key_count);

    for (int i = 0; i < key_count; ++i) {
        keys.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());

    // Shuffle the array
    std::shuffle(keys.begin(), keys.end(), g);

    auto probe_keys = keys;
    std::shuffle(probe_keys.begin(), probe_keys.end(), g);



    BPT::BPlusTree<int, int> bpt_tree;
    BPT::BPlusTree<int, int, 100> bpt_tree_big;
    BPT::set<int> bpt_set;
    std::set<int> std_set;
    std::map<int, int> std_map;

    time_it<BPT::set<int>, 'O'>(bpt_set, "BPT::set<int>", keys, probe_keys);
    time_it<std::set<int>, 'O'>(std_set, "std::set<int>", keys, probe_keys);
    time_it<std::map<int, int>, 'P'>(std_map, "std::map<int,int>", keys, probe_keys);
    time_it<BPT::BPlusTree<int, int>, 'T'>(bpt_tree, "BPT::BPlusTree<int, int>", keys, probe_keys);
    time_it<BPT::BPlusTree<int, int, 100>, 'T'>(bpt_tree_big, "BPT::BPlusTree<int, int, 100>", keys, probe_keys);

    return 0;

}
