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

#include <termios.h>

#include "gen/termios.inc"

#include "helper.h"

#define CHECK(type) type JOIN(termios_check_type_, __LINE__)

CHECK(cc_t);
CHECK(speed_t);
CHECK(tcflag_t);
CHECK(struct termios);
CHECK(pid_t);

void termios_check_fields(struct termios *s)
{
    s->c_iflag = (tcflag_t)0;
    s->c_oflag = (tcflag_t)0;
    s->c_cflag = (tcflag_t)0;
    s->c_lflag = (tcflag_t)0;
    s->c_cc[0] = (cc_t)0;
}

void termios_check_functions(struct termios *t, const struct termios *ct)
{
    (void)cfgetispeed(ct);
    (void)cfgetospeed(ct);
    (void)cfsetispeed(t, (speed_t)0);
    (void)cfsetospeed(t, (speed_t)0);
    (void)tcdrain(0);
    (void)tcflow(0, 0);
    (void)tcflush(0, 0);
    (void)tcgetattr(0, t);
    (void)tcgetsid(0);
    (void)tcsendbreak(0, 0);
    (void)tcsetattr(0, 0, ct);
}
