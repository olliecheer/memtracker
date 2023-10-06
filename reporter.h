#ifndef REPORTER_H
#define REPORTER_H

// report for single thread or multiple threads

#include "common.h"
#include "plot.h"
#include "trace.h"
#include "alloc_map.h"

#include <mutex>

//#include <list>

struct MallocSymbolized {
    LogType log;
    std::optional<StackInfo> stack;
};

class Reporter {
public:
    PlotData alloc_distribution(uint32_t sum_interval);
    PlotData memory_occupy_distribution(uint32_t sum_interval);
    PlotData leak_distrubution(uint32_t sum_interval);

    void debug_print() const;

    std::vector<MallocSymbolized> leak_symbolize(TraceForThread const& traces);
};


class PlainReporter {
    std::map<pid_t, MallocMap<ResolvedMalloc>> all_resolved;
    std::mutex all_resolved_mtx;
    std::map<pid_t, MallocMap<UnresolvedMalloc>> all_unresolved;
    std::mutex all_unresolved_mtx;
public:
    bool load(std::string path_prefix, const std::vector<pid_t>& tids);
    void report();
};



#endif
