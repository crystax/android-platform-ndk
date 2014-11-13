#import <stdio.h>

#if __clang__
void (^testBlock)(void) = ^{
    printf("Hello world!\n");
};
#endif

int main()
{
#if __clang__
    testBlock();
#endif
    return 0;
}
