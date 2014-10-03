#import "base.h"

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)

@implementation BaseObject

+ alloc
{
    return class_createInstance(self, 0);
}

- free
{
    object_dispose(self);
}

@end /* BaseObject */

#endif

