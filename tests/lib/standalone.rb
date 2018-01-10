# Copyright (c) 2011-2018 CrystaX.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX.

require 'open3'

require_relative 'log'
require_relative 'mro'

class StandaloneTests
  attr_reader :type

  def initialize(ndk, options = {})
    @type = :standalone

    @ndk = ndk
    @options = options

    @abis = options[:abis] || ['armeabi-v7a', 'armeabi-v7a-hard', 'x86', 'mips', 'arm64-v8a', 'x86_64', 'mips64']

    @toolchain_version = options[:toolchain_version]
    raise "No toolchain version passed" if @toolchain_version.nil?

    @outdir = File.join(options[:outdir], 'standalone')
  end

  def elapsed(seconds)
    s = seconds.to_i % 60
    m = (seconds.to_i / 60) % 60
    h = seconds.to_i / 3600
    "%d:%02d:%02d" % [h,m,s]
  end
  private :elapsed

  def log_info(msg)
    Log.info msg
  end
  private :log_info

  def log_notice(msg)
    Log.notice msg
    @last_notice = Time.now
  end
  private :log_notice

  def run_cmd(cmd)
    log_info "## COMMAND: #{cmd}"
    log_info "## CWD: #{Dir.pwd}"

    Open3.popen3(cmd) do |i,o,e,t|
      [i,o,e].each { |io| io.sync = true }

      ot = Thread.start do
        while line = o.gets.chomp rescue nil
          log_info "   > #{line}"
        end
      end

      et = Thread.start do
        while line = e.gets.chomp rescue nil
          log_info "   * #{line}"
        end
      end

      wt = Thread.start do
        lt = Time.now
        while ot.alive? || et.alive?
          sleep 5
          now = Time.now
          next if now - @last_notice < 30
          log_notice "## STILL RUNNING (#{elapsed(now - lt)})"
        end
      end

      i.close
      ot.join
      et.join
      wt.kill

      t.join

      raise "#{File.basename(cmd.split(/\s+/).first)} failed" unless t.value.success?
    end
  end
  private :run_cmd

  def tmpdir(abi, cxxstdlib)
    File.join(@outdir, abi, cxxstdlib)
  end
  private :tmpdir


  def run
    ruby = File.join(RbConfig::CONFIG['bindir'], RbConfig::CONFIG['ruby_install_name'])

    log_notice "Starting standalone tests"
    run_cmd "#{ruby} #{@ndk}/tests/standalone/run-standalone-tests.rb"
  end
end
