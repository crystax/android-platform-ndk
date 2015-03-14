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

require 'pathname'

module Common

  NDK_ROOT_DIR = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.dirname.dirname.to_s

  NDK_DIR = "#{NDK_ROOT_DIR}/platform/ndk"
  NDK_BUILD_DIR = "/tmp/ndk-#{ENV['USER']}/vendor"
  VENDOR_DIR = "#{NDK_ROOT_DIR}/vendor"

  BUILD_BASE = "#{NDK_BUILD_DIR}/#{Crystax::PKG_NAME}"
  BUILD_DIR = "#{BUILD_BASE}/build"
  SRC_DIR = "#{Common::VENDOR_DIR}/#{Crystax::PKG_NAME}"
  DST_DIR = "#{NDK_DIR}/tools"
  INSTALL_DIR = "#{BUILD_BASE}/#{Crystax::PKG_NAME}"

  LOG_FILE = "#{NDK_BUILD_DIR}/build-#{Crystax::PKG_NAME}.log"

  MACOSX_VERSION_MIN = '10.6'

  def self.target_os
    raise "target OS was never set" unless @@target_os
    @@target_os
  end

  def self.target_cpu
    raise "target CPU was never set" unless @@target_cpu
    @@target_cpu
  end

  def self.target_platform
    "#{target_os}-#{target_cpu}"
  end

  def self.num_jobs
    @@num_jobs
  end

  def self.no_clean?
    @@no_clean
  end

  def self.no_check?
    @@no_check
  end

  def self.make_archive_name
    "crystax-#{Crystax::PKG_NAME}-#{Crystax::PKG_VERSION}-#{target_platform}.7z"
  end

  def self.parse_options
    ARGV.each do |opt|
      case opt
      when /^--target-os=(\w+)/
        @@target_os = $1
      when /^--target-cpu=(\w+)/
        @@target_cpu = $1
      when /^--num-jobs=(\w+)/
        @@num_jobs = $1
      when '--no-clean'
        @@no_clean = true
      when '--no-check'
        @@no_check = true
      else
        raise "unknown option: #{opt}"
      end
    end
  end

  private

  @@target_os = nil
  @@target_cpu = nil
  # todo: calculates as NUM_CPU * 2
  @@num_jobs = 16
  @@no_clean = false
  @@no_check = false
end
