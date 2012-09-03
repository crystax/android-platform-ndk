#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <new>

static bool memory_manager_initialized = false;

void *operator new(std::size_t n) throw(std::bad_alloc)
{
    if (!memory_manager_initialized)
        std::abort();

    void *p = ::malloc(n);
    if (!p)
        throw std::bad_alloc();
    return p;
}

void *operator new[](std::size_t n) throw(std::bad_alloc)
{
    if (!memory_manager_initialized)
        std::abort();

    void *p = ::malloc(n);
    if (!p)
        throw std::bad_alloc();
    return p;
}

void *operator new(size_t n, std::nothrow_t const &) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    return ::malloc(n);
}

void *operator new[](size_t n, std::nothrow_t const &) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    return ::malloc(n);
}

void operator delete(void *p) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    ::free(p);
}

void operator delete[](void *p) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    ::free(p);
}

void operator delete(void *p, std::nothrow_t const &) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    ::free(p);
}

void operator delete[](void *p, std::nothrow_t const &) throw()
{
    if (!memory_manager_initialized)
        std::abort();

    ::free(p);
}

int main()
{
    // Just to trigger fileio module initialization
    ::open("/", O_RDONLY);

    // And only after that do memory manager initialization
    memory_manager_initialized = true;

    return 0;
}
