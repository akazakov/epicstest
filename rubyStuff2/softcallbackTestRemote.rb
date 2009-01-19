#!/usr/bin/env ruby
require "rubTap"
require "testConfig"

# reference strings
class TestSoftCallbackRemote < RubTap::TestCase
	def global_setup
	
		# this function reads config 
		# and starts the ioc
		@ioc = common_setup_remote
		@tpcb = @localBinDir + "/" +  @config.findConfig("client")
		@bo_client = @tpcb + " mrk:bosyn mrk:boasyn mrk:bodelay 1 0"
		@ao_client = @tpcb + " mrk:aosyn mrk:aoasyn mrk:aodelay 1 0"
		@calcout_client = @tpcb + " mrk:calcoutsyn mrk:calcoutasyn mrk:calcoutdelay 1 0"
		@longout_client = @tpcb + " mrk:longoutsyn mrk:longoutasyn mrk:longoutdelay 1 0"
		@mbbo_client = @tpcb + " mrk:mbbosyn mrk:mbboasyn mrk:mbbodelay 1 0"
		@mbboDirect_client = @tpcb + " mrk:mbboDirectsyn mrk:mbboDirectasyn mrk:mbboDirectdelay 1 0"
		@stringout_client = @tpcb + " mrk:stringoutsyn mrk:stringoutasyn mrk:stringoutdelay 1 0"
		@client_reference  = "testDevAsyn OK"
	end

	def global_teardown
		@ioc.command("exit")
		common_teardown
	end

	def setup
		@ioc.read_all(1)
	end
	
	def test_bo_record
		do_test(@client_reference, create_reference("bo"), Plumber::GenericCommand.new(@bo_client,@shellCmd))
	end

	def test_ao_record
		do_test(@client_reference, create_reference("ao"), Plumber::GenericCommand.new(@ao_client,@shellCmd))
	end

	def test_calcout_record
		do_test(@client_reference, create_reference("calcout"), Plumber::GenericCommand.new(@calcout_client,@shellCmd))
	end

	def test_longout_record
		do_test(@client_reference, create_reference("longout"), Plumber::GenericCommand.new(@longout_client,@shellCmd))
	end

	def test_mbbo_record
		do_test(@client_reference, create_reference("mbbo"), Plumber::GenericCommand.new(@mbbo_client,@shellCmd))
	end

	def test_mbboDirect_record
		do_test(@client_reference, create_reference("mbboDirect"), Plumber::GenericCommand.new(@mbboDirect_client,@shellCmd))
	end

	def test_stringout_record
		do_test(@client_reference, create_reference("stringout"), Plumber::GenericCommand.new(@stringout_client,@shellCmd))
	end

	def create_reference(record)
		["Starting asynchronous processing: mrk:#{record}delay\n","Completed asynchronous processing: mrk:#{record}delay\n"]
	end

	

	def do_test(client_reference, reference, client)
		client_response = client.read_all(5)
		@ioc.command("")
		response = @ioc.read_all_lines(2)
		count = [0,0]
		response.each do |line| 
			if line == reference[0] then count[0] += 1 end
			if line == reference[1] then count[1] += 1 end
		end
		assert_equal([2,2],count)
	end

end
