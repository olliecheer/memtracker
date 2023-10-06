#ifndef ALLOC_MAP_H
#define ALLOC_MAP_H

#include "common.h"
#include <list>
#include <map>
#include <assert.h>

//template<typename TimeValueType = TimeValue>
struct ResolvedMalloc {
    void* address;
    size_t size;
    // alloc_tv should be checked as key
    TimeValue free_tv;
    LogType type;
};

//template<typename TimeValueType = TimeValue>
struct UnresolvedMalloc {
    void* address;
    size_t size;
    // alloc_tv should be checked as key
    LogType type;
};

template<typename TimeValueType>
using ResolvedMallocVec = std::list<ResolvedMalloc<TimeValueType>>;

template<typename TimeValueType>
using UnresolvedMallocVec = std::list<UnresolvedMalloc<TimeValueType>>;

template<typename MallocResolveType, typename std::enable_if<std::is_same_v<MallocResolveType, ResolvedMalloc> || std::is_same_v<MallocResolveType, UnresolvedMalloc>>>
class MallocMap {
    using AtSameMicrosec = std::list<MallocResolveType>;
    using AllMalloc = std::map<TimeValue, AtSameMicrosec>;
    AllMalloc map_;
public:
    template<typename std::enable_if<std::is_same_v<MallocResolveType, ResolvedMalloc>>>
    void insert(void* address, size_t size, LogType type, TimeValue const& alloc_tv, TimeValue const& free_tv) {
        auto match_tv = map_.find(alloc_tv);
        if (match_tv == map_.end()) {
            auto insert_res = map_.insert({alloc_tv, {}});
            assert(insert_res.second);
            match_tv = insert_res.first;
        }
        match_tv->second.emplace_back(MallocResolveType{address, size, free_tv, type});
    }

    template<typename std::enable_if<std::is_same_v<MallocResolveType, UnresolvedMalloc>>>
    void insert(void* address, size_t size, LogType type, TimeValue const& alloc_tv) {
        auto match_tv = map_.find(alloc_tv);
        if (match_tv == map_.end()) {
            auto insert_res = map_.insert({alloc_tv, {}});
            assert(insert_res.second);
            match_tv = insert_res.first;
        }
        match_tv->second.emplace_back(MallocResolveType{address, size, type});
    }

    using MallocResolveTypeVec = std::list<MallocResolveType>;

    MallocResolveTypeVec get_by_alloc_tv(TimeRange<TimeValue> const& range) const {
        assert(std::less_equal<TimeValue>{}(range.begin, range.end));
        TimeRange<TimeValue> required_range = range;
        auto low_iter = std::upper_bound(map_.begin(), map_.end(), required_range.begin);
        auto high_iter = std::upper_bound(map_.begin(), map_.end(), required_range.end);

        if(low_iter == map_.end() || high_iter == map_.end())
            return {};
        if(low_iter != map_.begin())
            low_iter--;

        MallocResolveTypeVec res;
        for(auto it = low_iter; it != high_iter; it++) {
            for(auto& alloc_element : it->second) {
                res.push_back(alloc_element);
            }
        }
        return res;
    }

    MallocResolveTypeVec get_all() const {
        TimeRange<TimeValue> required_range {TimeValueMin, TimeValueMax};
        return get_by_alloc_tv(required_range);
    }
};

// used per thread
class RawMallocList {
    pid_t tid_;
    std::list<Log> logs;
public:
    explicit RawMallocList(pid_t tid)
    : tid_(tid) {}
    void push_back(Log const& log) {
        logs.push_back(log);
    }
    Log get_pop() {
        Log res = logs.front();
        logs.pop_front();
        return res;
    }
    bool empty() const {
        return logs.empty();
    }
    pid_t get_tid() const {
        return tid_;
    }
};

#endif
