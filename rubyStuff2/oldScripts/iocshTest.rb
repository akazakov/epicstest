#!/usr/bin/env ruby
if ENV['EPICS_RUBY_DIR'] then
 $LOAD_PATH << ENV['EPICS_RUBY_DIR']
end
EPICS_HOST_ARCH = ENV['EPICS_HOST_ARCH']
if EPICS_HOST_ARCH.nil? then
	puts "please set EPICS_HOST_ARCH !!!"
	exit
end

require "iocTalk"
Dir.chdir("iocBoot/iocput")

# reference strings
class TestIOCSH 
	def setup
		@ioc = IocTalk::Ioc.new("../../bin/#{EPICS_HOST_ARCH}/common put.main")
	end

	def teardown
		@ioc.command('exit')
	end

	def test_iocsh_redout
		puts @ioc.talk( /^epics>/, "dbl")
		response = @ioc.talk( /^epics>/)
		puts response
	end
end

obj = TestIOCSH.new
obj.setup
obj.test_iocsh_redout
obj.teardown
