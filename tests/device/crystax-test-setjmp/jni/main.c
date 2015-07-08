#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include <setjmp.h>
#include <stdio.h>

static jmp_buf jb;

void foo(int count)
{
    printf("foo(%d) called\n", count);
    longjmp(jb, count + 1);
}

int main()
{
    volatile int count = 0;
    if (setjmp(jb) < 9)
        foo(count++);

    assert(count == 9);
    return 0;
}
