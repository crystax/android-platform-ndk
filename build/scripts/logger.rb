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
require_relative 'common.rb'


class Logger
  def self.open_log_file(name, rename, verbose)
    if File.exists?(name) and rename
      rename_logfile(name)
    else
      dir = File.dirname(name)
      FileUtils.mkdir_p(dir) unless Dir.exists?(dir)
    end
    @@log_file = File.open(name, 'a')
    @@verbose = verbose
  end

  def self.start_msg(archive)
    msg "building #{archive}; args: #{ARGV}"
  end

  def self.msg(msg)
    puts msg
    file_msg msg
  end

  def self.log_msg(msg)
    file_msg msg
    puts msg if @@verbose
  end

  def self.file_msg(msg)
    @@log_file.puts msg if @@log_file
  end

  def self.log_exception(exc)
    puts "error: #{exc}"
    puts exc.backtrace
    file_msg "error: #{exc}"
    file_msg exc.backtrace
  end

  private

  @@verbose = false
  @@log_file = nil

  def self.rename_logfile(name)
    n = 1
    n += 1 while File.exists?("#{name}.#{n}")
    File.rename(name, "#{name}.#{n}")
  end
end
