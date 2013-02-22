#include <stdio.h>
// #include <iostream>
// #include <iomanip>



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
	A *pa = dynamic_cast<A*>(new B);
	B *pb = static_cast<B*>(pa);
	B *pc = dynamic_cast<B*>(pa);

    printf("pa = %x\n", pa);
    printf("pb = %x\n", pb);
    // this line will cause segfault with gcc 4.4.3 on armeabi
    printf("pc = %x\n", pc);

    /*
    std::cout << "pa = " << std::hex << pa << std::endl;
    std::cout << "pb = " << std::hex << pb << std::endl;
    // this line will cause segfault with gcc 4.4.3 on armeabi
    std::cout << "pc = " << std::hex << pc << std::endl;
    */

	delete pa;

	return 0;
}
