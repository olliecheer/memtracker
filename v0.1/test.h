//
// Created by pilot on 10/4/2023.
//

#ifndef UNTITLED_TEST_H
#define UNTITLED_TEST_H

#include "logtype.h"

#include <stddef.h>
#include <sys/types.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <optional>
#include <vector>
#include <mutex>

struct TimeRange {
    struct timeval begin;
    struct timeval end;
};

template<>
class std::less<TimeRange> {
public:
    bool operator()(struct TimeRange const& a, struct TimeRange const& b) const {
        return std::less<struct timeval>{}(a.begin, b.begin) ||
                (a.begin.tv_sec == b.begin.tv_sec && a.begin.tv_usec == b.begin.tv_usec &&
                    std::less<struct timeval>{}(a.end, b.end));
    }
};

class SliceFile{
public:
    SliceFile(std::string directory, std::string prefix);
    ~SliceFile();
    ssize_t read(char* buf, size_t n);
    ssize_t write(char* buf, size_t n);
    Log read_one();
    void write_one(Log const& log);
    bool eof();
};

struct ResolvedMalloc {
    void* address;
    size_t size;
    struct timeval free_tv;
    LogType type;
};

struct UnresolvedMalloc {
    void* address;
    size_t size;
    LogType type;
};

using ResolvedMallocVec = std::vector<ResolvedMalloc>;
using UnresolvedMallocVec = std::vector<UnresolvedMalloc>;

class ResolvedMallocMap {
public:
    void insert(void* address, size_t size, LogType type, struct timeval const&alloc_tv, struct timeval const&free_tv);

    ResolvedMallocVec get_by_alloc_tv(TimeRange range) const;
    ResolvedMallocVec get_by_free_tv(TimeRange range) const;
    ResolvedMallocVec get_all() const;
};

class UnresolvedMallocMap {
public:
    void insert(void* address, size_t size, LogType type, struct timeval const&alloc_tv);

    UnresolvedMallocVec get_by_alloc_tv(TimeRange range) const;
    UnresolvedMallocVec get_by_free_tv(TimeRange range) const;
    UnresolvedMallocVec get_all() const;
};

/* using ThreadMallocInfo = std:: */
struct MallocData {
    pid_t tid;
    ResolvedMallocMap resolved;
    UnresolvedMallocMap unresolved;
};

using MallocDataVec = std::vector<MallocData>;


class RawMallocList {
    pid_t tid_;
    std::list<Log> logs;
public:
    RawMallocList(pid_t tid)
        :tid_(tid) {}

    void push_back(Log const& log) {
        logs.push_back(log);
    }

    bool empty() const {return logs.empty();}

    Log get_pop() {
        Log res = logs.front();
        logs.pop_front();
        return res;
    }

    pid_t get_tid() const {return tid_;}
};

using RawMallocThreads = std::list<RawMallocList>;


/* using RawMallocList = std::pair<pid_t, std::list<Log>>; */
/* using RawMallocVec = std::vector<std::pair<pid_t, RawMallocForThread>>; */
//using RawMallocMap = std::map<


class FreeMap {
    std::mutex mtx_;
    std::unordered_map<void*, std::map<struct timeval, size_t>> map_;
public:
    std::optional<struct timeval> get_pop(void* address, struct timeval const&tv_after) {
        std::scoped_lock lk(mtx_);
        auto match_addr = map_.find(address);
        if(match_addr == map_.end())
            return {};
        auto match_tv = std::upper_bound(match_addr->second.begin(), match_addr->second.end(), tv_after);
        if(match_tv == match_addr->second.end())
            return {};

        struct timeval res = match_tv->first;

        match_tv->second--;
        if(!match_tv->second)
            match_addr->second.erase(match_tv);
        if(match_addr->second.empty())
            map_.erase(match_addr);

        return res;
    }

    void insert(void* address, struct timeval const& free_tv) {
        std::scoped_lock lk(mtx_);
        map_[address][free_tv]++;
    }
};



struct FrameInfo {
    void* address;
    std::string symbol;
};
using StackInfo = std::vector<FrameInfo>;
using TraceForThread = std::map<struct timeval, StackInfo>;
using Traces = std::map<pid_t, TraceForThread>;


struct MallocSymbolized {
    LogType log;
    std::optional<StackInfo> stack;
};


struct PlotUnit {
    int64_t x_axis;
    int64_t y_axis;
};

struct PlotGraph {
    std::vector<PlotUnit> data;
    std::string x_name;
    std::string x_unit;
    std::string y_name;
    std::string y_unit;
};

class Reporter {
public:
    PlotGraph alloc_distribution(uint32_t sum_interval);
    PlotGraph memory_occupy_distribution(uint32_t sum_interval);
    PlotGraph leak_distrubution(uint32_t sum_interval);

    std::vector<MallocSymbolized> leak_symbolize(Traces const& traces);
};


#endif //UNTITLED_TEST_H
