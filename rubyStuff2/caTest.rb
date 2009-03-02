require "EPICSTestUtils" 
class TestCA < EPICSTestUtils::TestCase
	# this is run after IOC and COMMAND initialization,
	# but before they are started. 
	def global_setup
		cmd[0].startcmd="#{cmd[0].localBinDir}/testcaput 5000 5"
		cmd[1].startcmd="#{cmd[1].localBinDir}/testcaget 5000 1"
		@client_caput = cmd[0]
		@client_caget = cmd[1]
		@unresponsive_message = /Warning: "Virtual circuit unresponsive"/
		@disconnect_message = /Warning: "Virtual circuit disconnect"/
	end
	def setup
		cmd[0].start
		cmd[1].start
		ioc[0].start
	end
	def teardown
		cmd[0].exit
		cmd[1].exit
	end
	
	# normal exit
	def test_courtesy_exit
		start_time = Time.now
		ioc[0].command("exit")
		response_caput = @client_caput.read_stderr(0.3)
		response_caget = @client_caget.read_stderr(0.3)
		assert(response_caput =~ @disconnect_message)
	  assert(response_caget =~ @disconnect_message)
	end

	# suspend
	def test_sudden_death
		ioc[0].suspend
		response_caput = @client_caput.read_stderr(15)
		response_caget = @client_caget.read_stderr(15)
		#up to now it should not have cisrcuits disconnected
		#
		assert(!(response_caput =~ @unresponsive_message or response_caget =~ @unresponsive_message))  
		sleep 6
		response_caput = @client_caput.read_stderr(2)
		response_caget = @client_caget.read_stderr(1)
		assert(response_caput =~ @unresponsive_message)
	  assert(response_caget =~ @unresponsive_message)
	end
end
config = Cfg.load("newconf.yml")
mtc = TestCA.new(EPICSTestUtils::TAPFormatter.new, config)
mtc.run
