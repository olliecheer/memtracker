#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <cstdint>
#include <algorithm>
#include <limits>



template <>
class std::less<struct timeval> {
public:
    bool operator() (struct timeval const&a, struct timeval const&b) const {
        return a.tv_sec < b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec < b.tv_usec);
    }
};

template<>
class std::equal_to<struct timeval> {
public:
    bool operator() (struct timeval const&a, struct timeval const&b) const {
        return a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec;
    }
};

template<>
class std::less_equal<struct timeval> {
public:
    bool operator() (struct timeval const&a, struct timeval const&b) const {
        return std::less<struct timeval>{}(a, b) || std::equal_to<struct timeval>{}(a, b);
    }
};



using TimeValue = struct timeval;
using TimeValueLite = decltype(TimeValue::tv_sec);

const TimeValue TimeValueMax = TimeValue {
    std::numeric_limits<decltype(TimeValue::tv_sec)>::max(),
    std::numeric_limits<decltype(TimeValue::tv_usec)>::max()
};

const TimeValue TimeValueMin = TimeValue {
        std::numeric_limits<decltype(TimeValue::tv_sec)>::min(),
        std::numeric_limits<decltype(TimeValue::tv_usec)>::min()
};

template<typename To, typename From>
class convert_to {
public:
    To operator() (From const&) const;
};

template<>
class convert_to<TimeValue, TimeValueLite> {
public:
    TimeValue operator() (TimeValueLite const&from) const {
        return {from, 0};
    }
};

template<>
class convert_to<TimeValueLite, TimeValue> {
public:
    TimeValueLite operator() (TimeValue const&from) const {
        return from.tv_sec;
    }
};

template<typename TimeValueType>
struct TimeRange {
    TimeValueType begin;
    TimeValueType end;
    bool operator<(TimeValueType const& other) const {
        using less = std::less<struct timeval>;
        using equal_to = std::equal_to<struct timeval>;
        return less{}(begin, other.begin) || (equal_to{}(begin, other.begin) && less{}(end, other.end));
    }
    bool operator==(TimeValueType const& other) const {
        using equal_to = std::equal_to<struct timeval>;
        return equal_to{}(begin, other.begin) && equal_to{}(end, other.end);
    }

    /* struct timeval begin; */
    /* struct timeval end; */
    /* bool operator<(struct TimeRange const& other) const { */
    /*     using less = std::less<struct timeval>; */
    /*     using equal_to = std::equal_to<struct timeval>; */
    /*     return less{}(begin, other.begin) || (equal_to{}(begin, other.begin) && less{}(end, other.end)); */
    /* } */
    /* bool operator==(struct TimeRange const& other) const { */
    /*     using equal_to = std::equal_to<struct timeval>; */
    /*     return equal_to{}(begin, other.begin) && equal_to{}(end, other.end); */
    /* } */
};


enum class LogType {
    FREE,
    MALLOC,
    CALLOC,
};

struct Log {
    void* address;
    size_t size;
    TimeValue tv;
    LogType type;
};

#endif
