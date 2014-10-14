#import <stdio.h>

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

@interface foo : BaseObject {
    BOOL boolProperty;
    id stringProperty;
}

@property (assign) BOOL boolProperty;
@property (assign) id stringProperty;

-(id) init;
-(void) bar;

@end

@implementation foo

@synthesize boolProperty;
@synthesize stringProperty;

-(id) init
{
    printf("%s\n", __PRETTY_FUNCTION__);
    return self;
}

-(void) bar
{
    self.boolProperty = YES;
    self.boolProperty = NO;
    self.stringProperty = @"";
    self.boolProperty = YES;
}

@end

int test_objc()
{
    id obj = [[foo alloc] init];
    [obj bar];
    [obj free];
    return 1;
}

int main()
{
    if (!test_objc())
        return 1;

    return 0;
}
