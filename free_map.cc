#include "free_map.h"

void FreeMap::insert(void *address, const struct timeval &free_tv) {
    std::scoped_lock lk(mtx_);
    map_[address][free_tv]++;
}

std::optional<TimeValue> FreeMap::get_pop(void *address, const TimeValue &tv_after) {
    std::scoped_lock lk(mtx_);
    auto match_addr = map_.find(address);
    if(match_addr == map_.end())
        return {};

    auto match_tv = std::upper_bound(match_addr->second.begin(), match_addr->second.end(), tv_after, [](TimeValue const& tv_after, const auto it) {
        return std::less<TimeValue>{} (tv_after, it.first);
    });

    if(match_tv == match_addr->second.end())
        return {};

    TimeValue res = match_tv->first;
    match_tv->second--;

    if(!match_tv->second)
        match_addr->second.erase(match_tv);

    if(match_addr->second.empty())
        map_.erase(match_addr);

    return res;
}
