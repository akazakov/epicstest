require "EPICSTestUtils" 

class MyTest < EPICSTestUtils::Test
	@@title = "My Test"
	@@description = "Super test description"
	@@explanation = "Explanation DA YO!"
   def run
		 @result = :OK
		 @caputacks = "caputacks"
		 @caputackt = "caputackt"
		 @cagetalarm = "cagetalarm"
		 @getclassname = "cagetclassname"
		 @acktvalues = ["0","1"]
		 @acksvalues = ["1","2"]
		 @values = ["2","4","10","16","18"]
		 @noalarmvalue = "10"
		 @channel = "mrkai"
		 reference = IO.read("#{@test_case.options['referenceDir']}/alarmTest")
		 result = ''
		 @acktvalues.each do |ackt|
			 result << cmd[0].exec("#{@caputackt} #{@channel} #{ackt}")
			 @values.each do |v|
				 @acksvalues.each do |acks|
					 result << cmd[0].exec("caput -t #{@channel} #{v}")
					 result << cmd[0].exec("#{@cagetalarm} #{@channel}")
					 result << cmd[0].exec("echo caputacks #{@channel} #{acks}")
					 result << cmd[0].exec("#{@caputacks} #{@channel} #{acks}")
					 result << cmd[0].exec("#{@cagetalarm} #{@channel}")
					 result << cmd[0].exec("caput -t #{@channel} 10.0")
					 result << cmd[0].exec("#{@cagetalarm} #{@channel}")
				 end
			 end
		 end
		 if result != reference @result = :NOT_OK
		 else @result = :OK
		 end
   end
end
class MyTestCase < EPICSTestUtils::TestCase
end
config = EPICSTestUtils::Cfg.load("newconf.yml")
myConf = EPICSTestUtils::Cfg.find(config,"MyTestCase")
mtc = MyTestCase.new(EPICSTestUtils::TAPFormatter.new, "MyTest", myConf)
mtc << MyTest.new(mtc.formatter,mtc)
mtc.run
