#include <bplustree.hpp>
#include <treeprinter.hpp>

#include <catch2/catch_all.hpp>

#include <iostream>
#include <queue>

#include <vector>
#include <algorithm>
#include <random>

#include <chrono>
using namespace std::chrono;

using namespace BPT;

const int num_inserts = 100'000;
const int num_removes = 100'000;
const int num_range_expand = 1'000;

TEST_CASE("bulk", "[bulk]") {
    std::vector<int> keys;
    int total_inserts = num_inserts;
    int total_removes = 0;

    BPlusTree<int, int, 10> tree;
    auto tp = tree_printer(tree);
    keys.reserve(num_inserts);

    //std::cout << "Here 1\n";

    for (int i = 0; i < num_inserts; ++i) {
        keys.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());

    // inclusive
    std::uniform_int_distribution<int> shuffle_dist(0, num_inserts-1);

    // Shuffle the array
    std::shuffle(keys.begin(), keys.end(), g); 

    //std::cout << "Here 2\n";

    auto start = high_resolution_clock::now();

    // load a bunch
    for (int i : keys) {
        //std::cout << "insert : " << i << "--------------------------\n";
        REQUIRE(tree.insert(i, i*10).second == true);
        //tp.print();
    }

    //std::cout << "BEGIN ------------------------------------------------------------\n";

    //tree_printer(tree).print();
    //std::cout << "END   ------------------------------------------------------------\n";

    //std::cout << "Here 3\n";

    std::uniform_int_distribution<int> bulk_distr(0, num_inserts+num_range_expand-1);

    // randomly remove and insert
    for(int i = 0; i < num_removes; ++i) {

        int key = bulk_distr(g);
        if (tree.insert(key, key*100).second) {
            total_inserts += 1;
        }

        key = bulk_distr(g);
        if (tree.remove(key)) {
            total_removes += 1 ;
        }
    }

    auto stop = high_resolution_clock::now();

    std::cout << "Here 4\n";


    //tree_printer(tree).print();

    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << "DURATION ---- " << duration << "\n";
    std::cout << "inserts = " << total_inserts << "\n";
    std::cout << "removes = " << total_removes << "\n";


}