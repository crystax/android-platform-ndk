#ifdef NDEBUG
#undef NDEBUG
#endif
#import <assert.h>
#import <string.h>
#import <stdio.h>

#import <objc/runtime.h>
#import <objc/message.h>

#ifdef __NEXT_RUNTIME__
#import <objc/NSObject.h>
#else
#import <objc/Object.h>
#endif


#define MAGIC_STRING "AHBJAYEBDMAJ*EUDIQKJNSADJY^G#JAHBELAKJHBS"

const char *s = NULL;

#ifdef __NEXT_RUNTIME__
typedef NSObject BaseObject;
#else
typedef Object BaseObject;
#endif

@interface TestClass : BaseObject
- (void)testMethod;
@end

@implementation TestClass
- (void)testMethod
{
    s = MAGIC_STRING;
    printf("Hello from method!\n");
}
@end

int main()
{
    Class cls = objc_getClass("TestClass");
    assert(cls != NULL);
    TestClass *obj = class_createInstance(cls, 0);
    assert(obj != NULL);

    SEL sel = sel_registerName("testMethod");
    assert(sel != NULL);

    objc_msgSend(obj, sel);
    assert(s != NULL && strcmp(s, MAGIC_STRING) == 0);

    printf("OK\n");

    return 0;
}
