#ifndef __CRYSTAX__
#error "__CRYSTAX__ macro is undefined"
#endif

#if __CRYSTAX__ != 1
#error "__CRYSTAX__ macro defined, but has value other than '1'"
#endif

int main()
{
    return 0;
}
