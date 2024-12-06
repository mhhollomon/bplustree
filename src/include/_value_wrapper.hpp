
template<class K, class V>
struct ValueWrapper {
    ValueWrapper *previous = nullptr;
    ValueWrapper *next = nullptr;
    K key;
    V value;
    bool deleted = false;

    ValueWrapper(K inkey, V invalue) : key(inkey), value(invalue) {}
};