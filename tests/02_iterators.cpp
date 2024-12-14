#include <bplustree.hpp>
#include <treeprinter.hpp>

#include <catch2/catch_all.hpp>

using namespace BPT;

TEST_CASE("iterator", "[iterator]") {
    BPlusTree<int, int, 4> tree;

    tree.insert(1,1);
    tree.insert(20,20);
    tree.insert(5,5);
    tree.insert(100,100);
    tree.insert(10,10);
    tree.insert(11,11);
    tree.insert(12,12);

    SECTION("forward") {

        auto iter = tree.begin();
        REQUIRE(iter->value == 1);
        ++iter;
        REQUIRE(iter->value == 5);
        ++iter;
        REQUIRE(iter->value == 10);
        ++iter;
        REQUIRE(iter->value == 11);
        ++iter;
        REQUIRE(iter->value == 12);

        REQUIRE(iter != tree.end());
        REQUIRE(iter != tree.end());

    }

    SECTION("reverse") {

        auto iter = tree.rbegin();
        REQUIRE(iter->value == 100);
        ++iter;
        REQUIRE(iter->value == 20);
        ++iter;
        REQUIRE(iter->value == 12);
        ++iter;
        REQUIRE(iter->value == 11);
        ++iter;
        REQUIRE(iter->value == 10);

        REQUIRE(iter != tree.rend());
        REQUIRE(iter != tree.rend());

    }


}