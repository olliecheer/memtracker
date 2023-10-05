#ifndef ALLOC_MAP_H
#define ALLOC_MAP_H

#include "common.h"
#include <list>

template<typename TimeValueType>
struct ResolvedMalloc {
    void* address;
    size_t size;
    // alloc_tv should be checked as key
    TimeValueType free_tv;
    LogType type;
};

template<typename TimeValueType>
struct UnresolvedMalloc {
    void* address;
    size_t size;
    // alloc_tv should be checked as key
    LogType type;
};

template<typename TimeValueType>
using ResolvedMallocVec = std::vector<ResolvedMalloc<TimeValueType>>;

template<typename TimeValueType>
using UnresolvedMallocVec = std::vector<UnresolvedMalloc<TimeValueType>>;

template<typename TimeValueStorageType=TimeValue>
class ResolvedMallocMap {
public:
    template<typename TimeValueType>
    void insert(void* address, size_t size, LogType type, TimeValueType const&alloc_tv, TimeValueType const&free_tv);

    template<typename TimeValueType>
    ResolvedMallocVec<TimeValueType> get_by_alloc_tv(TimeRange<TimeValueType> const&range) const;

    template<typename TimeValueType>
    ResolvedMallocVec<TimeValueType> get_by_free_tv(TimeRange<TimeValueType> const&range) const;

    template<typename TimeValueType>
    ResolvedMallocVec<TimeValueType> get_all(TimeRange<TimeValueType> const&range) const;
};

template<typename TimeValueStorageType=TimeValue>
class UnresolvedMallocMap {
public:
    template<typename TimeValueType>
    void insert(void* address, size_t size, LogType type, TimeValueType const&alloc_tv);

    template<typename TimeValueType>
    UnresolvedMallocVec<TimeValueType> get_by_alloc_tv(TimeRange<TimeValueType> const&range) const;

    template<typename TimeValueType>
    UnresolvedMallocVec<TimeValueType> get_by_free_tv(TimeRange<TimeValueType> const&range) const;

    template<typename TimeValueType>
    UnresolvedMallocVec<TimeValueType> get_all(TimeRange<TimeValueType> const&range) const;
};


// used per thread
class RawMallocList {
    pid_t tid_;
    std::list<Log> logs;
public:
    explicit RawMallocList(pid_t tid);
    void push_back(Log const& log);
    Log get_pop();
    bool empty() const;
    pid_t get_tid() const;
};

#endif
