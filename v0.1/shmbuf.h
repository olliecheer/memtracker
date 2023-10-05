//
// Created by pilot on 10/2/2023.
//

#ifndef UNTITLED_SHMBUF_H
#define UNTITLED_SHMBUF_H

#include <climits>
#include <cstring>
#include <cstdlib>
#include "printer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdint.h>
//#include <string>

// PATH_MAX is 4096. We reset it here to avoid large memory block in stack
const int PATH_MAX_SIZE = 128;

class FilenameNumerator {
public:
    FilenameNumerator(char const* filename_base, size_t filename_base_strlen) {
        if(filename_base_strlen > PATH_MAX_SIZE) {
            Err("FilenameNumerator: filename_base_len %lu > %d", filename_base_strlen, PATH_MAX_SIZE - 1);
            exit(1);
        }
        strcpy(buffer, filename_base);
        base_len = filename_base_strlen;
//        if(!next()) {
//            Err("FilenameNumerator: next() failed\n");
//            exit(1);
//        }
    }
    size_t get_len() const {
        return total_len;
    }
    char const* get_name() const {
        return buffer;
    }

    bool next() {
        if(get_int_digit_size(numerator) + base_len > PATH_MAX_SIZE - 1 - 1)
            return false;
        int _len = snprintf(buffer+base_len, PATH_MAX_SIZE - base_len, "-%d", numerator);
        total_len = base_len + _len;
        numerator++;

        return true;
    }

private:
    static int get_int_digit_size(int n) {
        if(n == 0)
            return 0;
        int res = 1;
        for(; n != 0; res++, n/= 10);
        return res;
    }
private:
    int numerator = 0;
    char buffer[PATH_MAX_SIZE] = {};
    size_t base_len;
    size_t total_len = -1;
};

class ShmBuf {
public:
    explicit ShmBuf(char const* filename_base, size_t size_per_file)
    : size_per_file{size_per_file}, numerator(filename_base, strlen(filename_base)) {
        switch_file();
    }

    ~ShmBuf() {
        ::close(fd);
    }

    ssize_t write(char const* buf, size_t size) {
        if(size + file_used_size > size_per_file) {
            if(size > size_per_file) {
                Err("write size too large. %lu > %lu", size, size_per_file);
                exit(1);
            }
            switch_file();
        }

        ssize_t res = ::write(fd, buf, size);

        if(res == -1) {
            Err("ShmBuf: write failed\n");
            exit(1);
        }
        file_used_size += res;
        return res;
    }

private:
    void switch_file() {
        if(fd >= 0)
            ::close(fd);
        numerator.next();
        fd = open(numerator.get_name(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
        if(fd == -1) {
            Err("ShmBuf: open file failed.");
            exit(1);
        }
        file_used_size = 0;
    }

private:
    int fd = -1;
    size_t file_used_size = 0;
    size_t size_per_file;
    FilenameNumerator numerator;
};

#endif //UNTITLED_SHMBUF_H
