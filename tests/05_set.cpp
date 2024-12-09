#include <bplusset.hpp>

#include <catch2/catch_all.hpp>

#include <vector>

using namespace BPT;


TEST_CASE("constructors", "[set]") {

    SECTION("simple constructor") {
        set<int> myset{};
        REQUIRE(myset.size() == 0);
    }

    SECTION("iterator constructor") {
        std::vector<int>vec = {1,2,3,4,5,5};

        auto myset = set<int>(vec.begin(), vec.end());
        REQUIRE(myset.size() == 5);
    }

    SECTION("init list constructor") {
        set<int> myset = {1,2,3,4,5,5,6};
        REQUIRE(myset.size() == 6);
    }

    SECTION("copy constructor") {
        set<int> other = {1,2,3,4,5,5,6};

        auto myset = set<int>(other);
        REQUIRE(myset.size() == 6);
        REQUIRE(other.size() == 6);

    }

    SECTION("move constructor") {
        set<int> other = {1,2,3,4,5,5,6};

        auto myset = set<int>(std::move(other));
        REQUIRE(myset.size() == 6);
        REQUIRE(other.size() == 0);

    }

}

TEST_CASE("assignment operators", "[set]") {
    SECTION("copy assignment") {
        set<int> other = {1,2,3,4,5,5,6};

        set<int> myset = { 20, 21, 22 };

        myset = other;
        REQUIRE(myset.size() == 6);
        REQUIRE(other.size() == 6);

    }

    SECTION("init list assignment") {
        set<int> myset = { 20, 21, 22 };

        myset = {1,2,3,4,5,5,6};
        REQUIRE(myset.size() == 6);
    }

    SECTION("move assignment") {
        set<int> other = {1,2,3,4,5,5,6};

        set<int> myset = { 20, 21, 22 };

        myset = std::move(other);
        REQUIRE(myset.size() == 6);
        REQUIRE(other.size() == 3);

    }

}

TEST_CASE("empty,size,insert", "[set]") {
    set<int> myset;
    REQUIRE(myset.empty() == true);
    REQUIRE(myset.size() == 0);

    {
        auto retval = myset.insert(200);
        REQUIRE(retval.second == true);
        REQUIRE(*(retval.first) == 200);
        REQUIRE(myset.empty() == false);
        REQUIRE(myset.size() == 1);

    }

    {
        auto retval = myset.insert(200);
        REQUIRE(retval.second == false);
        REQUIRE(*(retval.first) == 200);
    }

    { 
        auto it = myset.begin();
        REQUIRE(it != myset.end());
        REQUIRE(*it == 200);
        ++it;
        REQUIRE(it == myset.end());
    
    }

    for (int item : myset) {
        REQUIRE(item == 200);
    }

}