#import "base.h"

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

#elif defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)

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

