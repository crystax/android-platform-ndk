#ifdef NDEBUG
#undef NDEBUG
#endif
#import <assert.h>
#import <string.h>
#import <stdio.h>

#import <objc/runtime.h>
#import <objc/message.h>

#define MAGIC_STRING "AHBJAYEBDMAJ*EUDIQKJNSADJY^G#JAHBELAKJHBS"

static const char *s = NULL;

@interface TestClass2
- (void)testMethod;
@end

@implementation TestClass2
- (void)testMethod
{
    s = MAGIC_STRING;
    printf("Hello from method!\n");
}
@end

extern "C"
void test_objcxx()
{
    Class cls = objc_getClass("TestClass2");
    assert(cls != NULL);
    TestClass2 *obj = class_createInstance(cls, 0);
    assert(obj != NULL);

    SEL sel = sel_registerName("testMethod");
    assert(sel != NULL);

    objc_msgSend(obj, sel);
    assert(s != NULL && strcmp(s, MAGIC_STRING) == 0);

    printf("OK\n");
}
