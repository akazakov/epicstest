require "EPICSTestUtils" 
class TestSoftCallback < EPICSTestUtils::TestCase
	def global_setup
		tpcb = cmd[0].localBinDir + "/" + "testPutCallback" 
		@bo_client = tpcb + " mrk:bosyn mrk:boasyn mrk:bodelay 1 0"
		@ao_client = tpcb + " mrk:aosyn mrk:aoasyn mrk:aodelay 1 0"
		@calcout_client = tpcb + " mrk:calcoutsyn mrk:calcoutasyn mrk:calcoutdelay 1 0"
		@longout_client = tpcb + " mrk:longoutsyn mrk:longoutasyn mrk:longoutdelay 1 0"
		@mbbo_client = tpcb + " mrk:mbbosyn mrk:mbboasyn mrk:mbbodelay 1 0"
		@mbboDirect_client = tpcb + " mrk:mbboDirectsyn mrk:mbboDirectasyn mrk:mbboDirectdelay 1 0"
		@stringout_client = tpcb + " mrk:stringoutsyn mrk:stringoutasyn mrk:stringoutdelay 1 0"
		@client_reference  = "testDevAsyn OK"
	end

	def setup
		ioc[0].read(1)
	end
	
	def test_bo_record
		do_test(@client_reference, create_reference("bo"), cmd[0].exec(@bo_client))
	end

	def test_ao_record
		do_test(@client_reference, create_reference("ao"), cmd[0].exec(@ao_client))
	end

	def test_calcout_record
		do_test(@client_reference, create_reference("calcout"), cmd[0].exec(@calcout_client))
	end

	def test_longout_record
		do_test(@client_reference, create_reference("longout"), cmd[0].exec(@longout_client))
	end

	def test_mbbo_record
		do_test(@client_reference, create_reference("mbbo"), cmd[0].exec(@mbbo_client))
	end

	def test_mbboDirect_record
		do_test(@client_reference, create_reference("mbboDirect"), cmd[0].exec(@mbboDirect_client))
	end

	def test_stringout_record
		do_test(@client_reference, create_reference("stringout"), cmd[0].exec(@stringout_client))
	end

	def create_reference(record)
		["Starting asynchronous processing: mrk:#{record}delay\n","Completed asynchronous processing: mrk:#{record}delay\n"]
	end

	def do_test(client_reference, reference, client_response)
		ioc[0].command("")
		response = ioc[0].get_response(3)
		debug(response,2)
		debug(reference,2)
		count = [0,0]
		response.each do |line| 
			line.gsub!("\r\n","\n")
			if line == reference[0] then count[0] += 1 end
			if line == reference[1] then count[1] += 1 end
		end
		assert_equal([2,2],count)
	end
end

=begin
class TestSoftCallbackVxWorks < TestSoftCallback
	def global_setup
		@ioc = common_setup_vxWorks
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
		@ioc.command("reboot")
		sleep(60)
		@ioc.pipe.close
	end
end
=end

config = Cfg.load("newconf.yml")
mtc = TestSoftCallback.new(EPICSTestUtils::TAPFormatter.new, config)
mtc.run
