#ifndef TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde
#define TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde

#import <objc/Object.h>
#import <objc/NXConstStr.h>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
@interface BaseObject : Object
+ alloc;
- free;
@end
#else
typedef Object BaseObject;
#endif

#endif /* TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde */
