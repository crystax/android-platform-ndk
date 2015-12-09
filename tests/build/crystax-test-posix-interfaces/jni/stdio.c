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

#include <stdio.h>
#if __APPLE__ || __gnu_linux__
#include <stdarg.h> /* for va_start */
#endif
#if __APPLE__
#include <xlocale.h>
#if !defined(__MAC_10_7)
#include <sys/types.h>
#endif
#endif
#if __gnu_linux__
#include <locale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_STDIO_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in stdio.h
#endif
#endif

#include "helper.h"

#define CHECK(type) type JOIN(stdio_check_type_, __LINE__)

CHECK(FILE);
CHECK(fpos_t);
CHECK(off_t);
CHECK(size_t);
CHECK(ssize_t);
CHECK(va_list);
CHECK(locale_t);

void stdio_check_functions(FILE *fp, locale_t l, ...)
{
    int iv;
    va_list args;
    va_start(args, l);

#if __APPLE__ || __ANDROID__
    (void)asprintf((char**)1234, "%d", 0);
    (void)asprintf_l((char**)1234, l, "%d", 0);
#endif
    (void)clearerr(fp);
    (void)ctermid((char*)0);
#if !__APPLE__ || defined(__MAC_10_7)
    (void)dprintf(0, "%d", 0);
#endif
#if (__APPLE__ && defined(__MAC_10_7)) || __ANDROID__
    (void)dprintf_l(0, l, "%d", 0);
#endif
    (void)fclose(fp);
    (void)fdopen(0, "");
    (void)feof(fp);
    (void)ferror(fp);
    (void)fflush(fp);
    (void)fgetc(fp);
    (void)fgetpos(fp, (fpos_t*)0);
    (void)fgets((char*)0, 0, fp);
    (void)fileno(fp);
    (void)flockfile(fp);
#if !__APPLE__
    (void)fmemopen((void*)0, (size_t)0, (const char *)0);
#endif
    (void)fopen((const char *)0, (const char *)0);
    (void)fprintf(fp, "%d, %s", 0, "");
#if __APPLE__ || __ANDROID__
    (void)fprintf_l(fp, l, "%d, %s", 0, "");
#endif
    (void)fputc(0, fp);
    (void)fputs("", fp);
    (void)fread((void*)0, (size_t)0, (size_t)0, fp);
    (void)freopen((const char *)0, (const char *)0, fp);
    (void)fscanf(fp, "%d", &iv);
    (void)fseek(fp, (long)0, 0);
    (void)fseeko(fp, (off_t)0, 0);
    (void)fsetpos(fp, (const fpos_t *)0);
    (void)ftell(fp);
    (void)ftello(fp);
    (void)ftrylockfile(fp);
    (void)funlockfile(fp);
    (void)fwrite((const void *)1234, (size_t)0, (size_t)0, fp);
    (void)getc(fp);
    (void)getchar();
    (void)getc_unlocked(fp);
    (void)getchar_unlocked();
#if !__APPLE__ || defined(__MAC_10_7)
    (void)getdelim((char**)0, (size_t*)0, 0, fp);
    (void)getline((char**)0, (size_t*)0, fp);
#endif
#if !__gnu_linux__
    (void)gets((char*)0);
#endif
#if !__APPLE__
    (void)open_memstream((char**)0, (size_t*)0);
#endif
    (void)pclose(fp);
    (void)perror((const char *)0);
    (void)popen((const char *)0, (const char *)0);
    (void)printf("%d, %ld", 0, (long)0);
#if __APPLE__ || __ANDROID__
    (void)printf_l(l, "%d, %ld", 0, (long)0);
#endif
    (void)putc(0, fp);
    (void)putchar(0);
    (void)putc_unlocked(0, fp);
    (void)putchar_unlocked(0);
    (void)puts("1234");
    (void)remove((const char *)0);
    (void)rename((const char *)0, (const char *)0);
#if HAVE_XXXAT
    (void)renameat(0, (const char *)0, 0, (const char *)0);
#endif
    (void)rewind(fp);
    (void)scanf("%d", &iv);
    (void)setbuf(fp, (char*)0);
    (void)setvbuf(fp, (char*)0, 0, (size_t)0);
    (void)snprintf((char *)1234, (size_t)0, "%d", 0);
#if __APPLE__ || __ANDROID__
    (void)snprintf_l((char*)1234, (size_t)0, l, "%d", 0);
#endif
    (void)sprintf((char *)1234, "%d", 0);
#if __APPLE__ || __ANDROID__
    (void)sprintf_l((char*)1234, l, "%d", 0);
#endif
    (void)sscanf((const char *)1234, "%d", &iv);
    (void)tmpfile();
    (void)ungetc(0, fp);
#if __APPLE__ || __ANDROID__
    (void)vasprintf((char**)1234, "%d", args);
    (void)vasprintf_l((char**)1234, l, "%d", args);
#endif
#if !__APPLE__ || defined(__MAC_10_7)
    (void)vdprintf(0, "%d", args);
#endif
#if (__APPLE__ && defined(__MAC_10_7)) || __ANDROID__
    (void)vdprintf_l(0, l, "%d", args);
#endif
    (void)vfprintf(fp, "%s", args);
#if __APPLE__ || __ANDROID__
    (void)vfprintf_l(fp, l, "%s", args);
#endif
    (void)vfscanf(fp, "%d", args);
    (void)vprintf("%s", args);
#if __APPLE__ || __ANDROID__
    (void)vprintf_l(l, "%s", args);
#endif
    (void)vscanf("%d", args);
    (void)vsnprintf((char*)1234, (size_t)0, "%s", args);
#if __APPLE__ || __ANDROID__
    (void)vsnprintf_l((char*)1234, (size_t)0, l, "%s", args);
#endif
    (void)vsprintf((char*)1234, "%s", args);
#if __APPLE__ || __ANDROID__
    (void)vsprintf_l((char*)1234, l, "%s", args);
#endif
    (void)vsscanf((const char *)1234, "%d", args);
}
