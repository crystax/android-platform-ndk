#include <stdio.h>
#include <objc/Object.h>

@interface foo : Object

-(id) init;
-(void) bar;

@end

@implementation foo

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

int test_objc()
{
    id obj = [[foo alloc] init];
    [obj bar];
    [obj free];
    return 1;
}
