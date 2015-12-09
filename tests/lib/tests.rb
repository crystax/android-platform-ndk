# Copyright (c) 2011-2015 CrystaX.
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

require_relative 'project'

class Tests
    attr_reader :type

    def initialize(ndk, options = {})
        @ndk = ndk
        @options = options
        @type = options[:type]
        @tests = Dir.glob(File.join(@ndk, 'tests', @type, '*')).sort
    end

    def run
        fails = 0
        @tests.each do |t|
            next if @options[:tests] && !@options[:tests].include?(File.basename(t))

            proj = Project.new(t, @ndk, @options)
            begin
                proj.test
            rescue
                raise unless @options[:keep_going]
                fails += 1
            else
                proj.cleanup
            end
        end

        raise "Testing failed" if fails > 0
    end
end

class BuildTests < Tests
    def initialize(ndk, options = {})
        super(ndk, options.merge(type: 'build'))
    end
end

class DeviceTests < Tests
    def initialize(ndk, options = {})
        super(ndk, options.merge(type: 'device'))
    end
end
