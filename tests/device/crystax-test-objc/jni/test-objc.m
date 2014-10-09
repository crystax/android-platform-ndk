#import <stdio.h>
#import "base.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

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
    assert(self != nil);
    return self;
}

-(void) bar
{
    self.boolProperty = YES;
    assert(self.boolProperty == YES);
    self.boolProperty = NO;
    assert(self.boolProperty == NO);
    self.stringProperty = @"";
    assert([self.stringProperty isEqual:@""]);
    self.boolProperty = YES;
    assert(self.boolProperty == YES);
}

@end

int test_objc()
{
    id obj = [[foo alloc] init];
    [obj bar];
    [obj free];
    return 1;
}
