#include <new>
#include <stdio.h>
#include <string.h>

class Test
{
public:
    Test() {
        ::fprintf(stderr, "ctor start\n");
        //memset(c, 0, sizeof(c));
        ::fprintf(stderr, "ctor finish\n");
    }

private:
    char c[1024*1024*1024];
};

int main()
{
    try {
        while (1) {
            Test *p = new Test();
            if (!p) {
                ::fprintf(stderr, "ERROR: we've got NULL from 'new' instead of std::bad_alloc\n");
                return 1;
            }
            fprintf(stderr, "p=%p\n", p);
        }
        ::fprintf(stderr, "ERROR: we somehow finished infinite loop (how it could be?)\n");
        return 1;
    } catch (std::bad_alloc) {
        ::fprintf(stderr, "OK: we've got std::bad_alloc\n");
        return 0;
    }

    ::fprintf(stderr, "ERROR: we somehow reached unreachable code point\n");
    return 1;
}
