#!/usr/bin/env ruby
if ENV['EPICS_RUBY_DIR'] then
 $LOAD_PATH << ENV['EPICS_RUBY_DIR']
end

f = File.open("/dev/ttyS0", "r+")
f.puts "pwd"
puts f.read_nonblock(1024)
f.puts "pwd"
sleep(10)
puts f.read_nonblock(1024)
f.close
