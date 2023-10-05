#ifndef REPORTER_H
#define REPORTER_H

// report for single thread or multiple threads

#include "common.h"
#include "plot.h"
#include "trace.h"

struct MallocSymbolized {
    LogType log;
    std::optional<StackInfo> stack;
};

class Reporter {
public:
    PlotGraph alloc_distribution(uint32_t sum_interval);
    PlotGraph memory_occupy_distribution(uint32_t sum_interval);
    PlotGraph leak_distrubution(uint32_t sum_interval);

    std::vector<MallocSymbolized> leak_symbolize(TraceForThread const& traces);
};

#endif
