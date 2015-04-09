#ifndef MYASSERT_H_6C8D79949B9E46C4ABF9CE9B5D69738D
#define MYASSERT_H_6C8D79949B9E46C4ABF9CE9B5D69738D

#include <stdio.h>
#include <stdlib.h>

#ifdef assert
#undef assert
#endif

#define assert(x) \
    do { \
        if (!(x)) \
        { \
            fprintf(stderr, "ASSERTION ERROR at %s:%d: %s\n", __FILE__, __LINE__, #x); \
            abort(); \
        } \
    } while (0)

#endif /* MYASSERT_H_6C8D79949B9E46C4ABF9CE9B5D69738D */
