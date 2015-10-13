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
require 'fileutils'
require_relative 'options.rb'


class Common

  CREW_UTILS = ['curl', 'p7zip', 'ruby']

  NDK_ROOT_DIR = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.dirname.dirname.to_s

  NDK_DIR        = "#{NDK_ROOT_DIR}/platform/ndk"
  VENDOR_DIR     = "#{NDK_ROOT_DIR}/vendor"
  CREW_DIR       = "#{NDK_DIR}/tools/crew"
  BUILD_BASE_DIR = "/tmp/ndk-#{ENV['USER']}/crew"

  FileUtils.mkdir_p "#{NDK_DIR}/prebuilt/#{Options.host_platform}/crew"

  require "#{Common::CREW_DIR}/library/formula.rb"
  require "#{Common::CREW_DIR}/library/formulary.rb"
  require "#{Common::CREW_DIR}/library/utility.rb"

  def self.parse_build_options(pkgname)
    # set default values for build options
    options = Options.new

    # parse command line args
    ARGV.each do |opt|
      case opt
      when /^--target-os=(\w+)/
       options.target_os = $1
      when /^--target-cpu=(\w+)/
        options.target_cpu = $1
      when /^--num-jobs=(\d+)/
        options.num_jobs = $1
      when '--no-clean'
        options.no_clean = true
      when '--no-check'
        options.no_check = true
      when '--force'
        options.force = true
      when '--dont-update-sha256-sums'
        options.update_sha256_sums = false
      when /^--log-file=(\S+)/
        options.log_file = $1
        # explicit log-file options implies log-rename disabling
        options.rename_log = false
      when '--verbose'
        options.verbose = true
      when '--help'
        show_build_help options, pkgname
        exit 1
      else
        raise "unknown option: #{opt}"
      end
    end

    options.log_file = default_build_logfile_name(options, pkgname) unless options.log_file
    # enforce no-check for cross builds
    options.no_check = options.target_os != options.host_os unless options.no_check?

    options
  end

  def self.parse_install_options
    # set default values for build options
    options = Options.new

    # parse command line args
    ARGV.each do |opt|
      case opt
      when /^--target-os=(\w+)/
       options.target_os = $1
      when /^--target-cpu=(\w+)/
        options.target_cpu = $1
      when /^--log-file=(\S+)/
        options.log_file = $1
        # explicit log-file options implies log-rename disabling
        options.rename_log = false
      when /^--out-dir=(\S+)/
       options.out_dir = $1
      when '--verbose'
        options.verbose = true
      when '--help'
        show_install_help options
        exit 1
      else
        raise "unknown option: #{opt}"
      end
    end

    raise "out-dir must be specified" unless options.out_dir

    options.log_file = default_install_logfile_name(options) unless options.log_file

    options
  end

  def self.make_build_data(pkgname, options)
    release, formula = formula_data(pkgname)
    paths = make_paths(pkgname, release, options)
    FileUtils.rm_rf paths[:build_base_dir]
    archive = make_archive_name(pkgname, release, options.target_platform)
    [release, paths, archive, formula]
  end

  def self.make_archive_base(pkgname, release)
    "crew-#{pkgname}-#{Formula.package_version(release)}"
  end

  def self.make_archive_name(pkgname, release, platform)
    "#{make_archive_base(pkgname, release)}-#{platform}.7z"
  end

  def self.host_ssl_cert_file(os)
    case os
    when 'darwin'
      '/usr/local/etc/openssl/osx_cert.pem'
    when 'linux'
      '/etc/ssl/certs/ca-certificates.crt'
    else
      raise "unknown host OS: #{host_os}"
    end
  end

  def self.write_properties(dir, release)
    props = {crystax_version: release.crystax_version}
    File.open(File.join(dir, Formula::PROPERTIES_FILE), 'w') { |f| f.puts props.to_json }
  end

  def self.write_active_file(out_dir, platform, uname, release)
    path = File.join(out_dir, 'prebuilt', platform, 'crew', uname, Global::ACTIVE_UTIL_FILE)
    File.open(path, 'w') { |f| f.puts release.to_s }
  end

  def self.update_release_shasum(formula_file, release, platform)
    ver = release.version
    cxver = release.crystax_version
    sum = release.shasum(platform)
    release_regexp = /^[[:space:]]*release[[:space:]]+version:[[:space:]]+'#{ver}',[[:space:]]+crystax_version:[[:space:]]+#{cxver}/
    platform_regexp = /#{platform}:/
    lines = []
    state = :copy
    File.foreach(formula_file) do |l|
      case state
      when :updated
        lines << l
      when :copy
        if  l !~ release_regexp
          lines << l
        else
          if l !~ platform_regexp
            state = :updating
            lines << l
          else
            state = :updated
            lines << l.gsub(/'[[:xdigit:]]+'/, "'#{sum}'")
          end
        end
      when :updating
        if l !~ platform_regexp
          lines << l
        else
          state = :updated
          lines << l.gsub(/'[[:xdigit:]]+'/, "'#{sum}'")
        end
      else
        raise "in formula #{File.basename(formula_file)} bad state #{state} on line: #{l}"
      end
    end

    File.open(formula_file, 'w') { |f| f.puts lines }
  end

  private

  def self.show_build_help(options, pkgname)
    puts "Usage: #{$PROGRAM_NAME} [OPTIONS]\n"                                              \
         "where OPTIONS are:\n"                                                             \
         "  --target-os=STR   set target OS; one of linux, darwin, windows;\n"              \
         "                    default #{options.host_os}\n"                                 \
         "  --target-cpu=STR  set target CPU; one of x86_64, x86;\n"                        \
         "                    default #{options.host_cpu}\n"                                \
         "  --num-jobs=N      specifies the number of make's jobs to run simultaneously;\n" \
         "                    default #{options.num_jobs}\n"                                \
         "  --no-clean        do not remove temporary files\n"                              \
         "  --no-check        do not run make check or make test\n"                         \
         "  --force           do not check cache, force build\n"                            \
         "  --dont-update-sha256-sums\n"                                                    \
         "                    do not update sha256 sum in the utility's formula\n"          \
         "  --log-file=NAME   set log filename\n"                                           \
         "                    default #{default_build_logfile_name(options, pkgname)}\n"    \
         "  --verbose         output more info to console\n"                                \
         "  --help            show this message and exit\n"
  end

  def self.show_install_help(options)
    puts "Usage: #{$PROGRAM_NAME} [OPTIONS]\n"                                             \
         "where OPTIONS are:\n"                                                            \
         "  --target-os=STR      set target OS; one of linux, darwin, windows;\n"          \
         "                       default #{options.host_os}\n"                             \
         "  --target-cpu=STR     set target CPU; one of x86_64, x86;\n"                    \
         "                       default #{options.host_cpu}\n"                            \
         "  --log-file=NAME      set log filename\n"                                       \
         "                       default #{default_install_logfile_name(options)}\n"       \
         "  --out-dir=NAME       set output directory; crew utilities will be installed\n" \
         "                       into that directory; required\n"                          \
         "  --verbose            output more info to console\n"                            \
         "  --help               show this message and exit\n"
  end

  def self.formula_data(name)
    # formula = Formulary.factory "#{CREW_DIR}/formula/utilities/#{name}.rb"
    path = "#{CREW_DIR}/formula/utilities/#{name}.rb"
    formula = Formulary.klass(path).new(path, :no_active_file)
    release = formula.releases.last
    [release, formula]
  end

  def self.make_paths(pkgname, release, options)
    pkgver = Formula.package_version(release)
    prebuilt = "prebuilt/#{options.target_platform}"

    { src_dir:        "#{VENDOR_DIR}/#{pkgname}",
      build_base_dir: "#{BUILD_BASE_DIR}/#{pkgname}",
      build_dir:      "#{BUILD_BASE_DIR}/#{pkgname}/#{pkgname}",
      prebuilt_dir:   "#{NDK_DIR}/#{prebuilt}",
      install_dir:    "#{BUILD_BASE_DIR}/#{pkgname}/#{prebuilt}/crew/#{pkgname}/#{pkgver}"
    }
  end

  def self.default_build_logfile_name(options, pkgname)
    "#{BUILD_BASE_DIR}/build-#{pkgname}-#{options.target_platform}.log"
  end

  def self.default_install_logfile_name(options)
    "#{BUILD_BASE_DIR}/install-crew-utils-#{options.target_platform}.log"
  end
end
