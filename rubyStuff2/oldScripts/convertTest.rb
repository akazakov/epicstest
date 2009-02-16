#!/usr/bin/env ruby
require "rubTap"
require "testConfig"

# reference strings
class TestConvert < RubTap::TestCase
	def global_setup

		# this function reads config
		# and starts the ioc
		@localIoc = common_setup_local
		@remoteIoc = common_setup_remote
		@testConvert = "#{@localBinDir}/testConvert"
		@testConvertENUM = "#{@localBinDir}/testConvertENUM"
		@reference = IO.read(@config.findConfig("reference"))
		@referenceENUM = IO.read(@config.findConfig("referenceENUM"))
	end

	def global_teardown
		@localIoc.command("exit")
		@remoteIoc.command("exit")
		common_teardown
	end

	#here goes test. All tests are run in alphabeteical order
	def test_conversion
		cmd1 = Plumber::GenericCommand.new(@testConvert)
		response1 = cmd1.read_all(90)
		debug(response1,1)
		assert_equal(@reference,response1)
	end
	def test_conversion_enum
		cmd2 = Plumber::GenericCommand.new(@testConvertENUM)
		response2 = cmd2.read_all(120)
		debug(response2,1)
		assert_equal(@referenceENUM,response2)
	end

end
