require "EPICSTestUtils" 
class TestConvert < EPICSTestUtils::TestCase
	def global_setup
		@testConvert = "#{cmd[0].localBinDir}/testConvert"
		@testConvertENUM="#{cmd[0].localBinDir}/testConvertENUM"
		@reference = IO.read("#{@options.topDir}/#{@options.referenceDir}/convertReference")
		@referenceENUM = IO.read("#{@options.topDir}/#{@options.referenceDir}/convertENUMReference")
		@local_ioc = ioc[0]
		@remote_ioc = ioc[1]
	end

	def test_conversion
		response1 = cmd[0].exec(@testConvert)
		assert_equal(@reference,response1)
	end

	def test_conversion_enum
		response2 = cmd[0].exec(@testConvertENUM)
		assert_equal(@referenceENUM,response2)
	end
end

config = Cfg.load("newconf.yml")
mtc = TestConvert.new(EPICSTestUtils::TAPFormatter.new, config)
mtc.run
