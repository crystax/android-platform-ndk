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

static int initialized = 0;
static void *libc = NULL;

#define atomic_fetch(p) __sync_add_and_fetch(p, 0)

#define def_atomic_swap(type, suffix) \
    static type atomic_swap_ ## suffix ( type v, type *ptr) \
    { \
        type prev; \
        do { \
            prev = *ptr; \
        } while (__sync_val_compare_and_swap(ptr, prev, v) != prev); \
        return prev; \
    }

def_atomic_swap(int, i)
def_atomic_swap(void *, p)

void * __crystax_bionic()
{
    if (atomic_fetch(&initialized) == 0)
    {
        void *pc;

        pc = dlopen("libc.so", RTLD_NOW);
        if (!pc)
            PANIC("dlopen(\"libc.so\") failed");

        atomic_swap_p(pc, &libc);
        atomic_swap_i(1, &initialized);
    }

    return atomic_fetch(&libc);
}

void * __crystax_bionic_symbol(const char *name)
{
    void *pc = __crystax_bionic();
    if (!pc) PANIC("__crystax_bionic() failed");
    return dlsym(pc, name);
}
