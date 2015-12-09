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

#ifndef __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDIO_LOCAL_H_6FB9C37DEC3846C391A863E327B58815
#define __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDIO_LOCAL_H_6FB9C37DEC3846C391A863E327B58815

#include <crystax/id.h>

/*
 * Redefine stdio local functions/variables to avoid link-time conflicts with Bionic
 */
#define __fcloseall  __crystax___fcloseall
#define __fflush     __crystax___fflush
#define __fgetwc_mbs __crystax___fgetwc_mbs
#define __fputwc     __crystax___fputwc
#define __fread      __crystax___fread
#define __sclose     __crystax___sclose
#define __sdidinit   __crystax___sdidinit
#define __sflags     __crystax___sflags
#define __sflush     __crystax___sflush
#define __sfp        __crystax___sfp
#define __sglue      __crystax___sglue
#define __sinit      __crystax___sinit
#define __slbexpand  __crystax___slbexpand
#define __smakebuf   __crystax___smakebuf
#define __sread      __crystax___sread
#define __srefill    __crystax___srefill
#define __srget      __crystax___srget
#define __sseek      __crystax___sseek
#define __svfscanf   __crystax___svfscanf
#define __swbuf      __crystax___swbuf
#define __swhatbuf   __crystax___swhatbuf
#define __swrite     __crystax___swrite
#define __swsetup    __crystax___swsetup
#define __ungetc     __crystax___ungetc
#define __ungetwc    __crystax___ungetwc
#define __vfprintf   __crystax___vfprintf
#define __vfscanf    __crystax___vfscanf
#define __vfwprintf  __crystax___vfwprintf
#define __vfwscanf   __crystax___vfwscanf
#define _cleanup     __crystax__cleanup
#define _fseeko      __crystax__fseeko
#define _ftello      __crystax__ftello
#define _fwalk       __crystax__fwalk
#define _sread       __crystax__sread
#define _sseek       __crystax__sseek
#define _swrite      __crystax__swrite

#endif /* __CRYSTAX_INCLUDE_CRYSTAX_SYS_STDIO_LOCAL_H_6FB9C37DEC3846C391A863E327B58815 */
