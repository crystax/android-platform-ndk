/*
 * Copyright (c) 2011-2015 CrystaX .NET.
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

#ifndef __CRYSTAX_INCLUDE_MACHINE_FENV_H_C7639E168FB34B38977528C2991EB5C5
#define __CRYSTAX_INCLUDE_MACHINE_FENV_H_C7639E168FB34B38977528C2991EB5C5

#include <crystax/id.h>

#if __arm__
# define _FENV_H_
# if __SOFTFP__
#  include <crystax/sys/fenvsoft.h>
# else
#  include <crystax/arm/fenv.h>
# endif
#elif __aarch64__
# include <crystax/arm64/fenv.h>
#elif __mips__ && !__mips64
# include <crystax/mips/fenv.h>
#elif __mips64
# include <crystax/mips64/fenv.h>
#elif __i386__
# include <crystax/x86/fenv.h>
#elif __x86_64__
# include <crystax/x86_64/fenv.h>
#else
# error "Not defined for this ABI"
#endif

#endif /* __CRYSTAX_INCLUDE_MACHINE_FENV_H_C7639E168FB34B38977528C2991EB5C5 */
