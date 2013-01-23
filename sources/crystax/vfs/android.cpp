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

#include "crystax/private.h"

namespace crystax
{

namespace fileio
{
namespace osfs
{
bool init(JNIEnv *env);
} // namespace osfs
} // namespace fileio

} // namespace crystax

CRYSTAX_GLOBAL __attribute__ ((constructor))
void __crystax_vfs_on_load()
{
    DBG("initialize vfs module");
    if (__crystax_fileio_init() < 0)
    {
        ERR("vfs initialization failed");
        ::abort();
    }
}

CRYSTAX_GLOBAL __attribute__ ((destructor))
void __crystax_vfs_on_unload()
{
    DBG("deinitialize vfs module");
}

CRYSTAX_GLOBAL
jint crystax_vfs_jni_on_load(JavaVM *vm)
{
    jint jversion = JNI_VERSION_1_4;

    JNIEnv *env = crystax_jnienv();

    TRACE;
    if (!::crystax::fileio::osfs::init(env))
    {
        ERR("can't init osfs");
        return -1;
    }

    TRACE;
    return jversion;
}

CRYSTAX_GLOBAL
void crystax_vfs_jni_on_unload(JavaVM * /* vm */)
{
    TRACE;
}
