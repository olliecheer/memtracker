#ifndef SLICED_FILE_IO_H
#define SLICED_FILE_IO_H

//#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <climits>
#include <cstring>
#include <unistd.h>
//#include <cassert>
#include <optional>
#include <cstdio>
//#include <unistd.h>
#include <stdlib.h>
#include "mm_file_io.h"



// PATH_MAX is 4096. We reset it here to avoid large memory block in stack
const int PATH_MAX_SIZE = 128;

class FilenameNumerator {
    int numerator = 0;
    char buffer[PATH_MAX_SIZE];
    int base_len;
    int total_len = -1;

private:
    static int get_int_digit_size(int n) {
        if(n == 0)
            return 1;
        int res = 1;
        for(; n != 0; res++, n/= 10);
        return res;
    }

public:
    FilenameNumerator(char const* path_prefix, int path_prefix_strlen, int tid) {
//        ::write(STDERR_FILENO, "MMFILEWRITER CONSTRUCTOR 2\n", 27);
        ASSERT(path_prefix_strlen < PATH_MAX_SIZE);
        strcpy(buffer, path_prefix);
        int tid_len = snprintf(buffer + path_prefix_strlen, PATH_MAX_SIZE - path_prefix_strlen, "-%d", tid);
        base_len = path_prefix_strlen + tid_len;
    }
    size_t len() const {
        return total_len;
    }
    char const* filename() const {
        return buffer;
    }
    bool next() {
        if(get_int_digit_size(numerator) + base_len > PATH_MAX_SIZE -1 - 1)
            return false;
        int _len = snprintf(buffer + base_len, PATH_MAX_SIZE - base_len, "-%d", numerator);
        total_len = base_len + _len;
        numerator++;
        return true;
    }
};


class SlicedFileReader {
    FilenameNumerator numerator;
    MMFileReader reader;

    bool next_file() {
        if(!reader.open(numerator.filename()))
            return false;
        numerator.next();
        return true;
    }
public:
    SlicedFileReader(char const* path_prefix)
    : numerator(path_prefix, strlen(path_prefix), getpid()){
        ASSERT(next_file());
    }
    ~SlicedFileReader() = default;

    template<typename T>
    std::optional<T> read_one() {
        T res;
        if(sizeof(res) != reader.read(&res, sizeof(res))) {
            if(!next_file())
                return {};
            if(sizeof(res) != reader.read(&res, sizeof(res)))
                return {};
        }
        return res;
    }
};

class SlicedFileWriter {
    FilenameNumerator numerator;
    MMFileWriter writer;
    size_t size_per_file;

    void next_file() {
//        ::write(STDERR_FILENO, "next file\n", 10);
        writer.open(numerator.filename(), size_per_file);
//        ::write(STDERR_FILENO, "next file 2\n", 12);
        numerator.next();
//        ::write(STDERR_FILENO, "next file 3\n", 12);
    }
public:
    SlicedFileWriter(char const* path_prefix, size_t size_per_file)
    : numerator(path_prefix, strlen(path_prefix), getpid()), size_per_file(size_per_file){
//        ::write(STDERR_FILENO, "MMFILEWRITER CONSTRUCTOR\n", 25);
        next_file();
    }
    ~SlicedFileWriter() = default;

    template<typename T>
    void write_one(T const&data) {
        if(writer.destructed())
            return;
//        ::write(STDERR_FILENO, "write_one\n", 11);
        if(sizeof(data) != writer.write(&data, sizeof(data))) {
//            ::write(STDERR_FILENO, "write_one 2\n", 13);
            next_file();
//            ::write(STDERR_FILENO, "write_one 3\n", 13);
            ASSERT(sizeof(data) == writer.write(&data, sizeof(data)));
//            ::write(STDERR_FILENO, "write_one 4\n", 13);
        }
    }
};

/* // act as a serializer and deserializer, allow sliced files */
/* class SliceFileIO { */

/* private: */
/*     size_t size_per_file; */
/*     size_t file_used_size; */
/*     int fd = -1; */
/*     FilenameNumerator numerator; */

/* private: */
/*     bool switch_file() { */
/*         if(fd >= 0) */
/*             close(fd); */
/*         ASSERT(numerator.next()); */
/*         fd = open(numerator.filename(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP); */
/*         file_used_size = 0; */
/*         return fd != -1; */
/*     } */

/* public: */
/*     SliceFileIO(char const* path_prefix, pid_t pid, size_t size_per_sliced_file = 4096 * 16) */
/*     : size_per_file(size_per_sliced_file), numerator(path_prefix, strlen(path_prefix), pid) { */
/*         switch_file(); */
/*     } */
/*     ~SliceFileIO()  { */
/*         close(fd); */
/*         fd = -1; */
/*     } */

/*     template<typename T> */
/*     void write_one(T const&data) { */
/*         if(sizeof(data) + file_used_size > size_per_file) { */
/*             ASSERT(sizeof (data) <= size_per_file); */
/*             ASSERT(switch_file()); */
/*         } */
/*         auto sz = write(fd, &data, sizeof(data)); */
/*         if(sz == -1) { */
/*             if(fd == -1) // seems this instance has been destructed */
/*                 return; */
/*             else */
/*                 abort(); */
/*         } */
/*         file_used_size += sz; */
/*     } */
/* }; */

#endif

