#!/usr/bin/env ruby

require "rubTap"
require "testConfig"

# reference strings
class TestPut < RubTap::TestCase
	def global_setup
		@ioc = common_setup_local
		@ioc.talk( "< init", 5)
		@testcache_reference = IO.read("testcache26MAR2008.darwinx86")
		@testnotifysync_reference = IO.read("testnotifysync26MAR2008.darwinx86")
		@testnotifyasync_reference = IO.read("testnotifyasyn26MAR2008.darwinx86")
	end

	def global_teardown
		@ioc.command("exit")
	end

	def test_cache_notify_sync_async
		@ioc.talk("< p2sec")
		cache_response = @ioc.talk( "< testcache", 20)
		cache_response.gsub!("\r\n","\n")
		debug(cache_response,2)
		#puts @cache_response
		assert_equal(@testcache_reference,cache_response)
	end

	def test_notifysync
		@ioc.talk("< p0sec")
		notifysync_response =  @ioc.talk("< testnotifysync", 10)
		notifysync_response.gsub!("\r\n","\n")
		debug(notifysync_response,2)
		#puts @testnotifysync_response
		assert_equal(@testnotifysync_reference,notifysync_response)
	end

	def test_notifyasync
		@ioc.talk("< p2sec")
		notifyasync_response =  @ioc.talk("< testnotifyasyn", 80)
		notifyasync_response.gsub!("\r\n","\n")
		debug(notifyasync_response,2)
		#puts @testnotifyasync_response
		assert_equal(@testnotifyasync_reference,notifyasync_response)
	end
end

class TestPutVxWorks < TestPut
	def global_setup
		@ioc = common_setup_vxWorks
		debug(Dir.pwd, 2)
		@testcache_reference = IO.read("testcache15SEP2004")
		@testnotifysync_reference = IO.read("testnotifysync15SEP2004")
		@testnotifyasync_reference = IO.read("testnotifyasyn15SEP2004")
		@ioc.talk( "< init", 5)
	end

	def global_teardown
		@ioc.command("reboot")
		sleep(60)
		@ioc.pipe.close
	end
end


