# Copyright (c) 2011-2015 CrystaX .NET.
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
# THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX .NET.

require 'open3'

require_relative 'log'

class StandaloneTests
    attr_reader :type

    def initialize(ndk, options = {})
        @ndk = ndk
        @options = options

        @abis = options[:abis] || ['armeabi', 'armeabi-v7a', 'armeabi-v7a-hard', 'x86', 'mips', 'arm64-v8a', 'x86_64', 'mips64']

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

    def variants
        v = ""
        v << " #{@options[:toolchain_version]}" unless @options[:toolchain_version].nil?
        v
    end
    private :variants

    def tmpdir(abi)
        File.join(@outdir, abi)
    end
    private :tmpdir

    def toolchain_dir(abi)
        File.join(tmpdir(abi), 'toolchain', @toolchain_version)
    end
    private :toolchain_dir

    def create_toolchain(abi)
        log_notice "STA create [#{abi}]#{variants}"

        FileUtils.rm_rf toolchain_dir(abi)

        case abi
        when /^armeabi/
            toolchain = 'arm-linux-androideabi'
            apilevel = 9
        when 'arm64-v8a'
            toolchain = 'aarch64-linux-android'
            apilevel = 21
        when 'mips'
            toolchain = 'mipsel-linux-android'
            apilevel = 9
        when 'mips64'
            toolchain = 'mips64el-linux-android'
            apilevel = 21
        when 'x86'
            toolchain = 'x86'
            apilevel = 9
        when 'x86_64'
            toolchain = 'x86_64'
            apilevel = 21
        else
            raise "Unknown ABI: #{abi.inspect}"
        end

        toolchain << "-#{@toolchain_version.sub(/^gcc-?/, '')}"

        script = File.join(@ndk, 'build', 'tools', 'make-standalone-toolchain.sh')
        raise "No #!/bin/bash in make-standalone-toolchain.sh" if File.read(script).split("\n").first != "#!/bin/bash"

        args = [script]
        args << "--verbose"
        args << "--install-dir=#{toolchain_dir(abi)}"
        args << "--abis=#{abi}"
        args << "--platform=android-#{apilevel}"
        args << "--toolchain=#{toolchain}"

        run_cmd args.join(' ')
    end
    private :create_toolchain

    def cleanup(abi)
        log_info "## CLEANUP: #{tmpdir(abi)}"
        FileUtils.rm_rf tmpdir(abi)
    end
    private :cleanup

    def run
        fails = 0

        @abis.each do |abi|
            # gcc-4.8 doesn't support 64-bit ABIs
            next if @toolchain_version == 'gcc4.8' && ['arm64-v8a', 'x86_64', 'mips64'].include?(abi)

            begin
                create_toolchain(abi)
            rescue
                raise unless @options[:keep_going]
                fails += 1
            else
                cleanup(abi)
            end
        end

        raise "Testing failed" if fails > 0
    end
end
