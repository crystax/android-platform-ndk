#include <stdio.h>

void test_c();
void test_cpp();

int main(void)
{
    printf("Test wchar support in C...\n");
    test_c();
    printf("Test wchar support in C++...\n");
    test_cpp();
    return 0;
}

