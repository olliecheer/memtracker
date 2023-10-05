//
// Created by pilot on 10/2/2023.
//

#ifndef UNTITLED_RINGBUF_H
#define UNTITLED_RINGBUF_H

#include <stddef.h>
#include <mutex>


class RingBuf {
public:
    RingBuf();
    ~RingBuf();

    ssize_t Write(char* buffer, size_t len);
    ssize_t Read(char* buffer, size_t len);

private:
    void lock();

private:
    char* data;
    size_t len;
    char* r_begin;
    char* r_end;
    char* w_begin;
    char* w_end;

    std::mutex mtx;
};

#endif //UNTITLED_RINGBUF_H
