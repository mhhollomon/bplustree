#include "bplusset.hpp"

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std::chrono;


constexpr int key_count = 1'000'000;
constexpr int probe_count = 500'000;

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

    BPT::set<int> bpt_set;
    std::set<int> std_set;
    std::map<int, int> std_map;

    for (int i : keys) {
        bpt_set.insert(i);
        std_set.insert(i);
        std_map.insert({i,i});
    }

    // Reshuffle
    std::shuffle(keys.begin(), keys.end(), g); 

    int size = 0;

    // BPT timing
    auto start = high_resolution_clock::now();

    for (int i = 0; i < probe_count; ++i) {
        auto r = bpt_set.contains(keys[i % key_count ]);
        size += r;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << "BPT::set Duration = " << duration << "\n";

    start = high_resolution_clock::now();

    // STD timing
    for (int i = 0; i < probe_count; ++i) {
        auto r = std_set.contains(keys[i % key_count ]);
        size += r;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    std::cout << "std::set Duration = " << duration << "\n";

    start = high_resolution_clock::now();

    for (int i = 0; i < probe_count; ++i) {
        auto r = std_map.contains(keys[i % key_count ]);
        size += r;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    std::cout << "std::map Duration = " << duration << "\n";

    if (size != (probe_count * 3)) {
        std::cout << "The compiler cheated : " << size << std::endl;
    }

    return 0;


}
