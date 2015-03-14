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

require_relative 'common.rb'


module Builder
  def self.cc(platform)
    case platform
    when 'darwin-x86_64'
      # todo: builds ruby with not working psych library (gem isntall fails)
      #"#{Common::NDK_ROOT_DIR}/platform/prebuilts/gcc/darwin-x86/host/x86_64-apple-darwin-4.9.1/bin/x86_64-apple-darwin12-gcc"
      ''
    when 'linux-x86_64'
      "#{Common::NDK_ROOT_DIR}/platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/bin/x86_64-linux-gcc"
    else
      raise "unsupported CC platform: #{platform}"
    end
  end

  def self.cflags(platform)
    case platform
    when 'darwin-x86_64'
      "--sysroot #{Common::NDK_ROOT_DIR}/platform/prebuilts/sysroot/darwin-x86/MacOSX10.6.sdk " \
      "-mmacosx-version-min=#{Common::MACOSX_VERSION_MIN}"
    when 'linux-x86_64'
      "--sysroot #{Common::NDK_ROOT_DIR}/platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/sysroot "
    else
      raise "unsupported CFLAGS platform: #{platform}"
    end
  end
end
