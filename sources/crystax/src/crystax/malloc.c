/*
 * Copyright (c) 2011-2016 CrystaX.
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
 * THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX.
 */

#include <crystax/malloc.h>
#include <crystax/private.h>
#include <dlfcn.h>
#include <stddef.h>

#define CRYSTAX_DEFINE_FUNCTION(ret, params, name, args) \
    CRYSTAX_HIDDEN ret crystax_ ## name params \
    { \
        typedef ret (*func_t) params; \
        static func_t func = NULL; \
        if (!func) \
        { \
            func = (func_t) dlsym(__crystax_bionic_handle(), #name); \
            if (!func) PANIC("can't find '" #name "' in libc"); \
        } \
        return func args; \
    }

CRYSTAX_DEFINE_FUNCTION(void *, (size_t c, size_t s),           calloc,             (c, s))
CRYSTAX_DEFINE_FUNCTION(void *, (size_t s),                     malloc,             (s))
CRYSTAX_DEFINE_FUNCTION(void,   (void *p),                      free,               (p))
CRYSTAX_DEFINE_FUNCTION(void *, (size_t s),                     valloc,             (s))
CRYSTAX_DEFINE_FUNCTION(void *, (size_t a, size_t s),           memalign,           (a, s))
CRYSTAX_DEFINE_FUNCTION(size_t, (void const *p),                malloc_usable_size, (p))
CRYSTAX_DEFINE_FUNCTION(int,    (void **p, size_t a, size_t s), posix_memalign,     (p, a, s))
CRYSTAX_DEFINE_FUNCTION(void *, (size_t s),                     pvalloc,            (s))
CRYSTAX_DEFINE_FUNCTION(void *, (void *p, size_t s),            realloc,            (p, s))
