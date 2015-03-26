#!/usr/bin/env ruby
#
# Install CREW
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

module Crystax

  PKG_NAME = 'dummy'

end

require 'fileutils'
require_relative 'versions.rb'
require_relative 'commander.rb'
require_relative 'logger.rb'


USAGE_STR = <<-EOS
Usage: #{$PROGRAM_NAME} [OPTIONS]
where OPTIONS are:
  --out32-dir=STR  destination dir for 32-bit release, as defined in
                   package-release.sh build script
  --out64-dir=STR  destination dir for 64-bit release, as defined in
                   package-release.sh build script
  --log-file=STR   log filename; default value is taken from environment
                   variable 'TMPLOG'
  --help           output this message and exit
EOS


INTERNAL_CREW_URL = 'git@git.crystax.net:android/crew.git'
PUBLIC_CREW_URL   = ''


begin
  out32_dir = nil
  out64_dir = nil
  logfile = ENV['TMPLOG'] ? ENV['TMPLOG'] : ENV['NDK_LOGFILE']

  # parse command line options
  ARGV.each do |arg|
    case arg
    when /^--out32-dir=(\S+)/
      out32_dir = $1
    when /^--out64-dir=(\S+)/
      out64_dir = $1
    when /^--log-file=(\S+)/
      logfile = $1
    when '--help'
      puts USAGE_STR
      exit 1
    else
      raise "unknown option #{arg}"
    end
  end

  Logger.set_no_rename
  Logger.open_log_file logfile
  Logger.msg "Intalling CREW"

  # test that all vars are set
  raise "use --out32-dir=STR to set output dir for 32 bit release" unless out32_dir
  raise "use --out64-dir=STR to set output dir for 64 bit release" unless out64_dir

  # find out what we are packaging: public release or internal build
  # if version in RELEASE.TXT looks like crystax-ndk-N.N.N then were are packagin a public build
  # if version has the form crystax-ndk-N.N.N-something then it's internal build
  text = File.read("#{out32_dir}/RELEASE.TXT")
  ndkver = text.split(' ')[0]
  if (ndkver.split('-').size == 4)
    crewurl = INTERNAL_CREW_URL
  else
    crewurl = PUBLIC_CREW_URL
    raise "public CREW repository not ready yet"
  end

  FileUtils.cd(out32_dir) { Commander.run "git clone -b master #{crewurl}" }
  FileUtils.cd(out64_dir) { Commander.run "git clone -b master #{crewurl}" }

rescue SystemExit => e
  exit e.status
rescue Exception => e
  Logger.log_exception(e)
  exit 1
ensure
  Logger.close_log_file
end
