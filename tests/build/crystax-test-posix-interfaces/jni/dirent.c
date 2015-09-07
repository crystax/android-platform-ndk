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

#include <dirent.h>
#if __linux__ && !__ANDROID__
#include <sys/stat.h> /* for ino_t */
#endif

#include "helper.h"

#define CHECK(type) type JOIN(dirent_check_type_, __LINE__)

CHECK(DIR*);
CHECK(ino_t);
CHECK(struct dirent);

void dirent_check_dirent_fields()
{
    struct dirent d;

#if __XSI_VISIBLE
    d.d_ino = 0;
#endif
    d.d_name[0] = '\0';

    (void)d;
}

#if __APPLE__ && !defined(__MAC_10_8)
typedef int (*scandir_third_argument_t)(struct dirent *);
typedef int (*scandir_fourth_argument_t)(const void *, const void *);
#else
typedef int (*scandir_third_argument_t)(const struct dirent *);
typedef int (*scandir_fourth_argument_t)(const struct dirent **, const struct dirent **);
#endif

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

void seekdir(DIR *d, long l)
{
    (void)d;
    (void)l;
}

long telldir(DIR *d)
{
    (void)d;
    return 0;
}

#endif /* __ANDROID__ */

void dirent_check_functions()
{
    (void)alphasort((const struct dirent **)1234, (const struct dirent **)1234);
    (void)closedir((DIR*)1234);
    (void)dirfd((DIR*)1234);
#if HAVE_FDOPENDIR
    (void)fdopendir(0);
#endif
    (void)opendir("");
    (void)readdir((DIR*)1234);
    (void)readdir_r((DIR*)1234, (struct dirent *)1234, (struct dirent **)1234);
    (void)rewinddir((DIR*)1234);
    (void)scandir((const char *)1234, (struct dirent ***)1234,
            (scandir_third_argument_t)0, (scandir_fourth_argument_t)0);
#if __XSI_VISIBLE
    (void)seekdir((DIR*)0, 0);
    (void)telldir((DIR*)0);
#endif
}
