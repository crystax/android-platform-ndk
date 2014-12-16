/*
 * Copyright (c) 2011-2014 CrystaX .NET.
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

#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#include <crystax/private.h>
#include <crystax/atomic.h>
#include <crystax/bionic.h>

static void *libc = NULL;

static const char *symbols[] = {
#ifdef DEF
#error Macro DEF is already defined
#endif
#define DEF(x, y) y ,
#include "crystax/details/bionic.inc"
#undef DEF
};

typedef struct {
    int initialized;
    void *addr;
} addr_t;

static addr_t addresses[] = {
#ifdef DEF
#error Macro DEF is already defined
#endif
#define DEF(x, y) {.initialized = 0, .addr = NULL} ,
#include "crystax/details/bionic.inc"
#undef DEF
};

static void * __crystax_bionic()
{
    if (__crystax_atomic_fetch(&libc) == NULL)
    {
        void *pc;

        pc = dlopen("libc.so", RTLD_NOW);
        if (!pc)
            PANIC("dlopen(\"libc.so\") failed");

        __crystax_atomic_swap(&libc, pc);
    }

    return __crystax_atomic_fetch(&libc);
}

void * __crystax_bionic_symbol(__crystax_bionic_symbol_t sym, int maynotexist)
{
    void *addr;
    addr_t *ar;

    if (sym < 0 || sym >= sizeof(symbols)/sizeof(symbols[0]))
        PANIC("Wrong __crystax_bionic_symbol_t passed to __crystax_bionic_symbol()");

    ar = &addresses[sym];
    if (__crystax_atomic_fetch(&(ar->initialized)) == 0)
    {
        const char *symname = symbols[sym];
        addr = dlsym(__crystax_bionic(), symname);
        if (!addr && !maynotexist)
            PANIC("Can't find symbol \"%s\" in Bionic libc.so", symname);
        __crystax_atomic_swap(&(ar->addr), addr);
        __crystax_atomic_swap(&(ar->initialized), 1);
    }
    else
        addr = __crystax_atomic_fetch(&(ar->addr));

    return addr;
}
