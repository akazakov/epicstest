#!/usr/bin/env ruby

# reference strings
if ARGV[1] == "vxWorks" then 
	testcache_reference = IO.read("testcache15SEP2004")
	testnotifysync_reference = IO.read("testnotifysync15SEP2004")
	testnotifyasync_reference = IO.read("testnotifyasync15SEP2004")
else
	testcache_reference = IO.read("testcache26MAR2008.darwinx86")
	testnotifysync_reference = IO.read("testnotifysync26MAR2008.darwinx86")
	testnotifyasync_reference = IO.read("testnotifyasyn26MAR2008.darwinx86")
end

def talk(pipe, pattern, command = nil)
	line = ''
	response = ''
	while true do
		line << ch = pipe.getc
		unless (line =~ pattern).nil? 
			unless command.nil? then
			 	pipe.puts command
				#read back our command
				pipe.gets
			end
			break
		else
			if ch == 10 then 
				response.concat(line) 
				line = ''
		 	end
		end
	end
	response
end

def get_response(pipe, pattern)
	line = ""
	response = ""
	while true do
		line << ch = pipe.getc
		unless (line =~ pattern).nil? 
			break
		else
			if ch == 10
				response.concat(line) 
				line = ""
			end
		end
	end
	response
end

STDOUT.sync = true
ioc = IO.popen("../../bin/Darwin-x86/common put.main","r+")
talk(ioc, /epics>/, "< init")
print "."
talk(ioc, /epics>/, "< p2sec")
print "."
talk(ioc, /epics>/, "< testcache")
print "."
cache_response  = talk(ioc, /^epics>/, "< p0sec")
print "."
if cache_response == testcache_reference then puts " CACHE OK" else puts "FAIL" end
print "."
talk(ioc, /epics>/, "< testnotifysync")
print "."
notifysync_response  = talk(ioc, /^epics>/,"< p2sec")
print "."
if notifysync_response == testnotifysync_reference then puts " NOTIFY SYNC OK" else 
	puts "FAIL"
	puts notifysync_response
	puts "-----------"
	puts testnotifysync_reference
end
talk(ioc, /^epics>/,"< testnotifyasyn")
print "."
notifyasync_response  = talk(ioc, /^epics>/)
print "."
if notifyasync_response == testnotifyasync_reference then puts " NOTIFY ASYNC OK" else 
	puts "FAIL"
	puts notifysync_response
	puts "-----------"
	puts testnotifysync_reference
end

