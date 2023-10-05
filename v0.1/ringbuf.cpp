//
// Created by pilot on 10/2/2023.
//

#include "ringbuf.h"
#include <unistd.h>

long page_size = 0;

static void init() {
    page_size = sysconf(_SC_PAGESIZE);
    if(page_size == -1) {
        fprintf(stderr, "")
    }
}

RingBuf::RingBuf() {

}