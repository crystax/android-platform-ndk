#if !defined(__clang__)

int main()
{
    return 0;
}

#else

#import <stdio.h>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
typedef NSObject Object;
#else
#import <objc/Object.h>
#endif

@interface BaseObject : Object
+ (id) alloc;
- (void) free;
@end


#ifdef __APPLE__

@implementation BaseObject

+ (id) alloc
{
    return [super alloc];
}

- (void) free
{
    [super dealloc];
}

@end /* BaseObject */

#else

#import <objc/runtime.h>

@implementation BaseObject

+ (id) alloc
{
    return class_createInstance(self, 0);
}

- (void) free
{
    object_dispose(self);
}

@end /* BaseObject */

#endif

class Foo
{
public:
    Foo() {printf("%s\n", __PRETTY_FUNCTION__);}
    ~Foo() {printf("%s\n", __PRETTY_FUNCTION__);}
};

@interface foo2 : BaseObject

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


int main()
{
    if (!test_objcpp())
        return 1;

    return 0;
}

#endif
