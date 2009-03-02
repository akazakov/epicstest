require "EPICSTestUtils" 
class TestPut < EPICSTestUtils::TestCase
	def global_setup
		@testcache_reference = IO.read("#{@options.topDir}/#{@options.referenceDir}/testcache26MAR2008.darwinx86")
		@testnotifysync_reference = IO.read("#{@options.topDir}/#{@options.referenceDir}/testnotifysync26MAR2008.darwinx86")
		@testnotifyasync_reference = IO.read("#{@options.topDir}/#{@options.referenceDir}/testnotifyasyn26MAR2008.darwinx86")
	end
	def global_setup_after_start
		ioc[0].talk("< init", 5)
	end

	def test_cache_notify_sync_async
		ioc[0].talk("< p2sec")
		cache_response = ioc[0].talk( "< testcache", 20)
		cache_response.gsub!("\r\n","\n")
		debug(cache_response,1)
		assert_equal(@testcache_reference,cache_response)
	end

	def test_notifysync
		ioc[0].talk("< p0sec")
		notifysync_response =  ioc[0].talk("< testnotifysync", 10)
		notifysync_response.gsub!("\r\n","\n")
		debug(notifysync_response,1)
		assert_equal(@testnotifysync_reference,notifysync_response)
	end

	def test_notifyasync
		ioc[0].talk("< p2sec")
		notifyasync_response =  ioc[0].talk("< testnotifyasyn", 80)
		notifyasync_response.gsub!("\r\n","\n")
		debug(notifyasync_response,1)
		assert_equal(@testnotifyasync_reference,notifyasync_response)
	end
end
=begin
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
=end
config = Cfg.load("newconf.yml")
mtc = TestPut.new(EPICSTestUtils::TAPFormatter.new, config)
mtc.run
