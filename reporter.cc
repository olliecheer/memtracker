#include "reporter.h"
#include "sliced_file_io.h"
#include "free_map.h"
#include <thread>


bool PlainReporter::load(std::string path_prefix, const std::vector<pid_t>& tids) {
    FreeMap free_map;
    std::list<RawMallocList> all_raw_mallocs;
    std::mutex all_raw_mallocs_mtx;
    std::vector<std::thread> workers;

    for(auto tid : tids) {
        workers.emplace_back([tid, &path_prefix, &free_map, &all_raw_mallocs_mtx, &all_raw_mallocs](){
            std::string path = path_prefix + "-" + std::to_string(tid);
            SlicedFileReader reader(path.c_str());
            RawMallocList raw_mallocs(tid);
            std::optional<Log> log;

            while((log = reader.read_one<Log>()).has_value()) {
                auto const& v = log.value();
                switch (v.type) {
                    case LogType::FREE: {
                        free_map.insert(v.address, v.tv);
                        break;
                    }
                    case LogType::MALLOC:
                    case LogType::CALLOC: {
                        raw_mallocs.push_back(v);
                    }
                }
            }

            std::scoped_lock lk(all_raw_mallocs_mtx);
            all_raw_mallocs.push_back(raw_mallocs);
        });
    }
    for(auto& worker : workers)
        worker.join();
    workers.clear();

    for(auto& raw_mallocs : all_raw_mallocs) {
        workers.emplace_back([&free_map, this, &raw_mallocs]() {
            MallocMap<ResolvedMalloc> resolved_map;
            MallocMap<UnresolvedMalloc> unresolved_map;
            while(!raw_mallocs.empty()) {
                auto log = raw_mallocs.get_pop();
                auto match_free = free_map.get_pop(log.address, log.tv);
                if(match_free.has_value()) {
                    resolved_map.insert(log.address, log.size, log.type, log.tv, match_free.value());
                } else {
                    unresolved_map.insert(log.address, log.size, log.type, log.tv);
                }
            }

            {
                std::scoped_lock lk(all_resolved_mtx);
                all_resolved.insert({raw_mallocs.get_tid(), std::move(resolved_map)});
            }

            {
                std::scoped_lock lk(all_unresolved_mtx);
                all_unresolved.insert({raw_mallocs.get_tid(), std::move(unresolved_map)});
            }
        });
    }
    for(auto& worker : workers)
        worker.join();
    workers.clear();

    return true;
}

void PlainReporter::report() {
    for(auto& it : all_resolved) {
        fprintf(stdout, "==== tid: %d ====\n", it.first);
//        fprintf(stdout, "resolved_alloc: %llu; unresolved_alloc: %lu", it.second.)
    }
}
