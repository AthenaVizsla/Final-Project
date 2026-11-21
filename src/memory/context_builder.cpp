// src/memory/context_builder.cpp — YOU IMPLEMENT. Spec §4.4.

#include "memory/context_builder.h"
#include "util/min_heap.h"

#include <utility>

ContextPlan select_context(const std::vector<ContextItem>& items,
                           std::size_t budget, std::size_t keep_recent) {

    const std::size_t n = items.size();

    std::size_t total = 0;
    for (const ContextItem& it : items){
        total += it.tokens;
    }

    std::vector<bool> keep(n, true);

    if (total > budget){
        const std::size_t recent_start = (keep_recent >= n) ? 0 : n - keep_recent;

        MinHeap<std::pair<int, std::size_t>> heap;
        for (std::size_t i = 0; i < recent_start; ++i){
            const ContextItem& it = items[i];
            if (it.role == Role::System || it.pinned){
                continue;
            }
            heap.push({drop_rank(it.role), i});
        }

        while (total > budget && !heap.empty()){
            std::size_t idx = heap.pop().second;
            keep[idx] = false;
            total -= items[idx].tokens;
        }
    }

    ContextPlan plan;
    for (std::size_t i = 0; i < n; ++i){
        if (keep[i]){
            plan.keep.push_back(i);
        }
    }
    plan.total_tokens = total;
    plan.over_budget = total > budget;
    return plan;
}
