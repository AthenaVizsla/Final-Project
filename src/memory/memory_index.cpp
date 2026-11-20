// src/memory/memory_index.cpp — YOU IMPLEMENT. Spec §4.3.

#include "memory/memory_index.h"

MemoryIndex::~MemoryIndex() {
    clear();
}

bool MemoryIndex::insert(MemoryRecord rec) {
    Node** link = &root_;
    while (*link != nullptr){
        Node* cur = *link;
        if (rec.id == cur->rec.id){
            return false;   // duplicate
        }
        link = (rec.id < cur->rec.id) ? &cur->left : &cur->right;
    }
    Node* node = new Node;
    node->rec = std::move(rec);
    *link = node;
    ++count_;
    return true;
}

const MemoryRecord* MemoryIndex::find(std::uint64_t id) const {
    const Node* cur = root_;
    while (cur != nullptr){
        if (id == cur->rec.id){
            return &cur->rec;
        }
        cur = (id < cur->rec.id) ? cur->left : cur->right;
    }
    return nullptr;
}

bool MemoryIndex::remove(std::uint64_t id) {
    Node** link = &root_;
    while (*link != nullptr && (*link)->rec.id != id){
        link = (id < (*link)->rec.id) ? &(*link)->left : &(*link)->right;
    }
    if (*link == nullptr){
        return false;       // not found
    }

    Node* victim = *link;
    if (victim->left == nullptr){       // leaf or only a right child
        *link = victim->right;
        delete victim;
    } else if (victim->right == nullptr){   //only a left child
        *link = victim->left;
        delete victim;
    } else{
        Node** succ_link = &victim->right;
        while ((*succ_link)->left != nullptr){
            succ_link = &(*succ_link)->left;
        }
        Node* succ = *succ_link;
        *succ_link = succ->right;
        victim->rec = std::move(succ->rec);
        delete succ;
    }
    --count_;
    return true;
}

std::size_t MemoryIndex::size() const noexcept { return count_; }

std::size_t MemoryIndex::height() const {
    std::size_t best = 0;
    std::vector<std::pair<const Node*, std::size_t>> stack;
    if (root_ != nullptr){
        stack.emplace_back(root_, 1);
    }
    while (!stack.empty()){
        auto [node, depth] = stack.back();
        stack.pop_back();
        if (depth > best){
            best = depth;
        } if(node->left != nullptr){
            stack.emplace_back(node->left, depth + 1);
        } if(node->right != nullptr){
            stack.emplace_back(node->right, depth + 1);
        }
    }
    return best;
}

void MemoryIndex::for_each_in_order(const std::function<void(const MemoryRecord&)>& fn) const {
    std::vector<const Node*> stack;
    const Node* cur = root_;
    while (cur != nullptr || !stack.empty()){
        while (cur != nullptr){
            stack.push_back(cur);
            cur = cur->left;
        }
        cur = stack.back();
        stack.pop_back();
        fn(cur->rec);
        cur = cur->right;
    }
}

void MemoryIndex::clear() {
    std::vector<Node*> stack;
    if(root_ != nullptr){
        stack.push_back(root_);
    }
    while (!stack.empty()){
        Node* n = stack.back();
        stack.pop_back();
        if (n->left != nullptr){
            stack.push_back(n->left);
        } if( n->right != nullptr){
            stack.push_back(n->right);
        }
        delete n;
    }
    root_ = nullptr;
    count_ = 0;
}
