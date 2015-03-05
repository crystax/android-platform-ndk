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
  VENDOR_DIR = "#{NDK_ROOT_DIR}/vendor"
  SRC_DIR = "#{Common::VENDOR_DIR}/#{Crystax::PKG_NAME}"
  DST_DIR = "#{NDK_DIR}/tools"

  # todo: calculate target platform
  TARGET_PLATFORM = "darwin-x86_64"

  BUILD_BASE = "/tmp/ndk-#{ENV['USER']}/vendor/#{Crystax::PKG_NAME}"
  BUILD_DIR = "#{BUILD_BASE}/build"
  INSTALL_DIR = "#{BUILD_BASE}/#{Crystax::PKG_NAME}"

  def self.make_archive_name
    "#{Crystax::PKG_NAME}-#{Crystax::PKG_VERSION}-#{Common::TARGET_PLATFORM}.7z"
  end
end
