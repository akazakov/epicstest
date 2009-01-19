#!/usr/bin/env ruby
# convert \n to \r\n

data = ARGF.read
if data =~ /\r\n/ then STDERR.puts "This file already in crn format"; exit; end
data.gsub!("\n","\r\n")
print data
