# = EPICS TEST 
# Requirements to use: 
# 1. Install  EPICS and all the required libraries on ALL target
# computers intended for testing
# 2. Install test packag on all computers 
# 3. Make sure that PATH variable conatins epics/bin and test/bin directories. IMPORTANT! 
# you may need to edit your shell startup scripst or  *~/.ssh/environment* 
# file and make sure that /etc/sshd_conf has +PermitUserEnvironment+ set to +yes+
#
# Hopefully this is all you need to do to use this package.
# Edit config.yml fle to reflect your settings
# and run the tests!!! HOORAY!
#
# Please refer to EPICSTestUtils documentation for more details
# 
# For implementing your own test scenario please refer to the existing files
#
# 
# TODO list -> 
# 1.  Do something with variables visibility from TestCase to Test 
# maybe delete Test class and implement test functions again....
# currently it seems there is not point having separate Test class... 
# but.... I may run the Test.run in the TestCase scope as well
# and of course I can have both !!! 
#
# OK I remebered.... I implemented Test case in order to hide the printing and other stuff 
# to make pluggable Formatter. Insted I can require using provided Formatter API to print. 
# Or --> run threaded ..
#
# 2. make command.start configurable and automatic. 
#
# 3. reimplement setup/teardown for all tests and each 
#			make named setup/teardown ??? 
#
#
#	4. check the startup/cleanup procedures 
#		try to close IOC and commands gracefully. Of course they suppose to
#		exit by themselves when my script exits. Because they'll get broken-pipe
#		but... it's better to be on the safe side and do it myself. 
#	
#
# 5. Multithreading ??? to check the health of the components continiously 
#
# 6. also have to separate "life-cycles" of independent TestCases 
# one test case may "Bail Out" but what is left has to be executed. 
#
#
# 
#
#

require 'rubygems'
require 'net/ssh'
require 'cfg'
# = EPICS TEST AUTOMATION UTILITIES
#
# * TestCase is a container for a test case. It should contain the tests which share same IOCs and Commands. 
# * Test - is a sceleton for a single test 
# * Formatter classes provide different output formats for tests
# * Cfg - hadles configurations 
#
# == Configuration file
# it's YAML.
# All configs are tree-like. Where the top is the name of the test case, then individual configs for IOCs and Commands
# Each config can contain IOC1 to IOC5 
# and COMMAND1 to COMMAND5 
# please see example config file for details.  
#
module EPICSTestUtils
	TEST_RESULTS = [:OK, :NOT_OK, :BAIL_OUT, :TODO, :SKIP]
	IOC_NAMES = ["IOC0", "IOC1", "IOC2", "IOC3", "IOC4", "IOC5"]
	COMMAND_NAMES = ["COMMAND0","COMMAND1","COMMAND2","COMMAND3","COMMAND4","COMMAND5"]
=begin 
	EPICS_HOST_ARCH = ENV['EPICS_HOST_ARCH']
	if EPICS_HOST_ARCH.nil? then
		puts "please set EPICS_HOST_ARCH !!!"
		exit
	end
=end

#assrtion methods throw :assertion_failure in case of "false" 
#and the value returned is set to :NOT_OK
#otherwise they return :OK
def assert(val)
	throw :assertion_failure, :NOT_OK unless val 
	:OK
end
def assert_equal(a,b)
	throw :assertion_failure, :NOT_OK unless a == b 
	:OK
end

module DebugPrint
	attr_accessor :debug_level
	def debug(msg, esc, adr='')
		unless @debug_level.nil? then 
			if @debug_level >= esc then 
				STDERR.puts "<=:DEBUG[" + this_method + "|"  + adr + "]:  "+ msg.to_s + "   :=>"
			end
		end
	end

	def this_method
		caller[1][/`([^']*)'/, 1]
	end
end

#
# This Class is a container for a test case.
# When initialize it requires a formatter object - the output formatter. 
#
# Important vars:
# * @ioc - ioc array used in tests
# * @command - commands array used in tests
# * @object_test - array of Test class objects <- not adviced to use anymore
#
# to add tests you can use '<<' operator 
# 
class TestCase
	include DebugPrint
	attr_accessor :formatter, :ioc, :command, :opt
	attr_reader :options, :title
	def initialize(formatter, options = {})
		@formatter=formatter
		@object_tests=[]
		@ioc=[]
		@command=[]
		@title = self.class.name
		@options = Cfg.c[@title]
		@debug_level = @options['debug_level']
	end

	def << (test)
		@object_tests << test
	end

	def find_tests
			self.methods.delete_if { |meth| meth !~ /^test/ }
	end

	def cmd
		@command
	end

	#
	# Deafult configurator for IOCs
	#
	def setup_iocs
		IOC_NAMES.each do |cur| 
			unless @options.has_key?(cur) then break end
			iocOptions = @options[cur]
			if iocOptions["type"].nil? || iocOptions["type"] == "SH"
				@ioc << IOCLocal.new(iocOptions)
			elsif iocOptions["type"] == "SSH"
				@ioc << IOCSSH.new(iocOptions)
			else
				raise "Unknown connection type"
			end
		end
	end

	# Default configurator for commands
	def setup_commands
		COMMAND_NAMES.each do |cur| 
			unless @options.has_key?(cur) then break end
			options = @options[cur]
			if options["type"].nil? || options["type"] == "SH"
				@command << SH.new(options)
			elsif options["type"] == "SSH"
				@command << SSHCommand.new(options)
			else
				raise "Unknown connection type"
			end
		end
	end

	def start_commands
		#@command.each {|x| x.start}
	end

	def stop_commands
		@command.each { |x| x.exit } 
	end


	def start_iocs
		@ioc.each { |ioc| ioc.start }
	end

	def stop_iocs
		@ioc.each { |ioc| ioc.exit }
	end
	
	# Placeholder for user defined setup function
	# which is called before each test
	# it is called after ioc and commands are initialized, but 
	# before the iocs are started. 
	def setup 
	end

	# Placeholder for user defined setup function
	# which is called after ioc and commands are initialized, but 
	# before the iocs are started. 
	def global_setup
	end

	# Placeholder for user defined teardown function
	# which is called after each test
	def teardown
	end

	# Placeholder for user define global_teardown function 
	# which is called after all tests are completed
	# but before iocs and commands are stoped/killed
	def global_teardown
	end

	# This function is called before everything else
	# User should redefine setup function if needed
	# it configures iocs, commands, runs user setup func 
	# and start iocs
	# for commands start is not called
	def case_setup
		setup_iocs
		setup_commands
		global_setup
		start_iocs
		start_commands
	end


	def case_teardown
		global_teardown
		stop_iocs
		stop_commands
	end

	# It calles all the methods defined inside TestCase that 
	# start with 'test' 
	# and returns results as a hash, "meth_name => result" 
	def run_method_tests
		results_hash = {}
		i = 1
		find_tests.sort.each do |meth|
			setup
			results_hash[meth.name] = catch :assertion_failure do meth.call end 
			teardown
			@formatter.print_test_result(meth_name,results_hash[meth.name],i)
			i += 1
		end
		results_hash
	end

	def run_object_tests
		results_hash = {}
		i = find_tests.size + 1
		@object_tests.each do |test|
			setup
			results_hash[test.title] = catch :assertion_failure do test.run end
			teardown
			@formatter.print_test_result(test.title,results_hash[test.title],i)
			i += 1
		end
	end


	def run
		begin 
			case_setup
			formatter.header(self)
			run_method_tests
			run_object_tests
			formatter.footer(self)
			case_teardown
		rescue => ex
			puts  ex
			puts ex.backtrace
		end 
	end

	def count_test
		@object_tests.size + find_tests.size
	end
end # TestCase
# This class is a template for different output formats:
# Particular formatters should derive from here
# TAP, HTML, TEXT etc. 
class Formatter
	def header(context)
		puts context.title
	end

	def report(context)
		puts context.result
	end

	def put(string)
		puts string
	end

	def print_test_result(name, result, num)
		puts "#{name} #{result} # #{num}"
	end
	def footer(context)
		puts(context.title, "is over")
	end
end

class TAPFormatter < Formatter
	def header(context)
		puts "# #{context.title} starting"
		puts "1..#{context.count_test}"
	end

	def print_test_result(name, result, num = nil)
		if num 
			puts "#{num} #{result} # #{name}"
		else
			puts "#{result} # #{name}"
		end
	end

	def report(context)
		if context.result == :OK then puts "ok - #{context.title} #{context.description} # #{context.explanation}" 
		else puts "not_ok - #{context.title} #{context.description} # #{context.result} #{context.explanation}"
		end
	end
	def put(string)
		puts "# #{string}"
	end
	def footer(context)
		puts "# #{context.title} is over"
	end
end

# Template Class for user tests
# All produced output should go to fromatter.test_output
# Real Test Implementation should redefine the test routine.
#
class Test
	attr_accessor :formatter
	attr_reader	:title, :description, :explanation
	def initialize(test_case)
		@title = self.class.name
		@description = ""
		@explanation = ""
		@formatter = test_case.formatter
		@test_case = test_case
		@result = :TODO
	end
	def run
		@formatter.put("Run:Nothing here")
	end
	def result
		@result
	end
	def ioc
		@test_case.ioc
	end
	def cmd
		@test_case.command
	end
end

# This is a template class for PIPED external program
# output is buffered
# IOC and Command derive from it as well ass SSHCommand and IOCSSH
class ExternalCommand
	include DebugPrint
	DEFAULT_TIMEOUT = 2
	attr_accessor :localBinDir,:cmd, :startcmd, :timeout
	def initialize(options = {})
		topDir = options["topDir"]
		binDir = options["binDir"]
		hostArch = options["epicsHostArch"]
		@localBinDir = "#{topDir}/#{binDir}"
		@cmd = options["cmd"]
		@startcmd = "#{localBinDir}/#{cmd}"
		@startdir = topDir
		@timeout = options["timeout"].nil? ? DEFAULT_TIMEOUT : options["timeout"]
		@done = false
		@buffer = ''
	end
	def start
		if @cmd 
			savedir = Dir.pwd
			Dir.chdir(@startdir)
			@pipe = IO.popen(@startcmd,"r+")
			Dir.chdir(savedir)
			#	sleep(5)
			#read_loop
		end
	end
	def read(timeout = @timeout)
		read_loop(timeout)
		buf = @buffer
		@buffer = ''
		buf
	end
	def command(msg)
		@pipe.puts msg
	end
	def start_timer
		@start_time = Time.now
	end
	def timeout?(val = @timeout)
		(Time.now - @start_time) > val
	end

	def read_loop(timeout = @timeout)
		start_timer
		loop do
			empty = true
			begin 
				@buffer << @pipe.read_nonblock(1024)
				empty = false
			rescue Errno::EAGAIN
				if empty 
					if timeout?(timeout) then break 
					else
						sleep(timeout/3)
						next
					end
				else
					#reset timer and continue
					start_timer
					next
				end
			end
		end
	end

	def exit
		if @pipe then 
			Process.kill("TERM", @pipe.pid)
			@pipe.close
		end
	end

	def kill
		if @pipe then 
			Process.kill("KILL",@pipe.pid)
			@pipe.close
		end
	end
end

#Run command in shell on localhost
class SH < ExternalCommand
	def exec(command)
		@done = false
		result = `#{command}`
		@done = true
		result
	end
end

# 
# Class to wrap-up IOC run on local machine
#
class IOCLocal < ExternalCommand
	DEFAULT_TIMEOUT = 2
	def initialize(options={})
		@options=options
		topDir = options["topDir"]
		binDir = options["binDir"]
		bootDir = options["bootDir"]
		hostArch = options["epicsHostArch"]
		cmd = options["cmd"]
		ioc = options["ioc"]
		@localBinDir = "#{topDir}/#{binDir}"
		@startdir = "#{topDir}/#{bootDir}"
		@startcmd = "#{topDir}/#{binDir}/#{hostArch}/#{ioc} #{cmd}"
		@buffer = ''
		@timeout = options["timeout"].nil? ? DEFAULT_TIMEOUT : options["timeout"]
	end
	def start
		savedir = Dir.pwd
		Dir.chdir(@startdir)
		@pipe = IO.popen(@startcmd,"r+")
		Dir.chdir(savedir)
		sleep(5)
		read_loop
	end
end

class SSHCommand <  ExternalCommand
	include DebugPrint
	DEFAULT_TIMEOUT = 2
	attr_accessor :buffer, :err_buffer, :done
	def initialize(options={})
		topDir = options["topDir"]
		binDir = options["binDir"]
		hostArch = options["epicsHostArch"]
		@localBinDir = "#{topDir}/#{binDir}"
		@done = false
		@debug_level = options["debug_level"]
		cmd = options["cmd"]
		@startdir = topDir
		@startcmd = "#{topDir}/#{binDir}/#{cmd}"
		@timeout = options["timeout"].nil? ? DEFAULT_TIMEOUT : options["timeout"]
		@session = Net::SSH.start(options["hostname"], options["user"])
		@buffer = ''
		@err_buffer = ''
		@done = false
	end

	def start
		ssh = @session
		@channel = ssh.open_channel do |ch| 
			ch.exec "cd #{@startdir} && #{@startcmd}" do |ch, success| 
				abort "could not execute command" unless success
				ch.on_data do |chn, data|
					debug("got stdout: #{data}", 2)
					#ch.send_data "something for stdin\n"
					@buffer << data
				end
				ch.on_extended_data do |chn, type, data|
					debug("got stderr: #{data}", 2)
					@err_buffer << data
				end
				ch.on_close do |ch|
					debug("channel is closing!",2)
					#	unless @done then raise "Command exited unexpectedly" end
				end
			end
		end
		read_loop(5)
	end

	def read_loop(timeout = @timeout)
		start_timer
		@session.loop(timeout) { not timeout?(timeout) }
	end

	def command(cmd)
		@channel.send_data(cmd)
	end

	def exec(command)
		@session.exec!(command).to_s
	end
	def close
		@done = true
		@session.close
	end
	alias exit close
	alias kill close
end

class IOCSSH < SSHCommand
	include DebugPrint
	DEFAULT_TIMEOUT = 2
	attr_accessor :buffer, :err_buffer, :done
	def initialize(options={})
		@options = options
		@debug_level = options["debug_level"]
		remoteHostname = options["hostname"]
		remoteHostArch = options["epicsHostArch"]
		topDir = options["topDir"]
		binDir = options["binDir"]
		bootDir = options["bootDir"]
		hostArch = options["epicsHostArch"]
		cmd = options["cmd"]
		ioc = options["ioc"]
		@localBinDir = "#{topDir}/#{binDir}"
		@startdir = "#{topDir}/#{bootDir}"
		@startcmd = "#{topDir}/#{binDir}/#{hostArch}/#{ioc} #{cmd}"
		@buffer = ''
		@timeout = options["timeout"].nil? ? DEFAULT_TIMEOUT : options["timeout"]
		@session = Net::SSH.start(remoteHostname, options["user"])
		@buffer = ''
		@err_buffer = ''
		@done = false
	end
end
end

