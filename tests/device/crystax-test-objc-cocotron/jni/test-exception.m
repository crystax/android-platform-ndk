#import <Foundation/Foundation.h>

void foo()
{
    @throw [NSException exceptionWithName:@"EXCEPTION"
                                   reason:@"Test of Objective-C exceptions mechanism"
                                 userInfo:nil];
}

int main()
{
    // This test is temporarily disabled, until bug #1055 resolved.
    // See https://tracker.crystax.net/issues/1055 for details.
#if 0
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
#endif
}
