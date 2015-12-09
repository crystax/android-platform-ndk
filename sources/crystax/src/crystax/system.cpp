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

#include <crystax/system.h>

#include <crystax/private.h>
#include <crystax/atomic.h>
#include <crystax/bionic.h>

#include <sys/system_properties.h>

#include <string.h>

static int devtype = -1;

CRYSTAX_GLOBAL
int crystax_device_type()
{
    static const char *generic = "generic";

    int type = __crystax_atomic_fetch(&devtype);
    if (type < 0)
    {
        typedef int (*syspropget_t)(const char *, char *);

        char brand[PROP_VALUE_MAX + 1];

        syspropget_t syspropget = (syspropget_t)__crystax_bionic_symbol(__CRYSTAX_BIONIC_SYMBOL___SYSTEM_PROPERTY_GET, 1);
        if (!syspropget || syspropget("ro.product.brand", brand) <= 0)
            type = CRYSTAX_DEVICE_TYPE_UNKNOWN;
        else if (memcmp(brand, generic, strlen(generic)) == 0)
            type = CRYSTAX_DEVICE_TYPE_EMULATOR;
        else
            type = CRYSTAX_DEVICE_TYPE_REAL;

        __crystax_atomic_swap(&devtype, type);
    }

    return type;
}
