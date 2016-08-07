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

#include <sys/mman.h>
#include <sys/prctl.h>
#include <fcntl.h>

#include "private/KernelArgumentBlock.h"

#undef  PAGE_SIZE
#define PAGE_SIZE 4096

extern "C"
{
    extern __noreturn void _exit(int);
    extern int ___close(int);
    extern int __openat(int, const char *, int, int);
    extern void * __mmap2(void *, size_t, int, int, int, size_t);
}

namespace crystax
{
namespace init
{

#define EARLY_ABORT_IF(cond) \
    do { \
        if (cond) { \
            ::crystax::init::early_abort(__LINE__); \
        } \
    } while (0)

__noreturn static void early_abort(int line)
{
    // We can't write to stdout or stderr because we're aborting before we've checked that
    // it's safe for us to use those file descriptors. We probably can't strace either, so
    // we rely on the fact that if we dereference a low address, either debuggerd or the
    // kernel's crash dump will show the fault address.
    *reinterpret_cast<int*>(line) = 0;
    _exit(line > 0 && line < 255 ? line : 255);
}

static void * memalloc(size_t length)
{
#if defined(__LP64__) || (defined(__x86_64__) && defined(__ILP32__))
    return ::mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
    return ::__mmap2(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}

static void const *readfile(const char *path, size_t *plength)
{
    int fd = ::__openat(AT_FDCWD, path, O_RDONLY, 0);
    EARLY_ABORT_IF(fd < 0);

    size_t length = 0;
    void *addr = 0;
    for (;;)
    {
        uint8_t buf[PAGE_SIZE];
        ssize_t n = ::read(fd, buf, sizeof(buf));
        EARLY_ABORT_IF(n < 0);
        if (n == 0)
            break;

        void *newaddr = memalloc(length + n);
        EARLY_ABORT_IF(newaddr == MAP_FAILED);
        if (addr)
        {
            ::memcpy(newaddr, addr, length);
            int rc = ::munmap(addr, length);
            EARLY_ABORT_IF(rc != 0);
        }
        ::memcpy(reinterpret_cast<uint8_t*>(newaddr) + length, buf, n);

        addr = newaddr;
        length += n;
    }
    int rc = ::___close(fd);
    EARLY_ABORT_IF(rc != 0);

    if (plength)
        *plength = length;

    return addr;
}

template <typename T>
static void put_to_rawargs(void *&rawargs, size_t &size, off_t &offset, T value)
{
    if (!rawargs) size = 0;

    if (offset + sizeof(value) > size)
    {
        void *newargs = memalloc(size + PAGE_SIZE);
        EARLY_ABORT_IF(newargs == MAP_FAILED);
        if (rawargs)
        {
            ::memcpy(newargs, rawargs, size);
            int rc = ::munmap(rawargs, size);
            EARLY_ABORT_IF(rc != 0);
        }
        rawargs = newargs;
        size += PAGE_SIZE;
        EARLY_ABORT_IF(offset + sizeof(value) > size);
    }

    ::memcpy(reinterpret_cast<uint8_t*>(rawargs) + offset, &value, sizeof(value));
    offset += sizeof(value);
}

static void *construct_rawargs_impl()
{
    size_t argslen = 0;
    char const *args = static_cast<char const *>(
            readfile("/proc/self/cmdline", &argslen));
    EARLY_ABORT_IF(argslen == 0);

    size_t envslen = 0;
    char const *envs = static_cast<char const *>(
            readfile("/proc/self/environ", &envslen));
    EARLY_ABORT_IF(envslen == 0);

    size_t auxvlen;
    ElfW(auxv_t) const *auxv = static_cast<ElfW(auxv_t) const *>(
            readfile("/proc/self/auxv", &auxvlen));
    EARLY_ABORT_IF(auxvlen == 0);

    void *rawargs = 0;
    size_t rawargssize = 0;
    off_t rawargsoffset = 0;

    int argc = 0;
    put_to_rawargs(rawargs, rawargssize, rawargsoffset, static_cast<uintptr_t>(argc));

    char const *ap = args;
    for (size_t idx = 0; idx < argslen; ++idx)
    {
        if (args[idx] != '\0') continue;
        put_to_rawargs(rawargs, rawargssize, rawargsoffset, ap);
        ++argc;
        ap = args + idx + 1;
    }

    *reinterpret_cast<uintptr_t*>(rawargs) = static_cast<uintptr_t>(argc);

    char const *ep = envs;
    for (size_t idx = 0; idx < envslen; ++idx)
    {
        if (envs[idx] != '\0') continue;
        put_to_rawargs(rawargs, rawargssize, rawargsoffset, ep);
        ep = envs + idx + 1;
    }
    put_to_rawargs(rawargs, rawargssize, rawargsoffset, static_cast<char const *>(0));

    for (ElfW(auxv_t) const *v = auxv; v->a_type != AT_NULL; ++v)
    {
        put_to_rawargs(rawargs, rawargssize, rawargsoffset, *v);
    }
    ElfW(auxv_t) end;
    end.a_type = AT_NULL;
    end.a_un.a_val = 0;
    put_to_rawargs(rawargs, rawargssize, rawargsoffset, end);

    return rawargs;
}

static void *construct_rawargs()
{
    static void *args = NULL;
    if (!args)
    {
        int dumpable = prctl(PR_GET_DUMPABLE, 0, 0, 0, 0);
        EARLY_ABORT_IF(dumpable < 0);

        int const DUMPABLE = 1;

        if (dumpable != DUMPABLE)
            EARLY_ABORT_IF(prctl(PR_SET_DUMPABLE, DUMPABLE, 0, 0, 0) < 0);

        args = construct_rawargs_impl();

        if (dumpable != DUMPABLE)
            EARLY_ABORT_IF(prctl(PR_SET_DUMPABLE, dumpable, 0, 0, 0) < 0);
    }
    return args;
}

} // namespace init
} // namespace crystax

extern "C"
void *__crystax_construct_rawargs()
{
    return ::crystax::init::construct_rawargs();
}
