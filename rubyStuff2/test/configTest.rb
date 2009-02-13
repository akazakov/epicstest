require "../EPICSTestUtils.rb"
include EPICSTestUtils

config = EPICSTestUtils::Cfg.load("test.yml")
puts EPICSTestUtils::Cfg.default.debug_level
#puts EPICSTestUtils::Cfg.h.host1.hostname
#puts EPICSTestUtils::Cfg.h.host1.debug_level
Cfg.h.each do |k,v| 
	puts v.hostname
end
puts Cfg.h.localhost.hostname
puts Cfg.c.MyTestCase.IOC0.hostname
puts Cfg.c.MyTestCase.COMMAND0.hostname
puts Cfg.c.TestCase2.COMMAND0.hostname
puts Cfg.c.TestCase2.IOC0.hostname
puts Cfg.c.TestCase2.varname
