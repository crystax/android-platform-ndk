#!/usr/bin/env ruby
#
# This script runs all standalone tests for all supported toolchiains,
# platforms and architectures.
#
#
# Copyright (c) 2014 CrystaX .NET.
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

require 'optparse'
require 'fileutils'


class Ndk_data

  attr_reader :default_stl_types, :llvm_versions, :tag, :compiler_types

  def initialize
    user = ENV['USER']
    ndk_path = '.'

    # ./test/standalone/run.sh will use the value of the variable as log file name
    self.log_file = "/tmp/ndk-#{user}/tests/standalone.log"

    @ndk_tmp_dir = "/tmp/ndk-#{user}/tmp"
    @ndk_build_tools_path = File.join(ndk_path, 'build', 'tools')

    @cmd_prefix =
      "export NDK_BUILDTOOLS_PATH=#{@ndk_build_tools_path}"         + " ; " +
      ". " + File.join(@ndk_build_tools_path, 'ndk-common.sh')      + " ; " +
      ". " + File.join(@ndk_build_tools_path, 'prebuilt-common.sh') + " ; "

    @default_stl_types = ["gnustl", "libc++", "stlport"]

    @llvm_versions = default_llvm_versions
    @compiler_types = ["gcc"] + @llvm_versions.map { |v| "clang" + v }

    # todo: why 32 windows returns host_tag 'windows' but 64 -- cygwin-x86_64?
    @tag = (host_tag == 'cygwin-x86_64') ? 'windows-x86_64' : host_tag
  end

  def log_file
    ENV['NDK_LOGFILE']
  end

  def log_file=(file)
    ENV['NDK_LOGFILE'] = file
  end

  def default_api_levels
    get_info_from_shell("echo $API_LEVELS").split
  end

  def prebuilt_abis
    get_info_from_shell("echo $PREBUILT_ABIS").split
  end

  def default_gcc_versions
    get_info_from_shell("echo $DEFAULT_GCC_VERSION_LIST").split
  end

  def use_32bit_tools
    @tag = host_tag32
  end

  def allowed_api_levels(abi, apis)
    r = []
    apis.each { |api| if Integer(api) >= min_api_level(abi) then r << api end }
    r
  end

  def allowed_gcc_versions(abi, gcc_versions)
    case abi
    when /64/
      gcc_versions.include?("4.9") ? ["4.9"] : []
    else
      gcc_versions
    end
  end

  def standalone_path(api_level, arch, gcc_version, stl_type)
    File.join(@ndk_tmp_dir, "android-ndk-api#{api_level}-#{arch}-#{tag}-#{gcc_version}-#{stl_type}")
  end

  def toolchain_name_for_arch(arch, gcc_version)
    get_info_from_shell("echo $(get_toolchain_name_for_arch #{arch} #{gcc_version})")
  end

  def toolchain_prefix_for_arch(arch)
    get_info_from_shell("echo $(get_default_toolchain_prefix_for_arch #{arch})")
  end

  def abi_to_arch(abi)
    case abi
    when /arm64/ then "arm64"
    when /arm/   then "arm"
    else         abi
    end
  end

  private

  def min_api_level(abi)
    case abi
    when /64/   then 21
    when /x86/  then 10
    when /mips/ then 15
    else             3
    end
  end

  def default_llvm_versions
    get_info_from_shell("echo $DEFAULT_LLVM_VERSION_LIST").split
  end

  def host_tag
    get_info_from_shell("echo $HOST_TAG")
  end

  def host_tag32
    get_info_from_shell("echo $HOST_TAG32")
  end

  def get_info_from_shell(cmd)
    command = @cmd_prefix + cmd
    s = `#{command}`
    if $? != 0
      abort("failed command: #{command}")
    end
    return s.strip
  end
end


$ndk_data = Ndk_data.new
$num_tests_run = 0
$num_tests_failed = 0


class Toolchain

  def initialize(abi, gccver, stl, apilev)
    @abi = abi
    @arch = $ndk_data.abi_to_arch(abi)
    @gccver = gccver
    @stl = stl
    @apilev = apilev

    @install_dir_base = $ndk_data.standalone_path(@apilev, @arch, @gccver, @stl)
    @name = $ndk_data.toolchain_name_for_arch(@arch, @gccver)
    @prefix = $ndk_data.toolchain_prefix_for_arch(@arch)

    @results = Hash.new(0)

    $ndk_data.llvm_versions.each do |llvm_ver|
      cmd = "./build/tools/make-standalone-toolchain.sh"      +
            " --platform=android-#{apilev}"                   +
            " --install-dir=#{@install_dir_base}-#{llvm_ver}" +
            " --llvm-version=#{llvm_ver}"                     +
            " --stl=#{stl}"                                   +
            " --toolchain=#{@name}"                           +
            " --system=#{$ndk_data.tag}"
      `#{cmd}`
      if $? != 0
        abort("failed to make standalone toolchain with command: #{cmd}")
      end
    end
  end

  def clean
    $ndk_data.llvm_versions.each do |llvm_ver|
      FileUtils.remove_dir("#{@install_dir_base}-#{llvm_ver}")
    end
  end

  def run_tests
    # run tests with GCC toolchain
    # since GCC compilers are the same for each LLVM version
    # use first LLVM version to run GCC tests
    @results['gcc'] =
      if (@gccver == "4.6") && (@stl == 'libc++')
        -1
      else
        cmd = "./tests/standalone/run.sh " +
              " --no-sysroot"              +
              " --prefix=#{@install_dir_base}-#{$ndk_data.llvm_versions[0]}/bin/#{@prefix}-gcc"
        if /armeabi/ =~ @abi
          cmd += " --abi=#{@abi}"
        end
        run_test_cmd(cmd)
      end
    # run tests with LLVM toolchains
    # but do not run tests with LLVM for gcc 4.6 based toolchains
    $ndk_data.llvm_versions.each do |llvm_ver|
      @results['clang'+llvm_ver] =
        if @gccver == "4.6"
          -1
        else
          cmd = "./tests/standalone/run.sh " +
                " --no-sysroot"              +
                " --prefix=#{@install_dir_base}-#{llvm_ver}/bin/clang"
          if /armeabi/ =~ @abi
            cmd += " --abi=#{@abi}"
          end
          run_test_cmd(cmd)
        end
    end
    @results
  end

  private

  def run_test_cmd(cmd)
    output = `#{cmd}`
    num_failed = 0
    r = (output.split("\n")[-1]).split
    if r[-1] == 'Success.'
      $num_tests_run += Integer((r[0].split("/"))[0])
    elsif r[2] == 'failed'
      $num_tests_run += Integer(r[-1].delete("."))
      $num_tests_failed += num_failed = Integer(r[0])
    else
      abort("error: unexpected output from command: #{cmd}; output: #{output}")
    end
    num_failed
  end
end


api_levels = $ndk_data.default_api_levels
abis = $ndk_data.prebuilt_abis
stl_types = $ndk_data.default_stl_types
gcc_versions = $ndk_data.default_gcc_versions

options = { clean: true }
OptionParser.new do |opts|
  opts.banner = "Usage: #{$PROGRAM_NAME} [options]"

  opts.on("--api-levels=LIST", String, "List of API levels;", "#{api_levels}") do |l|
    api_levels = l.split(',').map {|s| Integer(s) }
  end

  opts.on("--abis=LIST", String, "List of ABIs;", "#{abis}") do |l|
    abis = l.split(',')
  end

  opts.on("--stl-types=LIST", String, "List of STL variants;", "#{stl_types}") do |l|
    stl_types = l.split(',')
  end

  opts.on("--gcc-versions=LIST", String, "List of GCC version;", "#{gcc_versions}") do |l|
    gcc_versions = l.split(',')
  end

  opts.on("--use-32bit-tools", "Use 32bit versions of tools on 64bit host") do |_|
    $ndk_data.use_32bit_tools
  end

  opts.on("--ndk-log-file=FILE", String, "Use the FILE as NDK's log file;", "#{$ndk_data.log_file}") do |f|
    $ndk_data.log_file = File.expand_path(f)
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

puts "using tools for tag: #{$ndk_data.tag}"
abis.each do |abi|
  puts "#{abi}"
  $ndk_data.allowed_gcc_versions(abi, gcc_versions).each do |gccver|
    puts "  #{gccver}"
    stl_types.each do |stl|
      print "    #{stl}, API levels: "
      results = Hash[$ndk_data.compiler_types.map { |v| [v, []] }]
      $ndk_data.allowed_api_levels(abi, api_levels).each do |apilev|
        # create toolchain and run tests
        toolchain = Toolchain.new(abi, gccver, stl, apilev)
        # run_test returns a hash indexed by compiler type
        # where each hash value is a number of failed tests
        r = toolchain.run_tests
        $ndk_data.compiler_types.each do |compiler|
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
      $ndk_data.compiler_types.each do |compiler|
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
