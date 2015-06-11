#import <Foundation/Foundation.h>

#define CHECK_LENGTH(s, l) \
    NSCAssert(s.length == l, @"Wrong length of string: %zu instead of %zu", s.length, (NSUInteger)l)

#define CHECK_EQUAL(s, t) \
    NSCAssert([s isEqualToString:@t], @"\"%@\" != \"%@\"", s, @t)

void test1()
{
    NSString *s = [NSString string];
    CHECK_LENGTH(s, 0);
}

void test2()
{
    NSString *s = [[NSString alloc] init];
    CHECK_LENGTH(s, 0);
}

void test3()
{
    NSString *s = [[NSString alloc] initWithBytes:"123" length:3 encoding:NSASCIIStringEncoding];
    CHECK_LENGTH(s, 3);
    CHECK_EQUAL(s, "123");
}

void test4()
{
#define S4 "This is an UTF-8 string (Это строка в кодировке UTF-8)"
    NSString *s = [[NSString alloc] initWithUTF8String:S4];
    CHECK_EQUAL(s, S4);
}

void test5()
{
    NSString *s = [[NSString alloc] initWithFormat:@"%s %d %@, %zu", "A", 1, @"B", (NSUInteger)6];
    CHECK_EQUAL(s, "A 1 B, 6");
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
