/*
 * Copyright (c) 2011-2015 CrystaX.
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

#ifndef __CRYSTAX_INCLUDE_UNISTD_H_03C293AE50EC4668ABCC15CAF1E1046F
#define __CRYSTAX_INCLUDE_UNISTD_H_03C293AE50EC4668ABCC15CAF1E1046F

#include <crystax/id.h>
#include <sys/limits.h>

#define getpagesize crystax_google_getpagesize
#include <crystax/google/unistd.h>
#undef getpagesize

#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200112
#define F_ULOCK     0   /* unlock locked section */
#define F_LOCK      1   /* lock a section for exclusive use */
#define F_TLOCK     2   /* test and lock a section for exclusive use */
#define F_TEST      3   /* test a section for locks by other procs */
#endif

__BEGIN_DECLS

#if __XSI_VISIBLE
int lockf(int, int, off_t);
#endif

int issetugid();
int getdtablesize();

int pipe2(int *, int);

int getpagesize(void);

__END_DECLS

#endif /* __CRYSTAX_INCLUDE_UNISTD_H_03C293AE50EC4668ABCC15CAF1E1046F */
