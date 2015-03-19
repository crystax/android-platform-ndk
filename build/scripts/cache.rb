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

require 'fileutils'
require_relative 'common.rb'
require_relative 'logger.rb'
require_relative 'commander.rb'


module Cache
  PATH = "/var/tmp/ndk-cache-#{ENV['USER']}"

  def self.try?(archive)
    if !exists?(archive)
      false
    else
      Logger.msg "found cached file: #{archive}"
      unpack(archive)
      true
    end
  end

  def self.exists?(archive)
    File.exists? "#{PATH}/#{archive}"
  end

  def self.unpack(archive, pkgname = Crystax::PKG_NAME, dstdir = Common::DST_DIR)
    FileUtils.remove_dir("#{dstdir}/#{pkgname}", true)
    Commander::run "7z x -o#{dstdir} #{PATH}/#{archive}"
  end

  def self.add(archive)
    FileUtils.cd(Common::BUILD_BASE) { Commander::run "7z a #{archive} #{Crystax::PKG_NAME}" }
    FileUtils.move("#{Common::BUILD_BASE}/#{archive}", PATH)
  end
end
