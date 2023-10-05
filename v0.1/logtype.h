//
// Created by pilot on 10/3/2023.
//

#ifndef UNTITLED_LOGTYPE_H
#define UNTITLED_LOGTYPE_H

#include <sys/time.h>
#include <cstddef>
#include <functional>

enum class LogType {
    FREE,
    MALLOC,
    CALLOC,
//    REALLOC,
};

struct Log {
    void* address;
    size_t size;
    struct timeval tv;
    LogType type;
};

template<>
class std::less<struct timeval> {
public:
    bool operator()(struct timeval const& a, struct timeval const& b) const {
        return a.tv_sec < b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec < b.tv_usec);
    }
};


#endif //UNTITLED_LOGTYPE_H
