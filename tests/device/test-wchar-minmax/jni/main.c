/* __WCHAR_MAX__ is known to be predefined in GCC since 3.3 and it's
 * predefined in clang 3.1 too */
#ifdef __WCHAR_MAX__
int wchar_max_defined = 1;
#else
#error __WCHAR_MAX__ undefined!
#endif

/* __WCHAR_MIN__ is known to be predefined in GCC since 4.5;
 * it's NOT predefined in clang 3.1 */
#ifdef __WCHAR_MIN__
int wchar_min_defined = 1;
#else
int wchar_min_defined = 0;
#endif


/* __INT_MAX__ is the predefined macro in GCC */
#ifndef __INT_MAX__
#error __INT_MAX__ undefined!
#endif


/* now we can decide how wchat_t is treated for a particular platform */
#if (__INT_MAX__ == __WCHAR_MAX__)
const char *wchar_is = "signed";
#else
const char *wchar_is = "unsigned";
#endif

#include <stdio.h>

int main()
{
    printf("__WCHAR_MIN__ is defined: %s\n", (wchar_min_defined ? "yes" : "no"));
    printf("__WCHAR_MAX__ is defined: %s\n", (wchar_max_defined ? "yes" : "no"));
    printf("\n");
    printf("wchar_t is treated as '%s' type\n", wchar_is);

    return 0;
}
