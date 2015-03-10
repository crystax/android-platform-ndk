#!/usr/bin/env ruby
#
# Build RUBY to use it with Crystax NDK
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


require 'fileutils'


module Crystax

  PKG_VERSION = '2.2.1'
  PKG_NAME = 'ruby'

end


require_relative 'common.rb'
require_relative 'logger.rb'
require_relative 'commander.rb'
require_relative 'builder.rb'
require_relative 'cache.rb'


begin
  archive = Common.make_archive_name
  Logger.open_log_file Common::LOG_FILE

  if Cache.try?(archive)
    Logger.msg "done"
    exit 0
  end

  Logger.msg "building #{archive}"
  FileUtils.mkdir_p(Common::BUILD_DIR)
  FileUtils.cd(Common::BUILD_DIR) do
    env = { 'CC' => Builder.cc(Common::TARGET_PLATFORM),
            'CFLAGS' => Builder.cflags(Common::TARGET_PLATFORM),
            'LDFLAGS' => Builder.ldflags(Common::TARGET_PLATFORM)
          }
    Commander::run env, "#{Common::SRC_DIR}/configure --prefix=#{Common::INSTALL_DIR} --disable-install-doc"

    Commander::run "make -j #{Common::NUM_JOBS}"
    Commander::run "make check"
    Commander::run "make install"
  end

  gems = ['rspec', 'minitest']
  Commander::run "#{Common::INSTALL_DIR}/bin/gem install #{gems.join(' ')}"

  Cache.add(archive)
  Cache.unpack(archive)

rescue SystemExit => e
  exit e.status
rescue Exception => e
  Logger.log_exception(e)
  exit 1
else
  FileUtils.remove_dir(Common::BUILD_BASE, true)
ensure
  Logger.close_log_file
end
