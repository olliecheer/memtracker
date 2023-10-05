//
// Created by pilot on 10/3/2023.
//

#ifndef UNTITLED_DATA_PROCESS_H
#define UNTITLED_DATA_PROCESS_H

#include "logtype.h"
#include <unordered_set>
#include <stddef.h>

class TimeRange {
    struct timeval start_;
    struct timeval end_;
public:
    struct timeval const& start() const {
        return start_;
    }
    struct timeval const& end() const {
        return end_;
    }

    void tunning_start(long delta_sec) {
        start_.tv_sec += delta_sec;
    }

    void tunning_end(long delta_sec) {
        end_.tv_sec += delta_sec;
    }
};

class ConsumedFree {
private:
    struct FreeKey {
        struct timeval tv;
        void* address;
        bool operator==(const FreeKey &other) const {
            return tv.tv_sec == other.tv.tv_sec
            && tv.tv_usec == other.tv.tv_usec
            && address == other.address;
        }
    };

    struct FreeKeyHasher {
        std::size_t operator()(const FreeKey& k) const {
            return (std::hash<decltype(k.tv.tv_usec)>()(k.tv.tv_usec) << 1)
                    ^ (std::hash<decltype(k.tv.tv_sec)>()(k.tv.tv_sec) << 1)
                    ^ (std::hash<decltype(k.address)>()(k.address) << 1);
        }
    };

    std::unordered_set<FreeKey, FreeKeyHasher> st;

public:
    void add(struct timeval tv, void* address) {
        st.insert(FreeKey{tv, address});
    }

    bool exist(struct timeval tv, void* address) {
        return st.find(FreeKey{tv, address}) != st.end();
    }
};

class AnalyzeEngine {
public:

#
};


#endif //UNTITLED_DATA_PROCESS_H
