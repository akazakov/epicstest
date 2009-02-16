require 'EPICSTestUtils.rb'
include EPICSTestUtils
class FailingTest < EPICSTestUtils::Test
	def run
		@description = "Should fail"
		assert(false)
	end
end

class OKTest < EPICSTestUtils::Test
	def run
		@description = "Should be ok"
		assert(true)
	end
end

class EQUALTest < EPICSTestUtils::Test
	def run
		@description = "Should be ok"
		assert_equal("superstring", "superstring")
	end
end

class EQUALFailTest < EPICSTestUtils::Test
	def run
		@description = "Should fail"
		assert_equal("superstring","anotherstring")
	end
end

class RemoteHostsPwdTest < EPICSTestUtils::Test
   def run
		 hosts = @test_case.options["hosts"]
		 user = @test_case.options["user"]
		 topDir = @test_case.options["topDir"]
		 hosts.each_pair do |k,v| 
			 hostname = v["hostname"]
			 connection = Net::SSH.start(hostname, user)
			 pwd = connection.exec!("cd #{topDir} && pwd")
			 assert(pwd.strip == topDir.strip)
		 end
	 end
end

class RemoteHostnameTest < EPICSTestUtils::Test
   def run
		 hosts = @test_case.options["hosts"]
		 user = @test_case.options["user"]
		 topDir = @test_case.options["topDir"]
		 hosts.each_pair do |k,v| 
			 hostname = v["hostname"]
			 connection = Net::SSH.start(hostname, user)
			 rem_hostname = connection.exec!("hostname -s")
			 assert(rem_hostname.strip == hostname.strip)
		 end
	 end
end

class RemoteHostnameTestNew < EPICSTestUtils::Test
   def run
		 hosts = @test_case.options["hosts"]
		 user = @test_case.options["user"]
		 topDir = @test_case.options["topDir"]
		 Cfg.h.each do |k,v|
			 hostname = v.hostname
			 connection = Net::SSH.start(v.hostname, v.user)
			 rem_hostname = connection.exec!("hostname -s")
			 assert(rem_hostname.strip == v.hostname)
		 end
	 end
end


class RemoteBuildExampleIOC < EPICSTestUtils::Test
	def run
		tmpDir = @test_case.options["tmpDir"]
		iocName = @test_case.options["ioc"]
		Cfg.h.each do |k,v| 
			makeApp = v.epicsTopDir.strip + "/bin/" + v.epicsHostArch.strip + "/makeBaseApp.pl"
			connection = Net::SSH.start(v.hostname, v.user)
			homeDir = connection.exec!("cd && pwd")
			pwd = connection.exec!("mkdir -p #{tmpDir}; cd #{tmpDir}; pwd")
			puts v.hostname + ": " + pwd.strip
			assert( homeDir.strip + "/" + tmpDir.strip == pwd.strip)
			puts("cd #{tmpDir} && #{makeApp} -t example #{iocName} && #{makeApp} -p #{iocName}-i -t exmaple ")
			res = connection.exec!("cd #{tmpDir} && #{makeApp} -t example #{iocName} && #{makeApp} -a #{v.epicsHostArch} -p #{iocName} -i -t example #{iocName}")
			puts res
			if v.epicsHostArch =~ /freebsd/ then makeCommand = 'gmake' else makeCommand = 'make' end
			res = connection.exec!("cd #{tmpDir} && env EPICS_HOST_ARCH=#{v.epicsHostArch} #{makeCommand}")
			puts res
			ls = connection.exec!("ls #{tmpDir}")
		end
	end
end

class MyTestCase < EPICSTestUtils::TestCase
end

config = Cfg.load("test.yml")
mtc = MyTestCase.new(EPICSTestUtils::TAPFormatter.new, config)
mtc << FailingTest.new(mtc)
mtc << OKTest.new(mtc)
mtc << EQUALTest.new(mtc)
mtc << EQUALFailTest.new(mtc)
#mtc << RemoteHostsTest.new(mtc)
mtc << RemoteHostnameTest.new(mtc)
mtc << RemoteHostnameTestNew.new(mtc)
mtc << RemoteBuildExampleIOC.new(mtc)
mtc.run
