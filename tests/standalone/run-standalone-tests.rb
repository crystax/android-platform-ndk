#!/usr/bin/env ruby
#
# This script runs all standalone tests for all supported toolchiains,
# platforms and architectures.
#
#
# Copyright (c) 2014, 2015, 2016, 2017 CrystaX.
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
# THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CrystaX OR CONTRIBUTORS BE LIABLE
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
# official policies, either expressed or implied, of CrystaX.
#

require 'optparse'
require 'fileutils'
require 'pathname'


NDK_DIR       = Pathname.new(__FILE__).realpath.dirname.dirname.dirname.to_s
CREW_CMD      = (RbConfig::CONFIG['EXEEXT'] == '.exe') ? "#{NDK_DIR}/crew.cmd" : "#{NDK_DIR}/crew"
CREW_DIR      = `#{CREW_CMD} -W env --base-dir`.strip
PLATFORM_NAME = File.basename(`#{CREW_CMD} -W env --tools-dir`.strip)
TMP_DIR       = `#{CREW_CMD} -W env --base-build-dir`.strip

require_relative "#{CREW_DIR}/library/arch.rb"
require_relative "#{CREW_DIR}/library/toolchain.rb"


class Ndk_data

  attr_reader :default_stl_types, :llvm_versions, :tag, :compiler_types

  def initialize
    user = ENV['USER']

    # ./test/standalone/run.sh will use the value of the variable as log file name
    self.log_file = "#{TMP_DIR}/tests/standalone.log"
    FileUtils.mkdir_p File.dirname(self.log_file)

    @ndk_tmp_dir = "#{TMP_DIR}/tmp"
    @default_stl_types = ["gnustl", "libc++"]
    @llvm_versions = Toolchain::SUPPORTED_LLVM.map(&:version)
    @compiler_types = ["gcc"] + @llvm_versions.map { |v| "clang" + v }
  end

  def log_file
    ENV['NDK_LOGFILE']
  end

  def log_file=(file)
    ENV['NDK_LOGFILE'] = file
  end

  def default_api_levels
    api_levels = []
    FileUtils.cd("#{NDK_DIR}/platforms") { api_levels = Dir['*'].map { |s| s.split('-')[1].to_i } }
    api_levels.sort
  end

  def allowed_api_levels(arch, apis)
    apis.select { |api| api >= arch.min_api_level }
  end

  def standalone_path(api_level, arch, gcc_version, stl_type)
    File.join(@ndk_tmp_dir, "android-ndk-api-#{api_level}-#{arch.name}-#{gcc_version}-#{stl_type}")
  end
end


NDK_DATA = Ndk_data.new
$num_tests_run = 0
$num_tests_failed = 0


class StandaloneToolchain

  attr_reader :arch, :gcc_version, :stl, :api_level, :install_dir_base, :prefix, :results

  def initialize(arch, gcc_version, stl, api_level)
    @arch        = arch
    @gcc_version = gcc_version
    @stl         = stl
    @api_level   = api_level

    @install_dir_base = NDK_DATA.standalone_path(api_level, arch, gcc_version, stl)
    @prefix = arch.host

    @results = Hash.new(0)

    # todo: why?
    log = nil

    NDK_DATA.llvm_versions.each do |llvm_ver|
      args = [" --clean-install-dir",
              " --install-dir=#{toolchain_dir(llvm_ver)}",
              " --gcc-version=#{gcc_version}",
              " --llvm-version=#{llvm_ver}",
              " --stl=#{stl}",
              " --arch=#{arch.name}",
              " --platform=#{PLATFORM_NAME}",
              " --api-level=#{api_level}"
             ]
      cmd = "#{CREW_CMD} -W -b make-standalone-toolchain #{args.join(' ')}"
      File.open(NDK_DATA.log_file, "a") do |log|
        log.puts "############################################"
        log.puts
        log.puts " Creating toolchain with command: #{cmd}"
        log.puts
        log.puts "############################################"
      end
      `#{cmd}`
      if $? != 0
        abort("failed to make standalone toolchain with command: #{cmd}")
      end
    end
  end

  def toolchain_dir(llvm_ver)
    "#{install_dir_base}-#{llvm_ver}"
  end

  def clean
    NDK_DATA.llvm_versions.each { |llvm_ver| FileUtils.rm_rf toolchain_dir(llvm_ver) }
  end

  def run_tests
    # run tests with GCC toolchain
    # since GCC compilers are the same for each LLVM version
    # use first LLVM version to run GCC tests
    # todo: test armeabi-v7a-hard too
    cmd = "./tests/standalone/run.sh --no-sysroot --prefix=#{toolchain_dir(NDK_DATA.llvm_versions[0])}/bin/#{prefix}-gcc --abi=#{arch.abis[0]}"
    results['gcc'] = run_test_cmd(cmd)
    # run tests with LLVM toolchains
    NDK_DATA.llvm_versions.each do |llvm_ver|
      cmd = "./tests/standalone/run.sh --no-sysroot --prefix=#{toolchain_dir(llvm_ver)}/bin/clang --abi=#{arch.abis[0]}"
      results['clang'+llvm_ver] = run_test_cmd(cmd)
    end
    results
  end

  def run_test_cmd(cmd)
    num_failed = 0
    FileUtils.cd(NDK_DIR) do
      output = `#{cmd}`
      r = (output.split("\n")[-1]).split
      if r[-1] == 'Success.'
        $num_tests_run += Integer((r[0].split("/"))[0])
      elsif r[2] == 'failed'
        $num_tests_run += Integer(r[-1].delete("."))
        $num_tests_failed += num_failed = Integer(r[0])
      else
        abort("error: unexpected output from command: #{cmd}; output: #{output}")
      end
    end
    num_failed
  end
end


architectures = Arch::LIST.values
gcc_versions  = Toolchain::SUPPORTED_GCC.map(&:version)
stl_types     = NDK_DATA.default_stl_types
api_levels    = NDK_DATA.default_api_levels

options = { clean: true }
OptionParser.new do |opts|
  opts.banner = "Usage: #{$PROGRAM_NAME} [options]"

  opts.on("--api-levels=LIST", String, "List of API levels;", "#{api_levels}") do |l|
    api_levels = l.split(',').map { |s| s.to_i }
  end

  opts.on("--archs=LIST", String, "List of architectures;", "#{architectures.map(&:name).join(',')}") do |l|
    architectures = l.split(',').map { |a| Arch::LIST[a.to_sym] }
  end

  opts.on("--stl-types=LIST", String, "List of STL variants;", "#{stl_types}") do |l|
    stl_types = l.split(',')
  end

  opts.on("--gcc-versions=LIST", String, "List of GCC version;", "#{gcc_versions}") do |l|
    gcc_versions = l.split(',')
  end

  opts.on("--ndk-log-file=FILE", String, "Use the FILE as NDK's log file;", "#{NDK_DATA.log_file}") do |f|
    NDK_DATA.log_file = File.expand_path(f)
  end

  opts.on("--no-clean", "Do not remove toolchains after tests were run") do |_|
    options[:clean] = false
  end

  opts.on("-h", "--help", "Display this help and exit") do
    puts opts
    # todo: exit with non zero code
    exit
  end
end.parse!

puts "using tools for platform: #{PLATFORM_NAME}"
architectures.each do |arch|
  puts "#{arch}"
  gcc_versions.each do |gccver|
    puts "  #{gccver}"
    stl_types.each do |stl|
      print "    #{stl}, API levels: "
      results = Hash[NDK_DATA.compiler_types.map { |v| [v, []] }]
      NDK_DATA.allowed_api_levels(arch, api_levels).each do |apilev|
        # create toolchain and run tests
        toolchain = StandaloneToolchain.new(arch, gccver, stl, apilev)
        # run_test returns a hash indexed by compiler type
        # where each hash value is a number of failed tests
        r = toolchain.run_tests
        NDK_DATA.compiler_types.each do |compiler|
          if r[compiler] > 0
            results[compiler] = results[compiler] << [Integer(apilev), r[compiler]]
          end
        end
        # remove toolchain if clean was requested
        if options[:clean]
          toolchain.clean
        end
        print "#{apilev} "
      end
      puts ""
      # output results
      NDK_DATA.compiler_types.each do |compiler|
        print "      #{compiler}: "
        if results[compiler].size == 0
          puts "OK"
        else
          puts results[compiler].to_s
        end
      end
      puts ""
    end
  end
end

puts "Summary:"
puts "    number of tests run:    #{$num_tests_run}"
puts "    number of failed tests: #{$num_tests_failed}"

exit $num_tests_failed
