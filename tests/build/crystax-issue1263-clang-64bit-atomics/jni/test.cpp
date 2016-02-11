#include <atomic>
#include <stdint.h>

int main()
{
    std::atomic<uint64_t> a(0);
    return ++a == 1 ? 0 : 1;
}
