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
class TestPut 
	def setup
		if ARGV[0] == "vxWorks" then 
			command = <<HERE
cd "/home/phoebus/KAZAKOV/soft-test/trunk/iocBoot/iocput"
< st.cmd
HERE
			@ioc = IocTalk::Ioc.new(command, :serial, "/dev/ttyS0")
		else
			@ioc = IocTalk::Ioc.new("../../bin/#{EPICS_HOST_ARCH}/common put.main")
		end
		puts @ioc.talk( /[a-zA-Z0-9]*>/, "< init")
		if ARGV[0] == "vxWorks" then 
			@testcache_reference = IO.read("testcache15SEP2004crn")
			@testnotifysync_reference = IO.read("testnotifysync15SEP2004")
			@testnotifyasync_reference = IO.read("testnotifyasyn15SEP2004")
		else
			@testcache_reference = IO.read("testcache26MAR2008.darwinx86")
			@testnotifysync_reference = IO.read("testnotifysync26MAR2008.darwinx86")
			@testnotifyasync_reference = IO.read("testnotifyasyn26MAR2008.darwinx86")
		end
	end

	def teardown
		@ioc.command('exit')
	end

	def test_cache_notify_sync_async
		puts @ioc.talk( /^[a-zA-Z0-9]*>/, "< p2sec")
		cache_response = @ioc.talk( /^[a-zA-Z0-9]*>/, "< testcache", 30)
		#cache_response  = @ioc.talk( /^[a-zA-Z0-9]*>/)
		puts cache_response
		#puts "--------------"
		#puts @testcache_reference
		if @testcache_reference == cache_response then puts "YO"
		else 
			puts @test_cache_reference
		end
	end
end

obj = TestPut.new
obj.setup
obj.test_cache_notify_sync_async
obj.teardown
