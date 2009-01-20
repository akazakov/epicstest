require "../EPICSTestUtils.rb"
require 'profile'
include EPICSTestUtils

config = EPICSTestUtils::Cfg.load("test.yml")
puts EPICSTestUtils::Cfg.default.debug_level
puts EPICSTestUtils::Cfg.h.host1.hostname
puts EPICSTestUtils::Cfg.h.host1.debug_level
Cfg.h.each do |k,v| 
	puts v.hostname
end
