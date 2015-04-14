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
require_relative 'versions.rb'

module Common

  NDK_ROOT_DIR = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.dirname.dirname.to_s

  NDK_DIR = "#{NDK_ROOT_DIR}/platform/ndk"
  NDK_BUILD_DIR = "/tmp/ndk-#{ENV['USER']}/vendor"
  VENDOR_DIR = "#{NDK_ROOT_DIR}/vendor"

  BUILD_BASE = "#{NDK_BUILD_DIR}/#{Crystax::PKG_NAME}"
  BUILD_DIR = "#{BUILD_BASE}/build"
  SRC_DIR = "#{Common::VENDOR_DIR}/#{Crystax::PKG_NAME}"
  DST_DIR = "#{NDK_DIR}"
  INSTALL_DIR = "#{BUILD_BASE}/tools/#{Crystax::PKG_NAME}"

  MACOSX_VERSION_MIN = '10.6'


  def self.log_file
    if not @@log_file
      @@log_file = "#{NDK_BUILD_DIR}/build-#{Crystax::PKG_NAME}-#{target_platform}.log"
    end
    @@log_file
  end

  def self.target_os
    @@target_os
  end

  def self.target_cpu
    @@target_cpu
  end

  def self.target_platform
    "#{target_os}-#{target_cpu}"
  end

  def self.host_os
    @@host_os
  end

  def self.host_cpu
    @@host_cpu
  end

  def self.host_platform
    "#{host_os}-#{host_cpu}"
  end

  def self.different_os?
    target_os != host_os
  end

  def self.same_platform?
    target_platform == host_platform
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

  def self.force?
    @@force
  end

  def self.verbose?
    @@verbose
  end

  def self.make_archive_base(pkgname = Crystax::PKG_NAME)
    "vendor-#{pkgname}-#{Crystax.version(pkgname)}"
  end

  def self.make_archive_name(pkgname = Crystax::PKG_NAME)
    "#{make_archive_base(pkgname)}-#{target_platform}.7z"
  end

  def self.parse_options
    ARGV.each do |opt|
      case opt
      when /^--target-os=(\w+)/
        @@target_os = $1
      when /^--target-cpu=(\w+)/
        @@target_cpu = $1
      when /^--num-jobs=(\d+)/
        @@num_jobs = $1
      when '--no-clean'
        @@no_clean = true
      when '--no-check'
        @@no_check = true
      when '--force'
        @@force = true
      when /^--log-file=(\S+)/
        @@log_file = $1
        Logger.rename = false
      when '--verbose'
        @@verbose = true
      when '--help'
        show_help
        exit 1
      else
        raise "unknown option: #{opt}"
      end
    end
  end

  private

  def self.set_host_platform
    h = RUBY_PLATFORM.split('-')
    cpu = h[0]
    case h[1]
    when /linux/
      os = 'linux'
    when /darwin/
      os = 'darwin'
    else
      raise "unsupported host OS: #{h[1]}"
    end
    [os, cpu]
  end

  def self.show_help
    puts "Usage: #{$PROGRAM_NAME} [OPTIONS]\n"                                              \
         "where OPTIONS are:\n"                                                             \
         "  --target-os=STR   set target OS; one of linux, darwin, windows;\n"              \
         "                    default #{host_os}\n"                                         \
         "  --target-cpu=STR  set target CPU; one of x86_64, x86;\n"                        \
         "                    default #{host_cpu}\n"                                        \
         "  --num-jobs=N      specifies the number of make's jobs to run simultaneously;\n" \
         "                    default #{num_jobs}\n"                                        \
         "  --no-clean        do not remove temporary files\n"                              \
         "  --no-check        do not run make check or make test\n"                         \
         "  --force           do not check cache, force build\n"                            \
         "  --log-file=NAME   set log filename\n"                                           \
         "                    default #{log_file}\n"                                        \
         "  --verbose         output more info to console\n"                                \
         "  --help            show this message and exit\n"
  end

  @@host_os, @@host_cpu = set_host_platform
  @@target_os = @@host_os
  @@target_cpu = @@host_cpu
  # todo: calculates as NUM_CPU * 2
  @@num_jobs = 16
  @@no_clean = false
  @@no_check = false
  @@force = false
  @@log_file = nil
  @@verbose = false
end
