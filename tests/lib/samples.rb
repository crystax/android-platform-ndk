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

require_relative 'log'
require_relative 'project'
require_relative 'tests'

class SamplesTests < Tests
    def initialize(ndk, options = {})
        super ndk, options.merge(type: 'samples')

        dirs = []
        if !File.exists?(File.join(ndk, 'RELEASE.TXT'))
            # This is a development work directory, we will take the samples
            # directly from development/ndk.
            devndk = File.join(File.dirname(ndk), 'development', 'ndk')
            dirs << File.join(devndk, 'samples')
            Dir.glob(File.join(devndk, 'platforms', 'android-*', 'samples')).each do |dir|
                dirs << dir
            end
        end
        dirs << File.join(ndk, 'samples')

        @tests = []
        dirs.each do |dir|
            Dir.glob(File.join(dir, '*')).each do |p|
                @tests << p unless @tests.map { |e| File.basename(e) }.include?(File.basename(p))
            end
        end
        @tests.sort! { |a,b| File.basename(a) <=> File.basename(b) }
    end
end
