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

# common for all build scripts
require 'pathname'
require 'fileutils'


NDK_ROOT_DIR = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.dirname.dirname.to_s
NDK_DIR = "#{NDK_ROOT_DIR}/platform/ndk"
VENDOR_DIR = "#{NDK_ROOT_DIR}/vendor"

# todo: calculate target platform
TARGET_PLATFORM = "darwin-x86_64"


class CommandFailed < RuntimeError
  def initialize(cmd)
    @cmd = cmd
  end

  def to_s
    "command failed: #{@cmd}"
  end
end


# todo: use module?
class Logger
  def self.log_msg(msg)
    # todo: all
    puts msg
  end

  def self.log_exception(exc)
    # todo: all
    puts "error: #{exc}"
    puts exc.backtrace
  end
end


def run_cmd(cmd)
  Logger.log_msg "executing: #{cmd}"
  `#{cmd}`
  if $? != 0
    raise CommandFailed, cmd, caller
  end
end


# todo: use module?
module Builder
  def self.gcc(platform)
    case platform
    when 'darwin-x86_64'
      "#{NDK_ROOT_DIR}/platform/prebuilts/gcc/darwin-x86/host/x86_64-apple-darwin-4.9.1/bin/x86_64-apple-darwin12-gcc"
    else
      raise "unsupported GCC platform: #{platform}"
    end
  end
end

module Cache
  PATH = "/var/tmp/ndk-cache-#{ENV['USER']}"

  def self.exists?(archive)
    File.exists? "#{PATH}/#{archive}"
  end

  def self.unpack(archive, dir)
    run_cmd "7z x -o#{dir} #{PATH}/#{archive} > /dev/null"
  end

  def self.add(file)
    FileUtils.move(file, PATH)
  end
end


PKG_VERSION = '2.2.1'
SRC_DIR = "#{VENDOR_DIR}/ruby"
DST_DIR = "#{NDK_DIR}/tools"
BUILD_BASE = "/tmp/ndk-#{ENV['USER']}/vendor/ruby"
BUILD_DIR = "#{BUILD_BASE}/build"
INSTALL_DIR = "#{BUILD_BASE}/ruby"


archive = "ruby-#{PKG_VERSION}-#{TARGET_PLATFORM}.7z"

begin

  Logger.log_msg "building #{archive}"

  if Cache.exists?(archive)
    Logger.log_msg "found cached file: #{archive}"
    Cache.unpack(archive, DST_DIR)
    Logger.log_msg "done"
    exit 0
  end

  FileUtils.mkdir_p(BUILD_DIR)

  FileUtils.cd(BUILD_DIR) do
    CC = Builder.gcc(TARGET_PLATFORM)
    run_cmd "CC=#{CC} #{SRC_DIR}/configure --prefix=#{INSTALL_DIR}"
    run_cmd "make"
    run_cmd "make install"
  end
  run_cmd "#{INSTALL_DIR}/bin/gem install rspec"

  FileUtils.cd(BUILD_BASE) { run_cmd "7z a #{archive} ruby > /dev/null" }
  Cache.add("#{BUILD_BASE}/#{archive}")

  FileUtils.remove_dir("#{DST_DIR}/ruby", true)
  Cache.unpack(archive, DST_DIR)

rescue Exception => e
  Logger.log_exception(e)
  exit 1
else
  # todo: clean tmp files
end
