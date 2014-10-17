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

  attr_reader :ndk_path, :gcc_versions_for_64_archs, :tag
  attr_accessor :api_levels, :architectures, :gcc_versions, :llvm_versions

  def initialize
    user = ENV['USER']
    @ndk_path = '.'

    # ./test/standalone/run.sh will use the values the variable as log file name
    ENV['NDK_LOGFILE'] = "/tmp/ndk-#{user}/tests/standalone.log"

    @ndk_tmp_dir = "/tmp/ndk-#{user}/tmp"
    @ndk_build_tools_path = File.join(ndk_path, 'build', 'tools')

    @cmd_prefix =
      "export NDK_BUILDTOOLS_PATH=#{@ndk_build_tools_path}"         + " ; " +
      ". " + File.join(@ndk_build_tools_path, 'ndk-common.sh')      + " ; " +
      ". " + File.join(@ndk_build_tools_path, 'prebuilt-common.sh') + " ; "

    @api_levels = default_api_levels
    @architectures = default_architectures
    @gcc_versions = default_gcc_versions
    @gcc_versions_for_64_archs = ["4.9"]
    @llvm_versions = default_llvm_versions

    # todo: why 32 windows returns host_tag 'windows' but 64 -- cygwin-x86_64?
    @tag = (host_tag == 'cygwin-x86_64') ? 'windows-x86_64' : host_tag
  end

  def min_api_level(arch)
    case arch
    when /64/   then 20
    when /x86/  then 10
    when /mips/ then 15
    else             3
    end
  end

  def standalone_path(api_level, arch, gcc_version)
    File.join(@ndk_tmp_dir, "android-ndk-api#{api_level}-#{arch}-#{tag}-#{gcc_version}")
  end

  def toolchain_name_for_arch(arch, gcc_version)
    get_info_from_shell("echo $(get_toolchain_name_for_arch #{arch} #{gcc_version})")
  end

  def toolchain_prefix_for_arch(arch)
    get_info_from_shell("echo $(get_default_toolchain_prefix_for_arch #{arch})")
  end

  def use_32bit_tools
    @tag = host_tag32
  end

  private

  def default_api_levels
    get_info_from_shell("echo $API_LEVELS").split
  end

  def default_architectures
    get_info_from_shell("echo $DEFAULT_ARCHS").split
  end

  def default_gcc_versions
    get_info_from_shell("echo $DEFAULT_GCC_VERSION_LIST").split
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

  attr_reader :api_level, :results

  def self.make_all(arch, ver)
    $ndk_data.api_levels.map do |api|
      if Integer(api == 'L' ? 20 : api) >= $ndk_data.min_api_level(arch)
        Toolchain.new(arch, ver, api)
      end
    end.compact
  end

  def self.result_keys
    ['gcc'] + $ndk_data.llvm_versions.map {|ver| 'clang' + ver }
  end

  def initialize(arch, gcc_version, api_level)
    @arch = arch
    @gcc_version = gcc_version
    @api_level = api_level

    @install_dir_base = $ndk_data.standalone_path(@api_level, @arch, @gcc_version)
    @name = $ndk_data.toolchain_name_for_arch(@arch, @gcc_version)
    @prefix = $ndk_data.toolchain_prefix_for_arch(@arch)

    @results = Hash.new
    Toolchain.result_keys.each do |k|
      @results[k] = { tests_run: 0, tests_failed: 0, tests_failed_names: [] }
    end
  end

  def make_standalone_toolchain
    $ndk_data.llvm_versions.each do |llvm_ver|
      cmd = "./build/tools/make-standalone-toolchain.sh"      +
            " --platform=android-#{api_level}"                +
            " --install-dir=#{@install_dir_base}-#{llvm_ver}" +
            " --llvm-version=#{llvm_ver}"                     +
            " --toolchain=#{@name}"                           +
            " --system=#{$ndk_data.tag}"
      `#{cmd}`
      if $? != 0
        abort("failed to make standalone toolchain with command: #{cmd}")
      end
    end
  end

  def delete_standalone_toolchain
    $ndk_data.llvm_versions.each do |llvm_ver|
      FileUtils.remove_dir("#{@install_dir_base}-#{llvm_ver}")
    end
  end

  def run_tests(abi)
    # run tests with GCC toolchain
    # since GCC compilers are the same for each LLVM version
    # use first LLVM version to run GCC tests
    cmd = "./tests/standalone/run.sh " +
          " --no-sysroot"              +
          " --prefix=#{@install_dir_base}-#{$ndk_data.llvm_versions[0]}/bin/#{@prefix}-gcc"
    if abi
      cmd += " --abi=#{abi}"
    end
    results['gcc'] = run_test_cmd(cmd)
    # run tests with LLVM toolchains
    $ndk_data.llvm_versions.each do |llvm_ver|
      cmd = "./tests/standalone/run.sh " +
            " --no-sysroot"              +
            " --prefix=#{@install_dir_base}-#{llvm_ver}/bin/clang"
      if abi
        cmd += " --abi=#{abi}"
      end
      results['clang'+llvm_ver] = run_test_cmd(cmd)
    end
  end

  def run_test_cmd(cmd)
    output = `#{cmd}`
    r = (output.split("\n")[-1]).split
    if r[-1] == 'Success.'
      tests_run = Integer((r[0].split("/"))[0])
      tests_failed = 0
      tests_failed_names = []
    elsif r[2] == 'failed'
      tests_run = Integer(r[-1].delete("."))
      tests_failed = Integer(r[0])
      # todo: parse names of the failed tests
      tests_failed_names = []
    else
      abort("error: unexpected output from command: #{cmd}; output: #{output}")
    end
    { tests_run:          tests_run,
      tests_failed:       tests_failed,
      tests_failed_names: tests_failed_names
    }
  end
end


class GCC

  attr_reader :version

  def self.make_all(arch)
    case arch
    when /64/
      $ndk_data.gcc_versions_for_64_archs.map {|ver| GCC.new(arch, ver) }
    else
      $ndk_data.gcc_versions.map {|ver| GCC.new(arch, ver) }
    end
  end

  def initialize(arch, version)
    @version = version
    @toolchains = Toolchain.make_all(arch, version)
  end

  def make_standalone_toolchain
    @toolchains.each {|tc| tc.make_standalone_toolchain}
  end

  def run_tests(abi=nil)
    @toolchains.each {|tc| tc.run_tests(abi) }
  end

  def clean
    @toolchains.each {|tc| tc.delete_standalone_toolchain }
  end

  def results
    s = {}
    f = {}
    Toolchain.result_keys.each {|k| s[k] = [] ; f[k] = [] }
    @toolchains.each do |tc|
      tc.results.each do |k, r|
        $num_tests_run += r[:tests_run]
        $num_tests_failed += r[:tests_failed]
        if r[:tests_failed] > 0
          f[k] << [tc.api_level, r[:tests_failed]]
        else
          s[k] << tc.api_level
        end
      end
    end
    Toolchain.result_keys.each do |k|
      if s[k] == []
        s[k] = 'none'
      elsif f == []
        s[k] = 'all'
        f[k] = 'none'
      end
    end
    return [s, f]
  end
end


options = { clean: true }
OptionParser.new do |opts|
  opts.banner = "Usage: #{$PROGRAM_NAME} [options]"

  opts.on("--apis=LIST", String, "List of API levels;", "#{$ndk_data.api_levels}") do |l|
    $ndk_data.api_levels = l.split(',').map {|s|  (s == 'L') ? s : Integer(s) }
  end

  opts.on("--archs=LIST", String, "List of architectures;", "#{$ndk_data.architectures}") do |l|
    $ndk_data.architectures = l.split(',')
  end

  opts.on("--gcc-vers=LIST", String, "List of GCC version;", "#{$ndk_data.gcc_versions}") do |l|
    $ndk_data.gcc_versions = l.split(',')
  end

  opts.on("--use-32bit-tools", "Use 32bit versions of tools on 64bit host") do |_|
    $ndk_data.use_32bit_tools
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
$ndk_data.architectures.each do |name|
  puts "#{name}:"
  print "  initializing..."
  gccs = GCC.make_all(name)
  puts "OK"
  print "  making standalone toolchains..."
  gccs.each {|c| c.make_standalone_toolchain }
  puts "OK"
  print "  running tests..."
  gccs.each {|c| c.run_tests }
  puts "OK"
  puts "  results:"
  gccs.each do |gcc|
    puts "  #{gcc.version}"
    ok, failures = gcc.results
    Toolchain.result_keys.each do |compiler|
      puts "    #{compiler}"
      puts "      successful api levels: #{ok[compiler]}"
      puts "      failed api levels:     #{failures[compiler]}"
    end
  end
  if name == "arm"
    puts "armeabi-v7a:"
    print "  running tests..."
    gccs.each {|c| c.run_tests("armeabi-v7a") }
    puts "OK"
    puts "  results:"
    gccs.each do |gcc|
      puts "  #{gcc.version}"
      ok, failures = gcc.results
      Toolchain.result_keys.each do |compiler|
        puts "    #{compiler}"
        puts "      successful api levels: #{ok[compiler]}"
        puts "      failed api levels:     #{failures[compiler]}"
      end
    end
    puts "armeabi-v7a-hard:"
    print "  running tests..."
    gccs.each {|c| c.run_tests("armeabi-v7a-hard") }
    puts "OK"
    puts "  results:"
    gccs.each do |gcc|
      puts "  #{gcc.version}"
      ok, failures = gcc.results
      Toolchain.result_keys.each do |compiler|
        puts "    #{compiler}"
        puts "      successful api levels: #{ok[compiler]}"
        puts "      failed api levels:     #{failures[compiler]}"
      end
    end
  end
  if options[:clean]
    print "  cleaning..."
    gccs.each {|c| c.clean }
    puts "OK"
  end
end

puts
puts "Summary:"
puts "    number of tests run:    #{$num_tests_run}"
puts "    number of failed tests: #{$num_tests_failed}"
