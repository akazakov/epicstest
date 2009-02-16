#!/usr/bin/env ruby
require "rubTap"
require "testConfig"

class TestLinkInfo < RubTap::TestCase
	SEVR_REGEXP = /connected nelements [0-9]+ sevr [0-9]+$/
	TIME_REGEXP = /^time/
	def global_setup
		common_setup
		@testlinkinfo_local_reference = IO.readlines("#{@config.findConfig('reference')}")
	end

	def global_teardown
		@ioc.command('exit')
		Dir.chdir(@pwd)
	end

	def test_linkifo
		linkinfo_response = @ioc.talk_lines(@config.findConfig("command"))
		debug(linkinfo_response,1,"RESPONSE")
		assert_equal(@testlinkinfo_local_reference.size,linkinfo_response.size)
		i = 0
		linkinfo_response.each do |line|
			ref_line = @testlinkinfo_local_reference[i]
			i += 1
			debug(line + ref_line,2)
			unless line == ref_line then
				if ((line =~ TIME_REGEXP) && (ref_line =~ TIME_REGEXP)) || ((line =~ SEVR_REGEXP) && (ref_line =~ SEVR_REGEXP)) then 
					debug("next cycle",2)
					next
				else
					debug("Ooops!",2)
					#finish comparison and bail off
					return [RubTap::NOT_OK, "Output does not match to reference file"]
				end
			end
		end
		#all strings are equal => test is ok! return true
		debug("All strings OK",2)
		RubTap::OK
	end
end

class TestLinkInfoRemote < TestLinkInfo
	def global_setup
		@localIoc = common_setup_local
		@ioc = common_setup_remote
		@testlinkinfo_local_reference = IO.readlines("#{@config.findConfig('reference')}")
	end
	def global_teardown
		@ioc.command('exit')
		@localIoc.command('exit')
		Dir.chdir(@pwd)
	end
end

class TestLinkInfoLocal < TestLinkInfo
	def global_setup
		@ioc = common_setup_local
		@testlinkinfo_local_reference = IO.readlines("#{@config.findConfig('reference')}")
	end
end
