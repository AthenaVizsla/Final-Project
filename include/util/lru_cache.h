// include/util/lru_cache.h
//
// YOU IMPLEMENT (this whole file; it is a template). Spec §4.2.
//
// A fixed-capacity cache that evicts the Least Recently Used entry.
// Both get() and put() must be O(1) (expected), so it is built from two
// structures that point at the same nodes:
//
//   std::unordered_map<Key, Node*>   finds a key's node in O(1)
//   a doubly linked list of Nodes    keeps recency order:
//        head_ = most recently used ... tail_ = least recently used
//
// Rules for this file:
//   * Write the doubly linked list yourself (Node with prev/next pointers,
//     new/delete inside this class only). Do NOT use std::list, std::deque
//     or std::forward_list (don't even #include their headers), or a
//     std::vector you shift around (spec §7.3).
//   * std::unordered_map is allowed (and expected) for the key -> Node* index.
//   * No leaks, no double frees: ASan runs on every test.

#pragma once
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class LruCache {
public:
    struct Stats {
        std::size_t hits = 0;        // get() found the key
        std::size_t misses = 0;      // get() did not find the key
        std::size_t evictions = 0;   // put() removed the LRU entry to make room
    };

    // A cache holding at most `capacity` entries. Capacity 0 is legal: such a
    // cache never stores anything (every get() is a miss).
    explicit LruCache(std::size_t capacity) : capacity_(capacity) {}

    // Frees every node.
    ~LruCache() { clear(); }

    // Copying a cache of raw-pointer nodes is not needed; forbid it.
    LruCache(const LruCache&) = delete;
    LruCache& operator=(const LruCache&) = delete;

    // Hit: marks the entry most recently used, counts a hit, and returns a
    //      pointer to its value (valid until the next put/erase/clear).
    // Miss: counts a miss and returns nullptr.
    const Value* get(const Key& key) {
        auto it = map_.find(key);
        if (it == map_.end()){
            ++stats_.misses;
            return nullptr;
        }
        ++stats_.hits;
        move_to_front(it->second);
        return &it->second->value;
    }

    // Key present: replaces its value and marks it most recently used (no
    //   eviction, no hit/miss counted).
    // Key absent: if the cache is full, evicts the least recently used entry
    //   (counting one eviction), then inserts the new entry as most recently
    //   used. With capacity 0, does nothing.
    void put(const Key& key, Value value) {
        // TODO
        // Clue (eviction): victim = tail_; unlink it; erase victim->key from
        // the map; THEN delete victim. Reading victim->key after delete is a
        // heap-use-after-free (ASan may show it as a SEGV inside hashtable
        // code: look for the first stack frame in lru_cache.h).
        // Clue: create nodes with  new Node{key, std::move(value), nullptr, nullptr}.
        (void)key;
        (void)value;
    }

    // True if the key is cached. Does NOT change recency or stats.
    bool contains(const Key& key) const {
        return map_.find(key) != map_.end();
    }

    // Removes the key if present (not an eviction). Returns whether it was.
    bool erase(const Key& key) {
        // TODO
        (void)key;
        return false;
    }

    // Removes every entry. Stats are kept.
    void clear() {
        // TODO: delete every node, empty the map, reset head_/tail_.
        // Clue: save n->next in a local BEFORE  delete n;  a loop like
        //   for (Node* n = head_; n; n = n->next) delete n;
        // reads n->next from freed memory.
    }

    std::size_t size() const noexcept { return map_.size(); }
    std::size_t capacity() const noexcept { return capacity_; }
    Stats       stats() const noexcept { return stats_; }

    // Keys from most to least recently used, by walking the list from head_.
    // Used by the tests to check your recency order.
    std::vector<Key> keys_mru_to_lru() const {
        std::vector<Key> out;
        for (Node* n = head_; n != nullptr; n = n->next) out.push_back(n->key);
        return out;
    }

private:
    struct Node {
        Key   key;
        Value value;
        Node* prev;
        Node* next;
    };

    // ===== YOUR HELPERS AND MEMBERS =====
    // Suggested helpers: unlink(Node*), push_front(Node*), move_to_front(Node*).
    // Clue for unlink(n): four cases. If n has a prev, prev->next skips n,
    // otherwise n was head_ and head_ moves to n->next. Likewise, if n has
    // a next, next->prev skips n, otherwise n was tail_ and tail_ moves to
    // n->prev. Unlinking the only node leaves head_ == tail_ == nullptr.
    // Spec Appendix B lists every pointer write.

    // detach n from the list and fix the head and or tail
    void unlink(Node* n){
        if (n->prev != nullptr){
            n->prev->next = n->next;
        }else {
            head_ = n->next;    // n was the head
        }
        if (n->next != nullptr){
            n->next->prev = n->prev;
        }else {
            tail_ = n->prev;    // n was the tail
        }
        n->prev = n->next = nullptr;
    }


    // insert a deatached node at the front
    void push_front(Node* n){
        n->prev = nullptr;
        n->next = head_;
        if (head_ != nullptr){
            head_->prev = n;
        }
        if (tail_ == nullptr){
            tail_ = n;
        }
    }

    // become head of list
    void move_to_front(Node* n){
        if (n == head_){
            return;
        }
        unlink(n);
        push_front(n);
    }


    std::unordered_map<Key, Node*, Hash> map_;
    Node*       head_ = nullptr;
    Node*       tail_ = nullptr;
    std::size_t capacity_;
    Stats       stats_;
};
