//
// Created by pilot on 10/2/2023.
//

//#include <stdint.h>
//#include <stddef.h>
#include <dlfcn.h>
#include "logtype.h"
#include "shmbuf.h"

extern "C" {
    void* malloc(size_t size);
    void* calloc(size_t nmemb,size_t size);
    void* realloc(void *ptr, size_t size);
    void free(void* ptr);
};


using MallocType = void* (size_t);
using CallocType = void* (size_t, size_t);
using ReallocType = void* (void*, size_t);
using FreeType = void (void*);

MallocType *original_malloc = nullptr;
CallocType *original_calloc = nullptr;
ReallocType *original_realloc = nullptr;
FreeType *original_free = nullptr;


const char* LOG_FILENAME_PREFIX = "/tmp/memtracker";
const size_t SIZE_PER_FILE = 4096 * 4;
static thread_local ShmBuf shmbuf(LOG_FILENAME_PREFIX, SIZE_PER_FILE);

void do_record(Log const& log) {
    shmbuf.write((const char*)&log, sizeof(log));
}

void* malloc(size_t size) {
    if(!original_malloc)
        original_malloc = (MallocType*) dlsym(RTLD_NEXT, "malloc");

    Log log;
    log.address = original_malloc(size);
    log.size = size;
    log.type = LogType::MALLOC;
    gettimeofday(&log.tv, nullptr);

    if(log.address)
        do_record(log);
    return log.address;
}

void* calloc(size_t nmemb,size_t size) {
    if(!original_calloc)
        original_calloc = (CallocType*) dlsym(RTLD_NEXT, "calloc");

    Log log;
    log.address = original_calloc(nmemb, size);
    log.size = size * nmemb;
    log.type = LogType::CALLOC;
    gettimeofday(&log.tv, nullptr);

    if(log.address)
        do_record(log);
    return log.address;
}

void* realloc(void* ptr, size_t size) {
    if(!original_realloc)
        original_realloc = (ReallocType*) dlsym(RTLD_NEXT, "realloc");

    Log log;
    if(ptr) {
        log.address = ptr;
        log.size = 0;
        log.type = LogType::FREE;
        gettimeofday(&log.tv, nullptr);
        do_record(log);
    }

    if(size) {
        log.address = original_realloc(ptr, size);
        log.size = size;
        log.type = LogType::MALLOC;
        gettimeofday(&log.tv, nullptr);
        if(log.address)
            do_record(log);
        return log.address;
    }

    return nullptr;
}

void free(void* ptr) {
    if(!original_free)
        original_free = (FreeType*) dlsym(RTLD_NEXT, "free");

    Log log;
    if(ptr) {
        log.address = ptr;
        log.size = 0;
        log.type = LogType::FREE;
        gettimeofday(&log.tv, nullptr);
        do_record(log);
    }
}