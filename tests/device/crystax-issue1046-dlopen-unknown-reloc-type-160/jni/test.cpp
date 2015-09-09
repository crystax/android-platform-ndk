#include <atomic>

#define N 3
struct S {
    int a[N];
};

extern "C"
void qq() {
    std::atomic<S> s;
    s.exchange(S());
}
