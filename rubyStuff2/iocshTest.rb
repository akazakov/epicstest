require 'EPICSTestUtils'
require 'ruby-debug'
Cfg.load("newconf.yml")
config = Cfg.c["TestRemoteIOCSh"]
ioc = EPICSTestUtils::IOCSSH.new(config["IOC0"])
ioc.start
puts ioc.read
ioc.command("dbl")
debugger
puts ioc.read
