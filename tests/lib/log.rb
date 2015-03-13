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

require 'time'
require 'fileutils'

class Log
    @@loggers = []
    @@mtx = Mutex.new

    def self.add(logger)
        @@mtx.synchronize do
            @@loggers << logger
        end
    end

    FATAL   = 0
    ERROR   = 1
    WARNING = 2
    NOTICE  = 3
    INFO    = 4
    DEBUG   = 5

    [:debug, :info, :notice, :warning, :error, :fatal].each do |sym|
        define_singleton_method(sym) do |msg, options = {}|
            level = Log.const_get(sym.to_s.upcase)
            ls = []
            @@mtx.synchronize do
                ls = @@loggers.select { |l| l.level >= level }
            end
            ls.each do |l|
                message = ""
                message << Time.now.utc.strftime('%Y-%m-%d %H:%M:%S.%3N UTC: ') unless options[:noprefix]
                case level
                when FATAL
                    message << "FATAL: "
                when ERROR
                    message << "ERROR: "
                when WARNING
                    message << "WARNING: "
                end
                message << msg
                l.log(message, level, options)
            end
        end
    end
end

class Logger
    class PureVirtualMethodCalled < Exception; end

    attr_reader :level

    def initialize(level = Log::DEBUG)
        @level = level
    end

    def log(msg, level, options)
        raise PureVirtualMethodCalled.new
    end
end

class StdLogger < Logger
    def initialize(level = Log::DEBUG)
        super(level)
        @mtx = Mutex.new
    end

    def log(msg, l, options)
        @mtx.synchronize do
            STDOUT.write msg
            STDOUT.write "\n" unless options[:nonewline]
            STDOUT.flush
        end
    end
end

class FileLogger < Logger
    def initialize(file, level = Log::DEBUG)
        super(level)
        @file = file
    end

    def log(msg, l, options)
        File.open(@file, "a") do |f|
            f.flock(File::LOCK_EX)
            f.write msg
            f.write "\n" unless options[:nonewline]
        end
    end
end
