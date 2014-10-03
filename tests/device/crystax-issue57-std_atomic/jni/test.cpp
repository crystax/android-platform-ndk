#include <atomic>
#include <stdio.h>

int main()
{
    std::atomic<void*> atomicPtr;
    void *ptr = (void*) 0x12345678;
    atomicPtr = ptr;
    if ((void*)ptr != (void*)atomicPtr)
    {
        printf("ERROR: ptr=%p, atomicPtr=%p\n", (void*)ptr, (void*)atomicPtr);
        return 1;
    }

    return 0;
}
