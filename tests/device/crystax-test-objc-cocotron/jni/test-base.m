#import <Foundation/Foundation.h>

#define MAGIC_STRING @"E52780578BAF49FF8866608312B947A4"

@interface TestClass : NSObject {
    int _intProperty;
    NSString *_stringProperty;
}

@property (nonatomic, assign) int intProperty;
@property (nonatomic, retain) NSString *stringProperty;

- (NSString*)testMethod;

@end

@implementation TestClass

@synthesize intProperty = _intProperty;
@synthesize stringProperty = _stringProperty;

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

    obj.intProperty = 15;
    if (obj.intProperty != 15) {
        NSLog(@"FAILED: intProperty=%d", obj.intProperty);
        return 1;
    }

    obj.stringProperty = MAGIC_STRING;
    if (![obj.stringProperty isEqualToString:MAGIC_STRING]) {
        NSLog(@"FAILED: stringProperty=%@", obj.stringProperty);
        return 1;
    }

    NSLog(@"OK");

    return 0;
}
