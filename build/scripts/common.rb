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


class Common

  NDK_ROOT_DIR = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.dirname.dirname.to_s

  NDK_DIR        = "#{NDK_ROOT_DIR}/platform/ndk"
  VENDOR_DIR     = "#{NDK_ROOT_DIR}/vendor"
  CREW_DIR       = "#{NDK_DIR}/tools/crew"
  BUILD_BASE_DIR = "/tmp/ndk-#{ENV['USER']}/crew"

  require "#{Common::CREW_DIR}/library/formula"
  require "#{Common::CREW_DIR}/library/formulary"

  def self.parse_build_options(pkgname)
    os, cpu = get_host_platform
    # set default values for build options
    options = { host_os:         os,
                host_cpu:        cpu,
                target_os:       os,
                target_cpu:      cpu,
                num_jobs:        '1',
                no_clean:        false,
                no_check:        false,
                force:           false,
                log_rename:      true,
                verbose:         false,
                target_platform: "#{os}-#{cpu}",
                host_platform:   "#{os}-#{cpu}",
                log_file:        "#{BUILD_BASE_DIR}/build-#{pkgname}-#{os}-#{cpu}.log"
              }
    # parse command line args
    ARGV.each do |opt|
      case opt
      when /^--target-os=(\w+)/
       options[:target_os] = $1
      when /^--target-cpu=(\w+)/
        options[:target_cpu] = $1
      when /^--num-jobs=(\d+)/
        options[:num_jobs] = $1
      when '--no-clean'
        options[:no_clean] = true
      when '--no-check'
        options[:no_check] = true
      when '--force'
        @@force = true
      when /^--log-file=(\S+)/
        options[:log_file] = $1
        # explicit log-file options implies log-rename
        options[:log_rename] = false
      when '--verbose'
        options[:verbose] = true
      when '--help'
        show_build_help options
        exit 1
      else
        raise "unknown option: #{opt}"
      end
    end

    # reset options that depend on other values
    options[:target_platform] = "#{options[:target_os]}-#{options[:target_cpu]}"
    options[:host_platform]   = "#{options[:host_os]}-#{options[:host_cpu]}"
    options[:log_file]        = "#{BUILD_BASE_DIR}/build-#{pkgname}-#{options[:target_platform]}.log"

    # enforce no-check for cross builds
    if !options[:no_check]
      options[:no_check] = options[:target_os] != options[:host_os]
    end

    options
  end

  def self.formula_data(name)
    formula = Formulary.factory "#{CREW_DIR}/formula/utilities/#{name}.rb"
    release = formula.releases.last
    ver = release[:version]
    bldnum = release[:build_number]
    [ver, bldnum]
  end

  def self.make_paths(pkgname, ver, bldnum, options)
    pkgver = pkg_version(ver, bldnum)
    osdir = (options[:target_os] == 'windows' and options[:target_cpu] == 'x86') ? 'windows' : options[:target_platform]
    prebuilt = "prebuilt/#{osdir}"

    { src_dir:        "#{VENDOR_DIR}/#{pkgname}",
      build_base_dir: "#{BUILD_BASE_DIR}/#{pkgname}",
      build_dir:      "#{BUILD_BASE_DIR}/#{pkgname}/#{pkgname}",
      prebuilt_dir:   "#{NDK_DIR}/#{prebuilt}",
      install_dir:    "#{BUILD_BASE_DIR}/#{pkgname}/#{prebuilt}/crew/#{pkgname}/#{pkgver}"
    }
  end

  def self.make_archive_base(pkgname, ver, bldnum)
    "crew-#{pkgname}-#{pkg_version(ver, bldnum)}"
  end

  def self.make_archive_name(pkgname, ver, bldnum, platform)
    "#{make_archive_base(pkgname, ver, bldnum)}-#{platform}.7z"
  end

  # def self.ssl_cert_file
  #   case host_os
  #   when 'darwin'
  #     '/usr/local/etc/openssl/osx_cert.pem'
  #   when 'linux'
  #     '/etc/ssl/certs/ca-certificates.crt'
  #   else
  #     raise "unknown host OS: #{host_os}"
  #   end
  # end

  private

  def self.get_host_platform
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

  def self.show_build_help(options)
    puts "Usage: #{$PROGRAM_NAME} [OPTIONS]\n"                                              \
         "where OPTIONS are:\n"                                                             \
         "  --target-os=STR   set target OS; one of linux, darwin, windows;\n"              \
         "                    default #{options[:host_os]}\n"                               \
         "  --target-cpu=STR  set target CPU; one of x86_64, x86;\n"                        \
         "                    default #{options[:host_cpu]}\n"                              \
         "  --num-jobs=N      specifies the number of make's jobs to run simultaneously;\n" \
         "                    default #{options[:num_jobs]}\n"                              \
         "  --no-clean        do not remove temporary files\n"                              \
         "  --no-check        do not run make check or make test\n"                         \
         "  --force           do not check cache, force build\n"                            \
         "  --log-file=NAME   set log filename\n"                                           \
         "                    default #{options[:log_file]}\n"                              \
         "  --verbose         output more info to console\n"                                \
         "  --help            show this message and exit\n"
  end

  def self.pkg_version(ver, bldnum)
    "#{ver}_#{bldnum}"
  end

end
