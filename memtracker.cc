//
// Created by pilot on 10/2/2023.
//

#include <dlfcn.h>
#include "sliced_file_io.h"
#include "common.h"
#include <sys/time.h>

using MallocType = void* (size_t);
using CallocType = void* (size_t, size_t);
using ReallocType = void* (void*, size_t);
using FreeType = void (void*);

MallocType *original_malloc = nullptr;
CallocType *original_calloc = nullptr;
ReallocType *original_realloc = nullptr;
FreeType *original_free = nullptr;

const char* LOG_FILENAME_PREFIX = "/tmp/memtracker";
const size_t SIZE_PER_FILE = 1024 * 4 * 25;
/* static thread_local SliceFileIO sliced_file_io(LOG_FILENAME_PREFIX, getpid(), SIZE_PER_FILE); */
static thread_local SlicedFileWriter sliced_file_writer(LOG_FILENAME_PREFIX, SIZE_PER_FILE);

void do_record(Log const& log) {
//    ::write(STDERR_FILENO, "DO_RECORD\n", 10);
    sliced_file_writer.write_one(log);
}

extern "C" {

void *malloc(size_t size) {
    if (!original_malloc)
        original_malloc = (MallocType *) dlsym(RTLD_NEXT, "malloc");

    Log log;
    log.address = original_malloc(size);
    log.size = size;
    log.type = LogType::MALLOC;
    gettimeofday(&log.tv, nullptr);

    if (log.address)
        do_record(log);
    return log.address;
}

void *calloc(size_t nmemb, size_t size) {
    if (!original_calloc)
        original_calloc = (CallocType *) dlsym(RTLD_NEXT, "calloc");

    Log log;
    log.address = original_calloc(nmemb, size);
    log.size = size * nmemb;
    log.type = LogType::CALLOC;
    gettimeofday(&log.tv, nullptr);

    if (log.address)
        do_record(log);
    return log.address;
}

void *realloc(void *ptr, size_t size) {
    if (!original_realloc)
        original_realloc = (ReallocType *) dlsym(RTLD_NEXT, "realloc");

    Log log;
    if (ptr) {
        log.address = ptr;
        log.size = 0;
        log.type = LogType::FREE;
        gettimeofday(&log.tv, nullptr);
        do_record(log);
    }

    if (size) {
        log.address = original_realloc(ptr, size);
        log.size = size;
        log.type = LogType::MALLOC;
        gettimeofday(&log.tv, nullptr);
        if (log.address)
            do_record(log);
        return log.address;
    }

    return nullptr;
}

void free(void *ptr) {
    if (!original_free)
        original_free = (FreeType *) dlsym(RTLD_NEXT, "free");

    Log log;
    if (ptr) {
        log.address = ptr;
        log.size = 0;
        log.type = LogType::FREE;
        gettimeofday(&log.tv, nullptr);
        do_record(log);
    }
}

}
