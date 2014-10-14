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

#ifndef __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDLIB_H_9B27D849A6AE48E1B4DC99764925FA50
#define __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDLIB_H_9B27D849A6AE48E1B4DC99764925FA50

#include <crystax/id.h>
/* Enable extended locale interfaces */
#include <xlocale.h>

#define __LIBCRYSTAX_STDLIB_H_XLOCALE_H_INCLUDED 1

/* For WEXITSTATUS, WIFEXITED and other constants as required by IEEE Std 1003.1, 2013 Edition */
#include <sys/wait.h>

#include <malloc.h>

__BEGIN_DECLS

int ptsname_r(int, char*, size_t);
int getpt(void);
int clearenv(void);

static __inline__ int  rand() { return (int)lrand48(); }
static __inline__ void srand(unsigned int s) { srand48(s); }
static __inline__ long random() { return lrand48(); }
static __inline__ void srandom(unsigned long s) { srand48(s); }

__END_DECLS

#endif /* __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDLIB_H_9B27D849A6AE48E1B4DC99764925FA50 */
