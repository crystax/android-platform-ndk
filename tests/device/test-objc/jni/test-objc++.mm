#include <stdio.h>
#include <objc/Object.h>

class Foo
{
public:
    Foo() {printf("%s\n", __PRETTY_FUNCTION__);}
    ~Foo() {printf("%s\n", __PRETTY_FUNCTION__);}
};

@interface foo2 : Object

-(id) init;
-(void) bar;

@end

@implementation foo2

-(id) init
{
    printf("%s\n", __PRETTY_FUNCTION__);
    return self;
}

-(void) bar
{
    printf("%s\n", __PRETTY_FUNCTION__);
}

@end

extern "C"
int test_objcpp()
{
    Foo cppobj;

    id obj = [[foo2 alloc] init];
    [obj bar];
    [obj free];

    return 1;
}
