#import <Foundation/Foundation.h>

#define MAGIC_STRING @"E52780578BAF49FF8866608312B947A4"

@interface TestClass : NSObject
- (NSString*)testMethod;
@end

@implementation TestClass
- (NSString*)testMethod
{
    NSLog(@"Hello from method!");
    return MAGIC_STRING;
}
@end

int main()
{
    TestClass *obj = [[TestClass alloc] init];
    NSString *s = [obj testMethod];

    if (![s isEqualToString:MAGIC_STRING]) {
        NSLog(@"FAILED: s=%@", s);
        return 1;
    }

    NSLog(@"OK");

    return 0;
}
