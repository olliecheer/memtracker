#ifndef FREE_MAP_H
#define FREE_MAP_H

#include "common.h"

#include <mutex>
#include <optional>
#include <map>

class FreeMap {
    std::mutex mtx_;
    using FreeAtSameTimeMap = std::map<TimeValue, size_t>;
    std::unordered_map<void*, FreeAtSameTimeMap> map_;
public:
    std::optional<TimeValue> get_pop(void* address, TimeValue const& tv_after);
    void insert(void* address, struct timeval const& free_tv);
};

#endif
