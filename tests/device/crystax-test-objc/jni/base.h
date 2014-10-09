#ifndef TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde
#define TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#else
#import <objc/Object.h>
#import <objc/NXConstStr.h>
#endif

#ifdef __APPLE__
typedef NSObject Object;
#endif

#if defined(__APPLE__) || defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
@interface BaseObject : Object
+ (id) alloc;
- (void) free;
@end
#else
typedef Object BaseObject;
#endif

#endif /* TEST_OBJC_BASE_6f293248b98949b0ab333a7b6436ccde */
