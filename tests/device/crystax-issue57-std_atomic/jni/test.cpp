#include <atomic>
#include <stdio.h>

int main()
{
    printf("std::atomic test - begin\n");

    std::atomic<void*> atomicPtr;
    void *ptr = (void*) 0x12345678;
    atomicPtr = ptr;
    if ((void*)ptr != (void*)atomicPtr)
    {
        fprintf(stderr, "ERROR: ptr=%p, atomicPtr=%p\n", (void*)ptr, (void*)atomicPtr);
        return 1;
    }

    printf("OK\n");
    return 0;
}
