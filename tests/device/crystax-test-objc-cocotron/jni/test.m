#import <Foundation/Foundation.h>

@interface TestClass : NSObject
- (void)testMethod;
@end

@implementation TestClass
- (void)testMethod
{
    NSLog(@"Hello from method!");
}
@end

int main()
{
    TestClass *obj = [[TestClass alloc] init];
    [obj testMethod];

    NSLog(@"OK");

    return 0;
}
