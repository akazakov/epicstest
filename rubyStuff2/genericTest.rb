#!/usr/bin/env ruby
require "rubTap"
require "testConfig"

# reference strings
class TestName < RubTap::TestCase
	def global_setup
	
		# this function reads config 
		# and starts the ioc
		common_setup

		#@ioc.talk( "< init", 5)
		@reference = IO.read(@config.findConfig("reference")
	end

	def global_teardown
		@ioc.command("exit")
		common_teardown
	end

	#here goes test. All tests are run in alphabeteical order
	def test_name
		@ioc.talk("< p2sec")
		cache_response = @ioc.talk( "< testcache", 20)
		assert_equal(@testcache_reference,cache_response)
	end
end
ts = RubTap::TestSuite.new
ts << TestPut.new(TestConfig::AllTestsConfig.new("config.yml"))
ts.run
