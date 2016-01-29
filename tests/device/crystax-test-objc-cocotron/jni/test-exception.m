#import <Foundation/Foundation.h>

void foo()
{
    @throw [NSException exceptionWithName:@"EXCEPTION"
                                   reason:@"Test of Objective-C exceptions mechanism"
                                 userInfo:nil];
}

int main()
{
    @try {
        foo();
    }
    @catch (NSException *ex)
    {
        NSLog(@"OK: Exception caught");
        return 0;
    }

    NSLog(@"FAILED: Exception was not caught");
    return 1;
}
