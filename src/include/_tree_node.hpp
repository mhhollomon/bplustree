#include <cstddef>


enum TreeNodeType {
    InternalNode,
    LeafNode
};


template<typename K, typename V, std::size_t FO>
struct TreeNode {

    using key_type = K;
    using value_type = V;
    using value_wrapper_type = ValueWrapper<K, V>;

    constexpr static std::size_t fan_out = FO;
    constexpr static std::size_t key_limit = FO-1;

    using child_ptr_type = void *;

    std::array<key_type, key_limit> keys;
    std::array<child_ptr_type, fan_out> child_ptrs;
    TreeNode *parent = nullptr;   

    std::size_t num_keys = 0;
    TreeNodeType ntype = InternalNode;

    TreeNode(TreeNodeType tntype = InternalNode) : ntype(tntype) {
        for (std::size_t i = 0; i < fan_out; ++i) {
            child_ptrs[i] = nullptr;
        }
    }

    bool is_full() const { return (num_keys >= key_limit); }
    bool is_internal() const { return (ntype == InternalNode); } 

    key_type max_key() const {
        if (num_keys > 0) {
            return keys[num_keys - 1];
        } else {
            return key_type{};
        }
    }

    key_type min_key() const {
        if (num_keys > 0) {
            return keys[0];
        } else {
            return key_type{};
        }
    }

};
