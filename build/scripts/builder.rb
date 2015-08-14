#
# Common builder funtions
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

require_relative 'exceptions.rb'
require_relative 'common.rb'
require_relative 'cache.rb'
require_relative 'commander.rb'
require_relative 'logger.rb'


class Builder

  MACOSX_VERSION_MIN = '10.6'

  def self.cc(os)
    case os
    when 'darwin'
      # todo: builds ruby with not working psych library (gem isntall fails)
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/clang/darwin-x86/host/x86_64-apple-darwin-3.7.0/bin/clang")
    when 'linux'
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/bin/x86_64-linux-gcc")
    when 'windows'
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/gcc/linux-x86/host/x86_64-w64-mingw32-4.8/bin/x86_64-w64-mingw32-gcc")
    else
      raise UnknownTargetOS, os, caller
    end
  end

  def self.cxx(os)
    case os
    when 'darwin'
      # todo: builds ruby with not working psych library (gem isntall fails)
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/clang/darwin-x86/host/x86_64-apple-darwin-3.7.0/bin/clang++")
    when 'linux'
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/bin/x86_64-linux-g++")
    when 'windows'
      File.join(Common::NDK_ROOT_DIR, "platform/prebuilts/gcc/linux-x86/host/x86_64-w64-mingw32-4.8/bin/x86_64-w64-mingw32-g++")
    else
      raise UnknownTargetOS, Common.target_os, caller
    end
  end

  def self.cflags(platform)
    case platform
    when 'darwin-x86_64'
      "-isysroot#{Common::NDK_ROOT_DIR}/platform/prebuilts/sysroot/darwin-x86/MacOSX10.6.sdk " \
      "-mmacosx-version-min=#{MACOSX_VERSION_MIN} " \
      "-m64"
    when 'darwin-x86'
      "-isysroot#{Common::NDK_ROOT_DIR}/platform/prebuilts/sysroot/darwin-x86/MacOSX10.6.sdk " \
      "-mmacosx-version-min=#{MACOSX_VERSION_MIN} " \
      "-m32"
    when 'linux-x86_64'
      "--sysroot=#{Common::NDK_ROOT_DIR}/" \
      "platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/sysroot "
    when 'linux-x86'
      "--sysroot=#{Common::NDK_ROOT_DIR}/" \
      "platform/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.11-4.8/sysroot " \
      "-m32"
    when 'windows-x86_64'
      '-m64'
    when 'windows-x86'
      '-m32'
    else
      raise UnknownTargetPlatform, platform, caller
    end
  end

  def self.toolchain_path_and_path
    case Common.target_platform
    when 'windows-x86_64'
      "#{Common::NDK_ROOT_DIR}/" \
      "platform/prebuilts/gcc/linux-x86/host/x86_64-w64-mingw32-4.8/x86_64-w64-mingw32/bin" \
      ":#{ENV['PATH']}"
    when 'windows-x86'
      "#{Common::NDK_ROOT_DIR}/" \
      "platform/prebuilts/gcc/linux-x86/host/x86_64-w64-mingw32-4.8/x86_64-w64-mingw32/bin" \
      ":#{ENV['PATH']}"
    else
      raise UnknownTargetPlatform, Common.target_platform, caller
    end
  end

  def self.configure_host(platform)
    case platform
    when 'darwin-x86_64'
      'x86_64-darwin10'
    when 'darwin-x86'
      'i686-darwin10'
    when 'linux-x86_64'
      'x86_64-linux'
    when 'linux-x86'
      'i686-linux'
    when 'windows-x86_64'
      'x86_64-mingw64'
    when 'windows-x86'
      'mingw32'
    else
      raise UnknownTargetPlatform, target_platform, caller
    end
  end

  def self.prepare_dependency(name)
    Logger.log_msg "= preparing #{name}"
    unpackdir = "#{Common::NDK_BUILD_DIR}/#{name}"
    FileUtils.mkdir_p(unpackdir)
    arch = Common.make_archive_name(name)
    Cache.unpack(arch, unpackdir)
    @@dependencies << name
    "#{unpackdir}/#{Common.prebuilt_dir}"
  end

  def self.copy_sources(srcdir, dstdir)
    pkgname = File.basename(srcdir)
    FileUtils.rm_rf File.join(dstdir, pkgname)
    FileUtils.mkdir dstdir unless Dir.exists?(dstdir)
    FileUtils.cp_r srcdir, dstdir
  end

  def self.build_zlib(options, paths)
    # todo: check for cached package
    Logger.log_msg "= building zlib for #{options.target_platform}"
    prepare_sources 'zlib', paths[:build_base_dir]
    build_dir = File.join(paths[:build_base_dir], 'zlib.build')
    install_dir = File.join(paths[:build_base_dir], 'zlib')
    FileUtils.cd(build_dir) do
      if options.target_os == 'windows'
        fname = 'win32/Makefile.gcc'
        text = File.read(fname).gsub(/^PREFIX/, '#PREFIX')
        File.open(fname, "w") {|f| f.puts text }
        # chop 'gcc' from the end of the string
        env = { 'PREFIX' => Builder.cc(options.target_os).chop.chop.chop }
        loc = options.target_cpu == 'x86' ? 'LOC=-m32' : 'LOC=-m64'
        Commander::run env, "make -j #{options.num_jobs} #{loc} -f win32/Makefile.gcc libz.a"
        FileUtils.mkdir_p ["#{install_dir}/lib", "#{install_dir}/include"]
        FileUtils.cp 'libz.a', "#{install_dir}/lib/"
        FileUtils.cp ['zlib.h', 'zconf.h'], "#{install_dir}/include/"
      else
        env = { 'CC' => Builder.cc(options.target_os),
                'CFLAGS' => Builder.cflags(options.target_platform)
              }
        args = ["--prefix=#{install_dir}",
                "--static"
               ]
        Commander::run env, "./configure #{args.join(' ')}"
        Commander::run env, "make -j #{options.num_jobs}"
        Commander::run env, "make check" unless options.no_check?
        Commander::run env, "make install"
        FileUtils.rm_rf ["#{install_dir}/share", "#{install_dir}/lib/pkgconfig"]
      end
    end
    FileUtils.rm_rf build_dir unless options.no_clean?

    # todo: cache package
    install_dir
  end

  def self.build_openssl(options, paths)
    # todo:
    # if Cache.try?(archive, :nounpack)
    #   Logger.msg "done"
    #   exit 0
    # end
    Logger.log_msg "= building openssl for #{options.target_platform}"
    Builder.copy_sources File.join(Common::VENDOR_DIR, 'openssl'), paths[:build_base_dir]
    FileUtils.cd(paths[:build_base_dir]) do
      FileUtils.rm_rf 'openssl.build'
      FileUtils.mv 'openssl', 'openssl.build'
    end
    build_dir   = File.join(paths[:build_base_dir], 'openssl.build')
    install_dir = File.join(paths[:build_base_dir], 'openssl')
    FileUtils.cd(build_dir) do
      zlib_dir = paths[:zlib_dir]
      env = { 'CC' => Builder.cc(options.target_os) }
      args = ["--prefix=#{install_dir}",
              "no-idea",
              "no-mdc2",
              "no-rc5",
              "no-shared",
              "zlib",
              openssl_platform(options.target_platform),
              Builder.cflags(options.target_platform),
              "-I#{zlib_dir}/include",
              "-L#{zlib_dir}/lib",
              "-lz"
             ]
      Commander::run env, "./Configure #{args.join(' ')}"
      Commander::run "make depend"
      Commander::run "make" # -j N breaks build on OS X
      Commander::run "make test" unless options.no_check?
      Commander::run "make install"
    end

    # todo:
    #Cache.add(archive)

    install_dir
  end

  def self.build_libssh2(options, paths)
    # todo:
    # if Cache.try?(archive)
    #   Logger.msg "done"
    #   exit 0
    # end
    Logger.log_msg "= building libssh2 for #{options.target_platform}"
    build_base_dir = paths[:build_base_dir]
    prepare_sources 'libssh2', build_base_dir
    #
    build_dir = File.join(build_base_dir, 'libssh2.build')
    install_dir = File.join(build_base_dir, 'libssh2')
    #
    FileUtils.cd(build_dir) do
      Commander.run "./buildconf"
      zlib_dir    = paths[:zlib_dir]
      openssl_dir = paths[:openssl_dir]
      env = { 'CC'      => Builder.cc(options.target_os),
              'CFLAGS'  => "#{Builder.cflags(options.target_platform)} -I#{openssl_dir}/include -I#{zlib_dir}/include",
              'LDFLAGS' => "-L#{openssl_dir}/lib -L#{zlib_dir}/lib -lz",
              'DESTDIR' => install_dir,
              'PATH'    => '/bin:/usr/bin:/sbin:/usr/sbin'
            }
      case options.target_os
      when 'windows'
        env['LIBS'] = "-lgdi32"
      when 'linux'
        env['LIBS'] = "-ldl"
      end
      args = ["--prefix=/",
              "--host=#{Builder.configure_host(options.target_platform)}",
              "--disable-shared",
              "--disable-examples-build",
              "--with-libssl-prefix=#{openssl_dir}",
              "--with-libz=#{zlib_dir}"
             ]
      Commander::run env, "./configure #{args.join(' ')}"
      Commander::run env, "make -j #{options.num_jobs}"
      Commander::run env, "make check" unless options.no_check?
      Commander::run env, "make install"
    end
    # todo:
    # Cache.add(archive)

    install_dir
  end

  private

  def self.prepare_sources(pkgname, build_base_dir)
    Builder.copy_sources File.join(Common::VENDOR_DIR, pkgname), build_base_dir
    build_dir = "#{pkgname}.build"
    FileUtils.cd(build_base_dir) do
      FileUtils.rm_rf build_dir
      FileUtils.mv pkgname, build_dir
    end
  end

  def self.openssl_platform(platform)
    case platform
    when 'darwin-x86_64'  then 'darwin64-x86_64-cc'
    when 'darwin-x86'     then 'darwin-i386-cc'
    when 'linux-x86_64'   then 'linux-x86_64'
    when 'linux-x86'      then 'linux-generic32'
    when 'windows-x86_64' then 'mingw64'
    when 'windows-x86'    then 'mingw'
    else
      raise UnknownTargetPlatform, platform, caller
    end
  end
end
