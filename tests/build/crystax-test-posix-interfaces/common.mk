# Copyright (c) 2011-2014 CrystaX .NET.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX .NET.

SRCFILES :=        \
	aio.c          \
	arpa_inet.c    \
	complex.c      \
	cpio.c         \
	ctype.c        \
	dirent.c       \
	dlfcn.c        \
	errno.c        \
	fcntl.c        \
	fenv.c         \
	float.c        \
	fmtmsg.c       \
	fnmatch.c      \
	ftw.c          \
	glob.c         \
	grp.c          \
	iconv.c        \
	inttypes.c     \
	iso646.c       \
	langinfo.c     \
	libgen.c       \
	limits.c       \
	locale.c       \
	main.c         \
	math.c         \
	monetary.c     \
	mqueue.c       \
	ndbm.c         \
	net_if.c       \
	netdb.c        \
	netinet_in.c   \
	netinet_tcp.c  \
	nl_types.c     \
	poll.c         \
	pthread.c      \
	pwd.c          \
	regex.c        \
	search.c       \
	semaphore.c    \
	setjmp.c       \
	signal.c       \
	spawn.c        \
	stdarg.c       \
	stdbool.c      \
	stddef.c       \
	stdint.c       \
	stdio.c        \
	stdlib.c       \
	string.c       \
	strings.c      \
	sys_ipc.c      \
	sys_mman.c     \
	sys_msg.c      \
	sys_resource.c \
	sys_select.c   \
	sys_sem.c      \
	sys_shm.c      \
	sys_socket.c   \
	sys_stat.c     \
	sys_statvfs.c  \
	sys_time.c     \
	sys_times.c    \
	sys_types.c    \
	sys_uio.c      \
	sys_un.c       \
	sys_utsname.c  \
	sys_wait.c     \
	syslog.c       \
	tar.c          \
	termios.c      \
	tgmath.c       \
	time.c         \
	unistd.c       \
	utime.c        \
	wchar.c        \
	wctype.c       \
	wordexp.c      \
	xlocale.c      \
	xlocale2.c     \

SRCFILES += checkm.c

CFLAGS := -Wall -Wextra -Werror -Wno-unused-result
