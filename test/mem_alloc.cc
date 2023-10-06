#include <iostream>
#include <vector>
#include <cstdlib>
#include <string.h>

int main() {
    int loop = 50000000;
    std::vector<void*> mem_ptrs;

    int mean_size_per_alloc = 256;
    int error = 100;

    
    for(int i=0; i<loop; i++) {
        int size = mean_size_per_alloc + rand() % error;
        char* p = (char*)malloc(size);
        memset(p, 0, size);
        mem_ptrs.push_back(p);
    }


    for(auto p: mem_ptrs)
        free(p);

    return 0;
}
