#ifndef MM_FILE_IO_H
#define MM_FILE_IO_H

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cstdlib>
#include <sys/mman.h>
#include <cstring>


static inline void ASSERT(bool cond) {
    if(!cond)
        abort();
}

class MMFileWriter {
    int fd_ = -1;
    void* ptr = nullptr;
    size_t* valid_offset_ = nullptr;
    size_t total_size_ = 0;
public:
    MMFileWriter() = default;

//    MMFileWriter() {
//        ::write(STDERR_FILENO, "MMFILEWRITER CONSTRUCTOR 3\n", 27);
//}
    void open(char const* filename, size_t file_size){

//        ::write(STDERR_FILENO, "open next file\n", 15);
//        ASSERT(fd_ == -1);
        close();
        fd_ = ::open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
//        ::write(STDERR_FILENO, "open next file 2\n", 17);
        ASSERT(fd_ >= 0);

        int err = fallocate(fd_, 0, 0, file_size);
        ASSERT(err == 0);
        total_size_ = file_size;

        ptr = mmap(nullptr, total_size_, PROT_WRITE, MAP_SHARED, fd_, 0);
        ASSERT(ptr != (void*) -1);

        valid_offset_ = (size_t*)ptr;
        *valid_offset_ = sizeof(*valid_offset_);
    }

    void close() {
        ::close(fd_);
        fd_ = -1;
        munmap(ptr, total_size_);
        ptr = nullptr;
        valid_offset_ = nullptr;
        total_size_ = 0;
    }

    ~MMFileWriter() {
        close();
    }

    ssize_t write(void const* buf, size_t count) {
        if(*valid_offset_ + count > total_size_)
            return 0;
//        ::write(STDERR_FILENO, "mmwriter write\n", 15);
        memcpy((char*)ptr + *valid_offset_, buf, count);
        *valid_offset_+=count;
        return count;
    }

    bool destructed() const {
        return fd_ == -1;
    }
};

class MMFileReader {
    int fd_ = -1;
    void* ptr = nullptr;
    size_t total_size_ = 0;
    size_t offset_ = 0;
public:
    MMFileReader() = default;
    bool open(char const* filename) {
        ASSERT(fd_ == -1);
        fd_ = ::open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
        if(fd_ < 0)
            return false;
        auto file_size = lseek(fd_, 0, SEEK_END);
        ptr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd_, 0);
        ASSERT(ptr != (void*) -1);
        total_size_ = *(size_t*)ptr;
        offset_ = sizeof(total_size_);
        return true;
    }

    void close() {
        munmap(ptr, total_size_);
        ptr = nullptr;
        ::close(fd_);
        fd_ = -1;
        total_size_ = 0;
        offset_ = 0;
    }

    ~MMFileReader() {
        close();
    }

    ssize_t read(void* buf, size_t count) {
        if(offset_ + count > total_size_)
            return 0;
        memcpy(buf, (char*)ptr + offset_, count);
        offset_+= count;
        return count;
    }
};


#endif
