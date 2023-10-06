#include <stdlib.h>
#include <stdio.h>

int main() {
    void* p = malloc(0x1230);
    printf("%p\n", p);
}
