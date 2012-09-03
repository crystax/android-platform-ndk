/*
 * Copyright (c) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>.
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
 * THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Dmitry Moskalchuk.
 */

#ifndef _CRYSTAX_FILEIO_COMMON_H_99544c48e9174f659a97671e7f64c763
#define _CRYSTAX_FILEIO_COMMON_H_99544c48e9174f659a97671e7f64c763

#if defined(CRYSTAX_FILEIO_DEBUG) && CRYSTAX_FILEIO_DEBUG
#ifdef CRYSTAX_DEBUG
#undef CRYSTAX_DEBUG
#endif
#define CRYSTAX_DEBUG 1
#endif

#include <crystax.h>
#include <crystax/jutils.hpp>
#include "crystax/private.h"
#include "crystax/lock.hpp"
#include "crystax/path.hpp"

#define NOT_IMPLEMENTED_BASE \
    WARN("FUNCTION IS NOT IMPLEMENTED!!!"); \
    errno = EIO

#define NOT_IMPLEMENTED \
    NOT_IMPLEMENTED_BASE; \
    return -1
#define NOT_IMPLEMENTED_NULL \
    NOT_IMPLEMENTED_BASE; \
    return NULL

#endif /* _CRYSTAX_FILEIO_COMMON_H_99544c48e9174f659a97671e7f64c763 */
