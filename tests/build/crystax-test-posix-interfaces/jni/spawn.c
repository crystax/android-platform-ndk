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

#if _POSIX_SPAWN > 0

#include <spawn.h>

#include "gen/spawn.inc"

#include "helper.h"

#define CHECK(type) type JOIN(spawn_check_type_, __LINE__)

CHECK(posix_spawnattr_t);
CHECK(posix_spawn_file_actions_t);
CHECK(mode_t);
CHECK(pid_t);
CHECK(sigset_t);
CHECK(struct sched_param *);

void spawn_check_functions(posix_spawn_file_actions_t *f, posix_spawnattr_t *a)
{
    (void)posix_spawn((pid_t*)0, "", f, a, (char * const *)0, (char * const *)0);
    (void)posix_spawn_file_actions_addclose(f, 0);
    (void)posix_spawn_file_actions_adddup2(f, 0, 0);
    (void)posix_spawn_file_actions_addopen(f, 0, "", 0, (mode_t)0);
    (void)posix_spawn_file_actions_destroy(f);
    (void)posix_spawn_file_actions_init(f);
    (void)posix_spawnattr_destroy(a);
    (void)posix_spawnattr_getflags(a, (short*)0);
    (void)posix_spawnattr_getpgroup(a, (pid_t*)0);
#if _POSIX_PRIORITY_SCHEDULING > 0
    (void)posix_spawnattr_getschedparam(a, (struct sched_param*)0);
    (void)posix_spawnattr_getschedpolicy(a, (int*)0);
#endif
    (void)posix_spawnattr_getsigdefault(a, (sigset_t*)0);
    (void)posix_spawnattr_getsigmask(a, (sigset_t*)0);
    (void)posix_spawnattr_init(a);
    (void)posix_spawnattr_setflags(a, (short)0);
    (void)posix_spawnattr_setpgroup(a, (pid_t)0);
#if _POSIX_PRIORITY_SCHEDULING > 0
    (void)posix_spawnattr_setschedparam(a, (const struct sched_param *)0);
    (void)posix_spawnattr_setschedpolicy(a, 0);
#endif
    (void)posix_spawnattr_setsigdefault(a, (const sigset_t *)0);
    (void)posix_spawnattr_setsigmask(a, (const sigset_t *)0);
    (void)posix_spawnp((pid_t*)0, "", f, a, (char * const *)0, (char * const *)0);
}

#endif /* _POSIX_SPAWN > 0 */
