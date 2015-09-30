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


class Options

  attr_accessor :host_os, :host_cpu, :target_os, :target_cpu, :num_jobs, :log_file, :out_dir
  attr_writer :no_clean, :no_check, :force, :verbose, :rename_log

  def initialize
    os, cpu = Options.get_host_platform
    @host_os = os
    @host_cpu = cpu
    @target_os = os
    @target_cpu = cpu
    @num_jobs = '16'
    @log_file = nil
    @out_dir = nil
    #
    @no_clean = false
    @no_check = false
    @force = false
    @verbose = false
    @rename_log = true
  end

  def no_clean?
    @no_clean
  end

  def no_check?
    @no_check
  end

  def force?
    @force
  end

  def verbose?
    @verbose
  end

  def rename_log?
    @rename_log
  end

  def host_platform
    "#{host_os}-#{host_cpu}"
  end

  def target_platform
    "#{target_os}-#{target_cpu}"
  end

  def target_platform_dir
    (target_os == 'windows' and target_cpu == 'x86') ? 'windows' : target_platform
  end

  def same_platform?
    host_platform == target_platform
  end

  def self.host_platform
    os, cpu = get_host_platform
    "#{os}-#{cpu}"
  end

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
end
