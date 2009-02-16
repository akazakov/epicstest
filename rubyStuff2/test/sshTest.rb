require '../EPICSTestUtils.rb'
include EPICSTestUtils

class RemoteHostsPwdTest < EPICSTestUtils::Test
   def run
		 hosts = @test_case.options["hosts"]
		 user = @test_case.options["user"]
		 topDir = @test_case.options["topDir"]
		 hosts.each_pair do |k,v| 
			 hostname = v["hostname"]
			 connection = Net::SSH.start(hostname, user)
			 connection.open_channel do |ch|
				 ch.exec "asdfasdf " do |ch, success|
					 abort "could not execute command" unless success
					 ch.on_data do |chn, data|
						 #ch.send_data "something for stdin\n"
					#	 puts data
					 end
					 ch.on_extended_data do |chn, type, data|
						 puts data
					 end
					 ch.on_close do |ch|
						 abort "MAZA FAKA!"
					 end
				 end
			 end
 
			 pwd = connection.exec!("cd #{topDir} && pwd")
			 assert(pwd.strip == topDir.strip)
		 end
	 end
end

class MyTestCase < EPICSTestUtils::TestCase
end

config = EPICSTestUtils::Cfg.load("test.yml")
mtc = MyTestCase.new(EPICSTestUtils::TAPFormatter.new, config)
mtc << RemoteHostsPwdTest.new(mtc)
mtc.run
