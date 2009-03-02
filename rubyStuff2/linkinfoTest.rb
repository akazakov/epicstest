require "EPICSTestUtils" 

class MetaTestLinkInfo < EPICSTestUtils::TestCase
	SEVR_REGEXP = /connected nelements [0-9]+ sevr [0-9]+$/
	TIME_REGEXP = /^time/
	def test_linkifo
		response = @ioc_.command_response(@custom_command)
		linkinfo_response = response.to_a
		assert_equal(@testlinkinfo_local_reference.size,linkinfo_response.size)
		i = 0
		linkinfo_response.each do |line|
			ref_line = @testlinkinfo_local_reference[i]
			i += 1
			assert do
				if line == ref_line then
					next true
				else
					if ((line =~ TIME_REGEXP) && (ref_line =~ TIME_REGEXP)) || ((line =~ SEVR_REGEXP) && (ref_line =~ SEVR_REGEXP)) then 
						next true
					else
						false
					end
				end
			end
		end
		:OK
	end
end
class TestLinkInfoLocal < MetaTestLinkInfo
	def global_setup
		@testlinkinfo_local_reference = IO.readlines("#{@options.topDir}/#{@options.referenceDir}/linkInfoLocalReference")
		@custom_command = "< localCommands"
		@ioc_ = ioc[0]
	end
end

class TestLinkInfoRemote < MetaTestLinkInfo
	def global_setup
		@testlinkinfo_local_reference = IO.readlines("#{@options.topDir}/#{@options.referenceDir}/linkInfoRemoteReference")
		@custom_command = "< remoteCommands"
		@ioc_ = ioc[1]
	end
end

config = Cfg.load("newconf.yml")
mtc1 = TestLinkInfoRemote.new(EPICSTestUtils::TAPFormatter.new, config)
mtc2 = TestLinkInfoLocal.new(EPICSTestUtils::TAPFormatter.new, config)
mtc1.run
mtc2.run
