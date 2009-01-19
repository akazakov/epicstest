#!/usr/bin/env ruby
#if ENV['EPICS_RUBY_DIR'] then
# $LOAD_PATH << ENV['EPICS_RUBY_DIR']
#end
EPICS_HOST_ARCH = ENV['EPICS_HOST_ARCH']
if EPICS_HOST_ARCH.nil? then
	puts "please set EPICS_HOST_ARCH !!!"
	exit
end

require "rubTap"
require "plumber"
require "testConfig"

# reference strings
class TestCA < RubTap::TestCase
	def setup
	
		# this function reads config 
		# and starts the ioc
		@ioc = common_setup_local

		#@ioc.talk( "< init", 5)
		@client_caput = Plumber::GenericCommand.new("#{@localBinDir}/testcaput 5000 5")
		@client_caget = Plumber::GenericCommand.new("#{@localBinDir}/testcaget 5000 1")
		@client_caget.debug_level = @debug_level
		@client_caput.debug_level = @debug_level
		@disconnect_message = /Warning: "Virtual circuit disconnect"/
		@unresponsive_message = /Warning: "Virtual circuit unresponsive"/
	end

	def teardown
		@client_caget.exit
		@client_caput.exit
		common_teardown
	end

	def global_teardown
		@ioc.kill
	end

	# normal exit
	def test_cortesy_exit
		# first read all from clients if there is anything 
		@client_caput.read_all(0.7)
		@client_caget.read_all(0.7)
		start_time = Time.now
		# stop ioc
		# and check response
		@ioc.command("exit")
		response_caput = @client_caget.read_all(0.3)
		response_caget = @client_caput.read_all(0.3)
		if response_caput =~ @disconnect_message and response_caget =~ @disconnect_message then 
			RubTap::OK
		else
			RubTap::NOT_OK
		end
	end

	# suspend
	def test_sudden_death
		@client_caput.read_all(0.7)
		@client_caget.read_all(0.7)
		start_time = Time.now
		@ioc.suspend
		response_caput = @client_caget.read_all(15)
		response_caget = @client_caput.read_all(15)
		#up to now it should not have cisrcuits disconnected
		if response_caput =~ @unresponsive_message or response_caget =~ @unresponsive_message then 
			[RubTap::NOT_OK, "Disconnected too fast!"]
		else
			sleep 6
			response_caput = @client_caget.read_all(2)
			response_caget = @client_caput.read_all(1)
			if response_caput =~ @unresponsive_message and response_caget =~ @unresponsive_message then 
				RubTap::OK
			else
				RubTap::NOT_OK
			end
		end
	end
end
