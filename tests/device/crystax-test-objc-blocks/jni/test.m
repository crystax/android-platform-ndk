#import <stdio.h>

void (^testBlock)(void) = ^{
    printf("Hello world!\n");
};

int main()
{
    testBlock();
    return 0;
}
