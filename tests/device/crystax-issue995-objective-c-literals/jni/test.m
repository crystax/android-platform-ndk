/*
 * Copyright (c) 2011-2015 CrystaX .NET.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX .NET.
 */

#import <Foundation/Foundation.h>

#import <assert.h>

#if !__has_feature(objc_array_literals)
#error "ObjC array literals not supported!"
#endif

#if !__has_feature(objc_dictionary_literals)
#error "ObjC dictionary literals not supported!"
#endif

#if !__has_feature(objc_subscripting)
#error "ObjC subscripting not supported!"
#endif

#if !__APPLE__ && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 7))

#if !__has_attribute(objc_boxable)
#error "ObjC boxable not supported!"
#endif

#if !__has_feature(objc_boxed_nsvalue_expressions)
#error "ObjC boxed NSValue expressions not supported!"
#endif

#endif // __APPLE__

int main()
{
    NSNumber *theLetterZ = @'Z';
    assert([theLetterZ charValue] == 'Z');

    NSNumber *fortyTwo = @42;
    assert([fortyTwo intValue] == 42);

    NSNumber *fortyTwoUnsigned = @42U;
    assert([fortyTwoUnsigned unsignedIntValue] == 42);

    NSNumber *fortyTwoLong = @42L;
    assert([fortyTwoLong longValue] == 42);

    NSNumber *fortyTwoLongLong = @42LL;
    assert([fortyTwoLongLong longLongValue] == 42);

    NSNumber *yesNumber = @YES;
    assert([yesNumber boolValue] == YES);

    NSNumber *noNumber = @NO;
    assert([noNumber boolValue] == NO);

    NSNumber *smallestInt = @(-INT_MAX - 1);
    assert([smallestInt intValue] == (-INT_MAX - 1));

    // There is bug with clang/armeabi-v7a-hard interpreting float literals
    // as zero values. See https://tracker.crystax.net/issues/1080.
    // Temporary disable this block til bug #1080 would be fixed.
#if !__ARM_PCS_VFP
    NSNumber *piFloat = @3.141592654F;
    NSLog(@"piFloat=%@", piFloat);
    assert(fabsf([piFloat floatValue] - 3.141592654F) < 0.000001);

    NSNumber *piDouble = @3.1415926535;
    NSLog(@"piDouble=%@", piDouble);
    assert(fabs([piDouble doubleValue] - 3.1415926535) < 0.000001);

    NSNumber *piOverTwo = @(M_PI / 2);
    NSLog(@"piOverTwo=%@", piOverTwo);
    assert(fabs([piOverTwo doubleValue] - M_PI / 2) < 0.000001);
#endif // !__ARM_PCS_VFP

    typedef enum { Red, Green, Blue } Color;
    NSNumber *favoriteColor = @(Green);
    assert([favoriteColor intValue] == (int)Green);

    NSString *path = @(getenv("PATH"));
    assert(strcmp([path UTF8String], getenv("PATH")) == 0);

    const char *ss = "ABCD";
    NSString *stringBoxed = @(ss + 1);
    assert(strcmp([stringBoxed UTF8String], "BCD") == 0);

#if !__APPLE__ && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 7))
    struct __attribute__((objc_boxable)) Point {
        int x;
        int y;
    };

    struct Point p = {1234, 7619};
    NSValue *point = @(p);

    struct Point anotherPoint;
    [point getValue:&anotherPoint];
    assert(p.x == anotherPoint.x && p.y == anotherPoint.y);
#endif

    NSArray *array = @[ @"Hello", @11, [NSNumber numberWithInt:42] ];
    assert([array count] == 3);
    assert([[array objectAtIndex:0] isEqualToString:@"Hello"]);
    assert([[array objectAtIndex:1] isEqualToNumber:@11]);
    assert([[array objectAtIndex:2] isEqualToNumber:@42]);

    NSDictionary *dictionary = @{
                @"name" : NSUserName(),
                @"date" : [NSDate date],
         @"processInfo" : [NSProcessInfo processInfo]
    };
    assert([dictionary count] == 3);

    NSMutableArray *array2 = [NSMutableArray arrayWithArray:@[@4234, @"Hello111"]];
    assert([array2 count] == 2);

    NSLog(@"array2[0]=%@", array2[0]);
    assert([array2[0] isEqualToNumber:@4234]);
    array2[0] = @4345;
    NSLog(@"array2[0]=%@", array2[0]);
    assert([array2[0] isEqualToNumber:@4345]);

    NSLog(@"array2[1]=%@ (%@)", array2[1], [array2[1] class]);
    NSLog(@"array2[1]=%@ (%@) (using objectAtIndex)", [array2 objectAtIndex:1], [[array2 objectAtIndex:1] class]);
    assert([array2[1] isEqualToString:@"Hello111"]);
    array2[1] = @"Hello222";
    NSLog(@"array2[1]=%@ (%@)", array2[1], [array2[1] class]);
    NSLog(@"array2[1]=%@ (%@) (using objectAtIndex)", [array2 objectAtIndex:1], [[array2 objectAtIndex:1] class]);
    assert([array2[1] isEqualToString:@"Hello222"]);

    NSMutableDictionary *dictionary2 = [NSMutableDictionary dictionaryWithDictionary:@{@"name" : @"NAME", @"value": @1234}];
    assert([dictionary2 count] == 2);
    assert([dictionary2[@"name"] isEqualToString:@"NAME"]);
    dictionary2[@"name"] = @"NAME2";
    assert([dictionary2[@"name"] isEqualToString:@"NAME2"]);
    assert([dictionary2[@"value"] isEqualToNumber:@1234]);
    dictionary2[@"value"] = @5678;
    assert([dictionary2[@"value"] isEqualToNumber:@5678]);

    NSLog(@"SUCCESS");
    return 0;
}
