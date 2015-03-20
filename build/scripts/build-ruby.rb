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

require_relative 'logger.rb'
require_relative 'commander.rb'
require_relative 'builder.rb'
require_relative 'cache.rb'


def prepare_openssl
    openssldir = "#{Common::BUILD_BASE}/openssl"
    FileUtils.mkdir_p(openssldir)
    arch = Common::make_archive_name('openssl', '1.0.2')
    Cache.unpack(arch, 'openssl', Common::BUILD_BASE)
    openssldir
end


def build_libffi(installdir)
  Logger.msg "building libffi"
  srcdir = "#{Common::VENDOR_DIR}/libffi"
  FileUtils.cd(srcdir) { Commander::run "./autogen.sh" } unless File.exists?("#{srcdir}/configure")
  builddir = "#{Common::BUILD_BASE}/libffi"
  FileUtils.mkdir_p(builddir)
  FileUtils.cd(builddir) do
    env = { 'CC' => Builder.cc,
            'CFLAGS' => Builder.cflags
          }
    # todo: use Builder.host
    args = ["--prefix=#{installdir}",
            "--host=#{Builder.configure_host}"
           ]
    Commander::run env, "#{srcdir}/configure #{args.join(' ')}"
    Commander::run "make -j #{Common::num_jobs}"
    # here make check requires DejaGNU installed
    #Commander::run env, "make check" unless Common::no_check?
    Commander::run "make install"
  end
  # todo: libffi version
  FileUtils.cp_r "#{installdir}/lib/libffi-3.2.1/include", "#{installdir}/"
  FileUtils.rm ["#{installdir}/lib/libffi.dll.a", "#{installdir}/lib/libffi.la"]
end


def build_zlib(installdir)
  Logger.msg "building zlib"
  FileUtils.cp_r "#{Common::VENDOR_DIR}/zlib", Common::BUILD_BASE
  FileUtils.cd("#{Common::BUILD_BASE}/zlib") do
    fname = 'win32/Makefile.gcc'
    text = File.read(fname).gsub(/^PREFIX/, '#PREFIX')
    File.open(fname, "w") {|f| f.puts text }
    # chop 'gcc' from the end of the string
    env = { 'PREFIX' => Builder.cc.chop.chop.chop }
    Commander::run env, "make -j #{Common::num_jobs} -f win32/Makefile.gcc"
    FileUtils.cp 'libz.a', "#{installdir}/lib/"
    FileUtils.cp ['zlib.h', 'zconf.h'], "#{installdir}/include"
  end
end


def install_gems(*gems)
  args = ['--no-document']
  env = {}

  if Common::target_os != 'windows'
    gem = "#{Common::INSTALL_DIR}/bin/gem"
  else
    gem = "/usr/bin/gem"
    env['GEM_HOME'] = "#{Common::INSTALL_DIR}/lib/ruby/gems/2.2.0"
  end

  Commander::run env, "#{gem} install #{gems.join(' ')} #{args.join(' ')}"
end


begin
  Common.parse_options

  Logger.open_log_file Common.log_file
  archive = Common.make_archive_name

  if Cache.try?(archive)
    Logger.msg "done"
    exit 0
  end

  if Common.target_os == 'windows'
    libsdir = "#{Common::BUILD_BASE}/libs"
    FileUtils.mkdir_p([Common::BUILD_BASE, "#{libsdir}/lib", "#{libsdir}/include"])
    build_libffi(libsdir)
    build_zlib(libsdir)
  end

  Logger.msg "building #{archive}"
  FileUtils.cd(Common::SRC_DIR) { Commander.run "autoconf" } unless File.exists?("#{Common::SRC_DIR}/configure")
  openssldir = prepare_openssl
  FileUtils.mkdir_p(Common::BUILD_DIR)
  FileUtils.cd(Common::BUILD_DIR) do
    env = { 'CC' => Builder.cc,
            'CFLAGS' => Builder.cflags,
            'DESTDIR' => Common::BUILD_BASE
          }
    args = ["--prefix=/ruby",
            "--host=#{Builder.configure_host}",
            "--disable-install-doc",
            "--enable-load-relative",
            "--with-openssl-dir=#{openssldir}",
            "--with-static-linked-ext"
           ]
    if Common::target_os == 'windows'
      args << '--host=x86_64-mingw64'
      env['PATH'] = Builder.toolchain_path_and_path
      env['CFLAGS'] += " -I#{libsdir}/include"
      env['LDFLAGS'] = "-L#{libsdir}/lib"
    end
    Commander::run env, "#{Common::SRC_DIR}/configure #{args.join(' ')}"
    Commander::run env, "make -j #{Common::num_jobs} V=1"
    #Commander::run env, "make check" unless Common::no_check?
    Commander::run env, "make install"
  end

  install_gems 'rspec', 'minitest'

  Cache.add(archive)
  Cache.unpack(archive) if Common::host_os == Common::target_os

rescue SystemExit => e
  exit e.status
rescue Exception => e
  Logger.log_exception(e)
  exit 1
else
  FileUtils.remove_dir(Common::BUILD_BASE, true) unless Common::no_clean?
ensure
  Logger.close_log_file
end
