extern int test_objc();
extern int test_objcpp();

int main()
{
    if (!test_objc())
        return 1;
    if (!test_objcpp())
        return 1;
    return 0;
}
