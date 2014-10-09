#define UNUSED(x) (void)x

#if defined(__APPLE__)

#import <Foundation/Foundation.h>

typedef NSObject BaseObject;

#elif defined(__GNUC__)

#import <objc/runtime.h>
#import <objc/Object.h>
#import <objc/NXConstStr.h>

#if (__GNUC__ == 4) && (__GNUC_MINOR__ <= 6) && !__clang__

typedef Object BaseObject;

#else

@interface BaseObject : Object

+ (id)alloc;
- (void)free;

@end

@implementation BaseObject

+ (id)alloc
{
    return class_createInstance(self, 0);
}

- (void)free
{
    object_dispose(self);
}

@end /* BaseObject */

#endif /* (__GNUC__ == 4) && (__GNUC_MINOR__ <= 6) && !__clang__ */
#endif

#ifdef __APPLE__
typedef NSZone Zone;
#else
typedef id Zone;
#endif

@interface Bar : BaseObject
{
    int _intProperty;
}

@property int intProperty;

- (id)init;
- (void)test;
- (id)copyWithZone: (Zone *) zone;

@end


@implementation Bar

@synthesize intProperty = _intProperty;

- (id)init
{
    if (self) {
        self.intProperty = 0;
    }

    return self;
}

- (void)test
{
    self.intProperty = 10;
}

- (id)copyWithZone: (Zone *) zone
{
    UNUSED(zone);
    Bar *bar = [[Bar alloc] init];
    bar.intProperty = self.intProperty;

    return bar;
}

@end


@interface Foo : BaseObject
{
    BOOL boolProperty;
    Bar *barProperty;
    id stringProperty;
}

@property BOOL boolProperty;
@property (copy) Bar *barProperty;
@property (assign) id stringProperty;

- (id)init;
- (void)test;

@end


@implementation Foo

@synthesize boolProperty;
@synthesize barProperty;
@synthesize stringProperty;

- (id)init
{
    if (self) {
        self.boolProperty = NO;
        self.barProperty = nil;
        self.stringProperty = @"";
    }

    return self;
}

- (void)test
{
    self.boolProperty = YES;
    self.barProperty = [[Bar alloc] init];
    self.stringProperty = @"String";
}

@end

int main()
{
    Foo *f = [[Foo alloc] init];
    [f test];

    return 0;
}
