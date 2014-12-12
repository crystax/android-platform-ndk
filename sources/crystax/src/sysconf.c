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

#include <sys/sysconf.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <crystax/private.h>

void * __crystax_bionic_symbol(const char *name);

static long crystax_sysconf(int name)
{
    switch (name)
    {
        case _SC_2_PBS:
        case _SC_2_PBS_ACCOUNTING:
        case _SC_2_PBS_CHECKPOINT:
        case _SC_2_PBS_LOCATE:
        case _SC_2_PBS_MESSAGE:
        case _SC_2_PBS_TRACK:
            return -1;
        case _SC_REGEXP:
            return _POSIX_REGEXP;
        case _SC_READER_WRITER_LOCKS:
            return _POSIX_READER_WRITER_LOCKS;
        case _SC_TIMEOUTS:
            return _POSIX_TIMEOUTS;
        default:
            errno = EINVAL;
            return -1;
    }
}

long sysconf(int name)
{
    static long (*bionic_sysconf)(int name);

    if (name & __CRYSTAX_SC_BASE)
        return crystax_sysconf(name);

    bionic_sysconf = __crystax_bionic_symbol("sysconf");

    if (!bionic_sysconf)
        PANIC("bionic_sysconf() failed");

    return bionic_sysconf(name);
}
