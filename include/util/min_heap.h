// include/util/min_heap.h
//
// YOU IMPLEMENT (this whole file; it is a template, so the code lives in the
// header). Spec §4.1.
//
// A binary MIN-heap stored in an array (a std::vector), plus top_k(), which
// uses a size-k min-heap to pick the k largest items in O(n log k).
//
// Rules for this file:
//   * Write sift-up and sift-down yourself.
//   * Do NOT use std::priority_queue, std::make_heap / push_heap / pop_heap /
//     sort_heap, std::sort / stable_sort / partial_sort / nth_element, or
//     std::set / multiset / map / multimap (full list: spec §7.3).
//   * std::vector is fine as the storage array; std::swap, std::reverse and
//     std::pair are fine too.
//
// `Less` is a "less-than" function object, exactly like the one std::sort
// takes: less(a, b) is true when a should come before b. The heap keeps the
// SMALLEST element (by less) at the top.

#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T, typename Less = std::less<T>>
class MinHeap {
public:
    explicit MinHeap(Less less = Less()) : less_(std::move(less)) {}

    // Adds value. O(log n).
    void push(T value) {
        // TODO: append to data_, then sift the new element up.
        // Clue: compare with less_(a, b), never a < b. Some element types
        // (e.g. the provided memory_store.cpp's Scored struct) have no
        // operator<, and a min-heap of std::greater must still work.
        data_.push_back(std::move(value));
        sift_up(data_.size() - 1);
    }

    // The smallest element. Throws std::out_of_range if empty. O(1).
    const T& top() const {
        if (data_.empty()){
            throw std::out_of_range("MinHeap::top not implemented");
        }
        return data_[0];
    }

    // Removes and returns the smallest element. Throws std::out_of_range if
    // empty. O(log n).
    T pop() {
        if (data_.empty()){
            throw std::out_of_range("MinHeap::pop on empty heap");
        }
        T result = std::move(data_[0]);
        if (data_.size() > 1){
            data_[0] = std::move(data_.back());
        }
        data_.pop_back();
        if (!data_.empty()){
            sift_down(0);
        }
        return result;
    }

    std::size_t size() const noexcept { return data_.size(); }
    bool        empty() const noexcept { return data_.empty(); }
    void        clear() noexcept { data_.clear(); }

private:
    // ===== YOUR HELPERS AND MEMBERS =====
    // Children of index i are 2i+1 and 2i+2; its parent is (i-1)/2.
    // Suggested: void sift_up(std::size_t i);  void sift_down(std::size_t i);
    // Clue: use std::size_t (not int) for indices, or -Wsign-compare warns
    // every time you compare with data_.size(). Careful: (i-1)/2 with i == 0
    // underflows, so sift_up must stop at i == 0.
    // Clue: in sift_down, the right child may not exist: check
    // 2*i+2 < data_.size() before reading it. Swap with the SMALLER child.


    // move data_[i] up while its smaller than its parent
    void sift_up(std::size_t i){    
        while(i > 0){
            std::size_t parent = (i - 1) / 2;
            if(!less_(data[i], data_[parent])){
                break;
            }
            std::swap(data_[i], data_[parent]);
            i = parent;
        }
    }

    // move data_[i] down while it is larger than its smaller child
    void sift_down(std::size_t i){
        const std::size_t n = data_.size();
        for(;;){
            std::size_t left = 2 * i + 1;
            if(left >= n){
                break;
            }
            std::size_t smallest = left;
            std::size_t right = left + 1;
            if (right < n && less_(data_[right], data_[left])){
                smallest = right;
            }
            if(!less_(data_[smallest], data_[i])){
                break;
            }
            std::swap(data_[i], data_[smallest]);
            i = smallest;
        }
    }

    std::vector<T> data_;
    Less           less_;
};

// Returns the k LARGEST items of `items` (by `less`), largest first.
//   * k == 0 or items empty -> empty result
//   * k >= items.size()     -> every item, largest first
// Must keep a MinHeap of at most k items (the k largest seen so far, with
// the smallest of them on top), so it runs in O(n log k) time and O(k) extra
// space. Items that compare equal may come out in any order.
template <typename T, typename Less = std::less<T>>
std::vector<T> top_k(const std::vector<T>& items, std::size_t k, Less less = Less()) {
    if(k == 0 || items.empty()){
        return {};
    }
    MinHeap<T, Less> heap(less);
    for(const T& item : items){
        if (heap.size() < k){
            heap.push(item);
        } else if (less(heap.top(), item)){
            heap.pop();
            heap.push(item);
        }
    }
    std::vector<T> out;
    out.reserve(heap.size());
    while (!heap.empty()){
        out.push_back(heap.pop());
    }
    std::reverse(out.begin(), out.end());
    return out;
}
