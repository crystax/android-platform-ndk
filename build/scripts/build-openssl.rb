#!/usr/bin/env ruby
#
# Build OpenSSL libraries to use it to build Crystax NDK utilities
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

  PKG_NAME = 'openssl'

end


require 'fileutils'
require_relative 'logger.rb'
require_relative 'commander.rb'
require_relative 'builder.rb'
require_relative 'cache.rb'
require_relative 'exceptions.rb'


def openssl_platform
  case Common.target_platform
  when 'darwin-x86_64'  then 'darwin64-x86_64-cc'
  when 'darwin-x86'     then 'darwin-i386-cc'
  when 'linux-x86_64'   then 'linux-x86_64'
  when 'linux-x86'      then 'linux-generic32'
  when 'windows-x86_64' then 'mingw64'
  when 'windows-x86'    then 'mingw'
  else
    raise UnknownTargetPlatform, Common.target_platform, caller
  end
end


begin
  Common.parse_options

  Logger.open_log_file Common.log_file
  archive = Common.make_archive_name

  if Cache.try?(archive, :nounpack)
    Logger.msg "done"
    exit 0
  end

  Logger.msg "building #{archive}; args: #{ARGV}"
  # todo: check that the specified version and the repository version are the same
  # since openssl does not support builds in non-source directory
  # we must copy source to build directory
  Builder.copy_sources
  FileUtils.cd(Common::BUILD_DIR) do
    env = { 'CC' => Builder.cc }
    args = ["--prefix=#{Common::INSTALL_DIR}",
            "no-idea",
            "no-mdc2",
            "no-rc5",
            "no-shared",
            openssl_platform,
            Builder.cflags
           ]
    Commander::run env, "#{Common::SRC_DIR}/Configure #{args.join(' ')}"
    Commander::run "make depend"
    Commander::run "make" # -j N breaks build on OS X
    Commander::run "make test" unless Common.no_check? or Common.different_os?
    Commander::run "make install"
  end

  # no need to unpack archive
  # it'll be unpacked in build-ruby.rb
  Cache.add(archive)

rescue SystemExit => e
  exit e.status
rescue Exception => e
  Logger.log_exception(e)
  exit 1
else
  FileUtils.remove_dir(Common::BUILD_BASE, true) unless Common.no_clean?
ensure
  Logger.close_log_file
end
