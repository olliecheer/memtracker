#ifndef TRACE_H
#define TRACE_H

#include "common.h"

#include <string>
#include <map>
#include <optional>


struct FrameInfo {
    void* address;
    std::string symbol;
};

using StackInfo = std::vector<FrameInfo>;
using TraceForThread = std::map<TimeRange<TimeValue>, StackInfo>;

class TracePack {
public:
    TracePack();
    ~TracePack();
    TraceForThread get_trace(pid_t tid) const;
};


#endif
