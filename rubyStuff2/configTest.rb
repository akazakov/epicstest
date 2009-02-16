require 'EPICSTestUtils'

Cfg.load("newconf.yml")
Cfg.h.each_pair do |k,v| 
	puts k + "=" + v.hostname
end
puts Cfg.h.localhost.hostname
puts Cfg.c.MyTestCase.IOC0.hostname
puts Cfg.c.MyTestCase.COMMAND0.hostname
puts Cfg.c.TestCase2.COMMAND0.hostname
puts Cfg.c.TestCase2.IOC0.hostname
puts Cfg.c.TestCase2.varname

puts Cfg.h.localhost.hostVar
puts Cfg.h.localhost.defaultVar


puts "======"
puts Cfg.c.MyTestCase.IOC0.topDir
puts Cfg.c.MyTestCase.IOC0.epicsTopDir
puts Cfg.c.MyTestCase.IOC1.topDir
puts Cfg.c.MyTestCase.IOC1.epicsTopDir
puts Cfg.c.MyTestCase.IOC1.debug_level
puts Cfg.c.MyTestCase.IOC1.hostname
puts Cfg.c.MyTestCase.IOC0.topDir
