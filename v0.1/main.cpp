#include <iostream>
#include "test.h"

#define DIRECTORY "/tmp"

//const char* FILE_LIST[] = {
//        DIRECTORY "/memtracker-1230",
//        DIRECTORY "/memtracker-1231",
//        DIRECTORY "/memtracker-1232",
//        DIRECTORY "/memtracker-1233",
//};


#include <stddef.h>
#include <sys/types.h>
#include <thread>
#include <mutex>

std::vector<SliceFile> files;

FreeMap free_map;
RawMallocThreads raw_malloc_threads;
std::mutex raw_malloc_threads_mtx;

MallocDataVec malloc_data_vec;
std::mutex malloc_data_vec_mtx;

void read_worker_func(SliceFile file, pid_t tid) {
    RawMallocList raw_mallocs(tid);
    while(!file.eof()) {
        auto const& log = file.read_one();
        switch (log.type) {
            case LogType::FREE: {
                free_map.insert(log.address, log.tv);
                break;
            }
            case LogType::MALLOC:
            case LogType::CALLOC: {
                raw_mallocs.push_back(log);
                break;
            }
            default:
                fprintf(stderr, "unexpectd Log Type");
                abort();
        }
    }

    std::scoped_lock lk(raw_malloc_threads_mtx);
    raw_malloc_threads.push_back(std::move(raw_mallocs));
}

void match_worker_func(RawMallocList raw_malloc) {
    UnresolvedMallocMap unresolved_malloc;
    ResolvedMallocMap resolved_malloc;

    while(!raw_malloc.empty()) {
        auto log = raw_malloc.get_pop();
        auto matched_free = free_map.get_pop(log.address, log.tv);

        if(matched_free.has_value()) {
            resolved_malloc.insert(log.address, log.size, log.type, log.tv, matched_free.value());
        } else {
            unresolved_malloc.insert(log.address, log.size, log.type, log.tv);
        }
    }

    std::scoped_lock lk(malloc_data_vec_mtx);
    malloc_data_vec.emplace_back(raw_malloc.get_tid(), std::move(resolved_malloc), std::move(unresolved_malloc));
}

int main() {
    auto threads = {34, 12,43,31,23};
    std::vector<std::thread> workers;

    for(auto tid : threads)
        workers.push_back(std::thread(read_worker_func,SliceFile("/tmp", std::string("memtracker") + std::to_string(tid)), tid));
    for(auto & it : workers)
        it.join();
    workers.clear();

    for(auto&& it : raw_malloc_threads)
        workers.push_back(std::thread(match_worker_func, std::move(it)));
    for(auto & it : workers)
        it.join();
    workers.clear();


}
