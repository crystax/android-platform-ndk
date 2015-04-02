#!/usr/bin/env ruby
#
# Build GIT to use it with Crystax NDK
#
# Copyright (c) 2015 CrystaX .NET.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation
# are those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of CrystaX .NET.
#

MINGW_CONFIG_MAK= <<-EOS
pathsep = ;

NO_ICONV                    = YesPlease
NO_REGEX                    = YesPlease
NO_PYTHON                   = YesPlease
NO_POLL                     = YesPlease
NO_NSEC                     = YesPlease
NO_UNIX_SOCKETS             = YesPlease
NO_PREAD                    = YesPlease
NO_SETENV                   = YesPlease
NO_MEMMEM                   = YesPlease
NO_POSIX_GOODIES            = UnfortunatelyYes
NO_PERL                     = YesPlease
NO_TCLTK                    = YesPlease
NO_STRCASESTR               = YesPlease
NO_ST_BLOCKS_IN_STRUCT_STAT = YesPlease
NO_D_INO_IN_DIRENT          = YesPlease
NO_MKDTEMP                  = YesPlease
NO_STRLCPY                  = YesPlease
NO_MKSTEMPS                 = YesPlease

USE_WIN32_MMAP    = YesPlease

NEEDS_CRYPTO_WITH_SSL = YesPlease

BASIC_CFLAGS  += -DPROTECT_NTFS_DEFAULT=1
COMPAT_CFLAGS += -Icompat/win32 -D__USE_MINGW_ACCESS -DSTRIP_EXTENSION='".exe"'
COMPAT_OBJS   += compat/mingw.o compat/winansi.o
COMPAT_OBJS   += compat/win32/pthread.o compat/win32/syslog.o compat/win32/dirent.o

PTHREAD_LIBS =
MINGW_LIBS  += -lws2_32 -lgdi32

NATIVE_CRLF                  = YesPlease
OBJECT_CREATION_USES_RENAMES = UnfortunatelyNeedsTo
UNRELIABLE_FSTAT             = UnfortunatelyYes
RUNTIME_PREFIX               = YesPlease

X = .exe
EOS


### COMPAT_CFLAGS += -DNOGDI -Icompat

### HAVE_ALLOCA_H = YesPlease
### NO_LIBGEN_H = YesPlease
### NO_SYMLINK_HEAD = YesPlease
### NO_STRTOUMAX = YesPlease
### NO_PERL_MAKEMAKER = YesPlease
### ETAGS_TARGET = ETAGS
### NO_INET_PTON = YesPlease
### DEFAULT_HELP_FORMAT = html
### BASIC_LDFLAGS += -Wl,--large-address-aware
### GITLIBS += git.res
### RC = windres -O coff
### SPARSE_FLAGS = -Wno-one-bit-signed-bitfield
### USE_NED_ALLOCATOR = YesPlease


require_relative 'versions.rb'

module Crystax

  PKG_NAME = 'git'

end

require 'fileutils'
require_relative 'logger.rb'
require_relative 'commander.rb'
require_relative 'builder.rb'
require_relative 'cache.rb'


def create_config_mak
  raise "target OS must be windows" unless Common.target_os == 'windows'
  file = File.open('config.mak', 'w')
  file.puts MINGW_CONFIG_MAK
  file.close
end


begin
  Common.parse_options

  Logger.open_log_file Common.log_file
  archive = Common.make_archive_name
  Logger.msg "building #{archive}; args: #{ARGV}"

  if Cache.try?(archive)
    Logger.msg "done"
    exit 0
  end

  if Common.target_os == 'windows'
    libsdir = "#{Common::BUILD_BASE}/libs"
    FileUtils.mkdir_p([Common::BUILD_BASE, "#{libsdir}/lib", "#{libsdir}/include"])
    Builder.build_zlib(libsdir)
  end

  openssldir = Builder.prepare_dependency('openssl')
  curldir = Builder.prepare_dependency('curl')

  Logger.msg "= building git"
  # todo: check that the specified version and the repository version are the same
  # since git does not support builds in non-source directory
  # we must copy source to build directory
  Builder.copy_sources
  FileUtils.mkdir_p(Common::BUILD_DIR)
  FileUtils.cd(Common::BUILD_DIR) do
    env = {'V' => '1',
           'NO_SVN_TESTS' => '1',
           'CURLDIR' => curldir,
           'OPENSSLDIR' => openssldir,
           'NO_R_TO_GCC_LINKER' => '1',
           'NO_EXPAT' => '1',
           'NO_GETTEXT' => '1',
           'NO_PERL' => '1',
           'NO_PYTHON' => '1',
           'NEEDS_SSL_WITH_CURL' => '1',
           'NEEDS_CRYPTO_WITH_SSL' => '1'
          }
    cflags = "#{Builder.cflags} -DCURL_STATICLIB"
    args = ["CC=#{Builder.cc}"]
    case Common.target_os
    when 'darwin'
      args << "LDFLAGS=-L#{openssldir}/lib"
      env["NO_FINK"] = "1"
      env["NO_DARWIN_PORTS"] = "1"
      env["NEEDS_LIBICONV"] = "1"
    when 'linux'
      args << "LDFLAGS=-ldl"
      env['NEEDS_CRYPTO_WITH_SSL'] = '1'
    when 'windows'
      env['ZLIB_PATH'] = libsdir
      env['GIT_CROSS_COMPILE'] = '1'
      cflags += " -D_POSIX"
      create_config_mak
      if Common.target_cpu == 'x86'
        cflags += "-D_USE_32BIT_TIME_T"
        env['NO_INET_NTOP'] = '1'
      end
    end
    args << "CFLAGS=\"#{cflags}\""

    Commander.run env, "make install -j #{Common::num_jobs} prefix=#{Common::BUILD_BASE}/git #{args.join(' ')}"

    if Common.target_os == 'darwin'
      FileUtils.cd('contrib/credential/osxkeychain') do
        Commander.run env, "make #{args.join(' ')}"
        FileUtils.cp 'git-credential-osxkeychain', "#{Common::BUILD_BASE}/git/bin"
      end
    end
  end

  Cache.add(archive)
  Cache.unpack(archive) if Common.same_platform?

rescue SystemExit => e
  exit e.status
rescue Exception => e
  Logger.log_exception(e)
  exit 1
else
  Builder.clean
ensure
  Logger.close_log_file
end
