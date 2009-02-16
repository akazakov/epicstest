#!/usr/bin/env ruby
require "rubTap"
require "plumber"
require "testConfig"

# reference strings
class TestAlarm < RubTap::TestCase
	def global_setup
		@ioc = common_setup_local
		@reference = IO.read(@config.findConfig('reference'))
		@class_reference = "pv: mrkai classname ai\n"
		@channel = @config.findConfig('channel')
		@caputacks = "#{@localBinDir}/caputacks"
		@caputackt = "#{@localBinDir}/caputackt"
		@cagetalarm = "#{@localBinDir}/cagetalarm"
		@getclassname = "#{@localBinDir}/cagetclassname"
		@acktvalues = ["0","1"]
		@acksvalues = ["1","2"]
		@values = ["2","4","10","16","18"]
		@noalarmvalue = "10"
	end

	def global_teardown
		@ioc.command("exit")
		Dir.chdir(@pwd)
	end

	def test_alarm
		result = ''
		@acktvalues.each do |ackt| 
			result << `#{@caputackt} #{@channel} #{ackt}`
			@values.each do |v| 
				@acksvalues.each do |acks| 
					result <<	`caput -t #{@channel} #{v}`
					result << `#{@cagetalarm} #{@channel}`
					result << `echo caputacks #{@channel} #{acks}`
					result << `#{@caputacks} #{@channel} #{acks}`
					result << `#{@cagetalarm} #{@channel}`
					result << `caput -t #{@channel} 10.0`
					result << `#{@cagetalarm} #{@channel}`
				end
			end
		end
		debug(result,1)
		assert_equal(@reference,result)
	end

	def test_get_class_name
		result = `#{@getclassname} #{@channel}`
		debug(result,1)
		assert_equal(@class_reference,result)
	end
end

class TestAlarmVxWorks < TestAlarm
	def global_setup
	@ioc = common_setup_vxWorks
		@reference = IO.read(@config.findConfig('reference'))
		@class_reference = "pv: mrkai classname ai\n"
		@channel = @config.findConfig('channel')
		@caputacks = "#{@localBinDir}/caputacks"
		@caputackt = "#{@localBinDir}/caputackt"
		@cagetalarm = "#{@localBinDir}/cagetalarm"
		@getclassname = "#{@localBinDir}/cagetclassname"
		@acktvalues = ["0","1"]
		@acksvalues = ["1","2"]
		@values = ["2","4","10","16","18"]
		@noalarmvalue = "10"
	end

	def global_teardown
		@ioc.command("reboot")
		sleep(60)
		@ioc.pipe.close
	end
end
