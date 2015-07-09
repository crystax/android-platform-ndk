#
# List of vendor's utils and their versions
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

module Crystax

  BUILD_UTILS = ['zlib', 'openssl', 'libssh2', 'libgit2', 'ruby', 'curl', 'p7zip']
  INSTALL_UTILS = BUILD_UTILS.slice(3, BUILD_UTILS.size)

  VERSIONS = {
    'zlib'    => '1.2.8',
    'libffi'  => '3.2.1',
    'openssl' => '1.0.2a',
    'libssh2' => '1.5.0',
    'libgit2' => '0.22.2',
    'ruby'    => '2.2.2',
    'curl'    => '7.42.0',
    'p7zip'   => '9.20.1'
  }

  def self.version(name)
    ver = VERSIONS[name]
    raise "no version for #{name}" unless ver
    ver
  end

  def self.package_version
    version(PKG_NAME)
  end

  def self.check_version(ver)
    if ver != package_version
      raise "bad #{PKG_NAME} version: repository: #{ver}; expected: #{package_version}"
    end
  end
end
