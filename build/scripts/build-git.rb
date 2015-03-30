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

require_relative 'versions.rb'

module Crystax

  PKG_NAME = 'git'
  PKG_VERSION = version PKG_NAME
end

require 'fileutils'
require_relative 'logger.rb'
require_relative 'commander.rb'
require_relative 'builder.rb'
require_relative 'cache.rb'


begin
  Common.parse_options

  Logger.open_log_file Common.log_file
  archive = Common.make_archive_name
  Logger.msg "building #{archive}; args: #{ARGV}"

  if Cache.try?(archive)
    Logger.msg "done"
    exit 0
  end

  # if Common.target_os == 'windows'
  #   libsdir = "#{Common::BUILD_BASE}/libs"
  #   FileUtils.mkdir_p([Common::BUILD_BASE, "#{libsdir}/lib", "#{libsdir}/include"])
  #   build_libffi(libsdir)
  #   build_zlib(libsdir)
  # end

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
           'OPENSSLDIR' => openssldir
          }
    # if Common::target_os == 'windows'
    #   args << '--host=x86_64-mingw64'
    #   env['PATH'] = Builder.toolchain_path_and_path
    #   env['CFLAGS'] += " -I#{libsdir}/include"
    #   env['LDFLAGS'] = "-L#{libsdir}/lib"
    # end
    if Common.target_os == 'darwin'
      env["NO_EXPAT"] = "1"
      env["NO_GETTEXT"] = "1"
      env["NO_FINK"] = "1"
      env["NO_DARWIN_PORTS"] = "1"
      env["NO_R_TO_GCC_LINKER"] = "1"
      env["NEEDS_SSL_WITH_CURL"] = "1"
      env["NEEDS_LIBICONV"] = "1"
    end
    args = ["CC=#{Builder.cc}", "CFLAGS=\"#{Builder.cflags}\""]

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
