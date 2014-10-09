#include <stdio.h>

class A {
public:
    A();
    virtual ~A();
};

class B : public A {
public:
    B();
    ~B();
};

A::A() {}
A::~A() {}
B::B() {}
B::~B() {}

int main()
{
    ::printf("dynamic_cast - begin");

    A *pa = dynamic_cast<A*>(new B);
    B *pb = static_cast<B*>(pa);
    B *pc = dynamic_cast<B*>(pa);

    ::printf("pa = %p\n", pa);
    ::printf("pb = %p\n", pb);
    // this line will cause segfault with gcc 4.4.3 on armeabi
    ::printf("pc = %p\n", pc);

    if (pa != pb)
    {
        ::fprintf(stderr, "*** ERROR: pa != pb\n");
        return 1;
    }

    if (pa != pc)
    {
        ::fprintf(stderr, "*** ERROR: pa != pc\n");
        return 1;
    }

    delete pa;

    ::printf("OK\n");

    return 0;
}
