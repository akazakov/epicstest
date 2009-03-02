# = EPICS TEST 
# !!!!! IMPORTANT !!!!!!
# !!! caRepeater has to be running on all the machines involved in this tests 
# otherwise you may experience zombie invasion of ca-lib using programs
# !!! You've been WARNED !!!
#
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
# DONE list ->
# 
# 2. make command.start configurable and automatic. 
# 3. reimplement setup/teardown for all tests and each 
#			make named setup/teardown ??? 
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

require 'rubygems'
require 'net/ssh'
require 'ruby-debug'
require 'cfg'

#assrtion methods throw :assertion_failure in case of "false" 
#and the value returned is set to :NOT_OK
#otherwise they return :OK
def assert(val = true, &block)
	throw :assertion_failure, :NOT_OK unless val 
	unless block.nil? 
		throw :assertion_failure, :NOT_OK unless block.call
	end
	:OK
end
def assert_equal(a,b)
	throw :assertion_failure, :NOT_OK unless a == b 
	:OK
end


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

module DebugPrint
	attr_accessor :debug_level
	def debug(msg, esc, adr='')
		unless @debug_level.nil? then 
			if @debug_level >= esc then 
				out_str = "DEBUG#{esc}|" + this_method + "|"  + adr + "|:"
				unless msg.to_s.empty? 
				 	out_str += " #{msg.to_s}" 
				end
				STDERR.puts  out_str
			end
		end
	end

	def this_method
		caller[1][/`([^']*)'/, 1]
	end
end

module Open3
  # 
	# This is just a little changed version of standard Open3 module. 
	# it returns pid along with pipes
  # Open stdin, stdout, and stderr streams and start external executable.
  # Non-block form:
  #   
  #   require 'open3'
  #
  #   [stdin, stdout, stderr] = Open3.popen3(cmd)
  #
  # Block form:
  #
  #   require 'open3'
  #
  #   Open3.popen3(cmd) { |stdin, stdout, stderr| ... }
  #
  # The parameter +cmd+ is passed directly to Kernel#exec.
  #
  def popen3(*cmd)
    pw = IO::pipe   # pipe[0] for read, pipe[1] for write
    pr = IO::pipe
    pe = IO::pipe
		rd, wr = IO::pipe

    pid = fork{
      # child
			rd.close
      chpid = fork{
	# grandchild
	pw[1].close
	STDIN.reopen(pw[0])
	pw[0].close

	pr[0].close
	STDOUT.reopen(pr[1])
	pr[1].close

	pe[0].close
	STDERR.reopen(pe[1])
	pe[1].close

	exec(*cmd)
      }
			wr.puts chpid
      exit!(0)
    }
		wr.close
		chpid = rd.gets.to_i
		rd.close
    pw[0].close
    pr[1].close
    pe[1].close
    Process.waitpid(pid)
    pi = [pw[1], pr[0], pe[0]]
		pi.each do |pp|
			def pp.setpid(spid) 
			@pid = spid
			end
			def pp.pid
				@pid
			end
			pp.setpid(chpid)
		end
    pw[1].sync = true
    if defined? yield
      begin
	return yield(*pi)
      ensure
	pi.each{|p| p.close unless p.closed?}
      end
    end
    pi
  end
  module_function :popen3
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
		raise "Config file has no section for #{@title}" if @options.nil? 
		@debug_level = @options['debug_level']
		@name = @title
		debug("",2,@name)
	end

	def << (test)
		@object_tests << test
	end

	def find_tests
		debug("",5,@name)
			self.methods.delete_if { |meth| meth !~ /^test/ }
	end

	def cmd
		@command
	end

	#
	# Deafult configurator for IOCs
	#
	def setup_iocs
		debug("",5,@name)
		IOC_NAMES.each do |cur| 
			unless @options.has_key?(cur) then break end
			iocOptions = @options[cur]
=begin
			if iocOptions["type"].nil? || iocOptions["type"] == "SH"
				@ioc << IOCLocal.new(iocOptions)
			elsif iocOptions["type"] == "SSH"
				@ioc << IOCSSH.new(iocOptions)
			else
				raise "Unknown connection type"
			end
=end
			if iocOptions.host == "localhost"
				@ioc << IOCLocal.new(iocOptions)
			else 
				@ioc << IOCSSH.new(iocOptions)
			end
		end
	end

	# Default configurator for commands
	def setup_commands
		debug("",5,@name)
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
		debug("",5,@name)
		@command.each {|x| x.autostart}
	end

	def stop_commands
		debug("",5,@name)
		@command.each { |x| x.autoexit } 
	end


	def start_iocs
		debug("",5,@name)
		@ioc.each { |ioc| ioc.autostart }
	end

	def stop_iocs
		debug("",5,@name)
		@ioc.each { |ioc| ioc.autoexit }
	end
	
	# Placeholder for user defined setup function
	# which is called before each test
	# it is called after ioc and commands are initialized, but 
	# before the iocs are started. 
	def setup 
	end

	# Placeholder for user defined setup function
	# which is called after ioc and commands are initialized, but 
	# before the iocs are started. Therefore IOCs and Commands cannot accept 
	# control commands yet. You can only set global variables change config and do 
	# other stuf which has to be done before the IOCs and COMMANDS are started. 
	def global_setup
	end

	# this function is called after IOC and Commands are started. 
	# thus you can redefine this function and issue some commands to IOC
	def	global_setup_after_start
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
		debug("",3,@name)
		setup_iocs
		setup_commands
		global_setup
		start_iocs
		start_commands
		global_setup_after_start
	end


	def case_teardown
		debug("",3,@name)
		global_teardown
		stop_iocs
		stop_commands
	end

	# It calls all the methods defined inside TestCase that 
	# start with 'test' 
	# and returns results as a hash, "meth_name => result" 
	def run_method_tests
		debug("",5,@name)
		results_hash = {}
		i = 1
		find_tests.sort.each do |meth|
			setup
			results_hash[meth] = catch :assertion_failure do self.send meth end 
			teardown
			@formatter.print_test_result(meth,results_hash[meth],i)
			i += 1
		end
		results_hash
	end

	# Calls tests implemented as Objects of Class Test
	# _Deprecated_
	def run_object_tests
		debug("",5,@name)
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

	# main test case exeution sequence
	def run
		debug("",3,@name)
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
			# kill all in case something is left behind
			@command.each { |x| x.kill_all }
			@ioc.each { |x| x.kill_all }
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
		@pp = []
		@options = options
		@pids = []
		@buffer = ''
		@err_buffer = ''
		@topDir = options["topDir"]
		@binDir = options["binDir"]
		@bootDir = options["bootDir"]
		@ioc = options["ioc"]
		@cmd = options["cmd"]
		@hostArch = options["epicsHostArch"]
		@hostname = options["hostname"]
		@localBinDir = "#{@topDir}/#{@binDir}/#{@hostArch}"
		@startdir = @topDir
		@timeout = options["timeout"].nil? ? DEFAULT_TIMEOUT : options["timeout"]
		@done = false
		@debug_level = options["debug_level"]
		if @cmd 
			@startcmd = "#{@localBinDir}/#{@cmd}"
		else 
			@startcmd = nil
		end
		@name = "#{self.class.name}:#{@cmd}@#{@hostname}"
		@echo = @prompt = nil
		debug("",5,@name)
	end

	def check_terminal
		@echo = false
		read(1)
		command("pwd")
		response = read(1).to_a
		if response[0] =~ /^ *pwd/ then @echo = true end
		@prompt = response[-1]
		debug("echo = #{@echo} prompt = #{@prompt}", 2 , @name)
	end


	# This function is called from TestCase.setup_commands and TestCase.setup_iocs 
	# if autostart is defined in corresponding section of config file then 
	# "start" is called, if autostart is not defined of equal "no" then it is not called, 
	# and user has to call "start" manually. I.e. if you need to stop/start commands/iocs between
	# tests, then this feature is useful. caTest.rb uses this functionality. 
	def autostart
		debug("autostart= #{@options['autostart']}",3,@name)
		if @options["autostart"] 
			self.start
		end
	end

	# same as autostart but for exit 
	def autoexit
		debug("autostart= #{@options['autostart']}",3,@name)
		if @options["autostart"] 
			self.exit
		end
	end

	# start the program
	def start
		debug("",2,@name)
		if @startcmd
			savedir = Dir.pwd
			Dir.chdir(@startdir)
			@pp = Open3.popen3(@startcmd)
			debug("pid #{@pp[1].pid}", 2, @name)
			debug("got pipe #{@pp}", 4, @name)
			@pids << @pp[1].pid
			Dir.chdir(savedir)
		end
	end
	def read_stderr(timeout = @timeout)
		debug("",2,@name)
		read_loop(timeout, @pp[2], @err_buffer)
		buf = @err_buffer
		@err_buffer = ''
		buf
	end

	def start_timer
		debug("",14,@name)
		@start_time = Time.now
	end
	
	def timeout?(val = @timeout)
		debug("",11,@name)
		(Time.now - @start_time) > val
	end

	def read_loop(timeout = @timeout, io = @pp[1], buf = @buffer)
		debug("",5,@name)
		raise "Pipe is nil" if io.nil?
		raise "Pipe is closed!" if io.closed? 
		start_timer
		loop do
			empty = true
			begin 
				buf << io.read_nonblock(1024)
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

	def read(timeout = @timeout)
		debug("",2,@name)
		read_loop(timeout, @pp[1], @buffer)
		buf = @buffer
		@buffer = ''
		buf
	end
	alias :read_stdin :read

	def command(msg, timeout = 0)
		debug(msg,2,@name)
		@pp[0].puts msg
		if timeout 
			sleep timeout
		end
	end

	def command_response(cmd, timeout = @timeout)
		read
		command(cmd)
		resp = read(timeout)
		resp_lines = resp.to_a
		debug(">>> #{cmd}\n #{resp} \n ===", 8)	
		debug("#{resp_lines} \n ===", 8)	
		if @echo
			resp_lines.delete_at(0)
			debug("first line deleted", 8)	
		end
		if resp_lines[-1] == @prompt
			resp_lines.delete_at(-1)
		end
		debug("#{resp_lines} \n ===", 8)	
		resp_lines.join
	end

	alias talk command_response 


	def close_pipes
		debug("",3,@name)
		@pp.each { |pp| pp.close unless pp.closed? }
		@pp = []
	end

	def exit
		debug("",2,@name)
		unless @pp.empty? then 
			Process.kill("TERM", @pp[2].pid)
			close_pipes
		end
	end
	
	def suspend
		debug("",2,@name)
		unless @pp.empty? then 
			Process.kill("STOP", @pp[2].pid)
		end
	end
	
	def continue
		debug("",2,@name)
		unless @pp.empty? then 
			Process.kill("CONT", @pp[2].pid)
		end
	end

	def kill
		debug("",2,@name)
		unless @pp.empty? then 
			Process.kill("KILL",@pp[2].pid)
			close_pipes
		end
	end

	def kill_all
		debug("",2,@name)
		@pids.each do |pid| 
			begin
				Process.kill("TERM",pid)
			rescue Errno::ESRCH
				next
			end
		end
		sleep(2)
		@pids.each do |pid| 
			begin
				Process.kill("KILL",pid)
			rescue Errno::ESRCH
				next
			end
		end
	end
end

#Run command in shell on localhost
class SH < ExternalCommand
	def exec(command)
		debug(command,2,@name)
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
		super
		@startdir = "#{@topDir}/#{@bootDir}"
		@startcmd = "#{@topDir}/#{@binDir}/#{@hostArch}/#{@ioc} #{@cmd}"
	end
	def start
		debug("",2,@name)
		savedir = Dir.pwd
		Dir.chdir(@startdir)
		@pp = Open3.popen3(@startcmd)
		@pids << @pp[2].pid
		Dir.chdir(savedir)
		sleep(5)
		read
		read_loop(@timeout, @pp[2],@err_buffer)
		check_terminal
	end
end

# Shell command on remote machine via SSH
class SSHCommand <  ExternalCommand
	include DebugPrint
	DEFAULT_TIMEOUT = 2
	attr_accessor :buffer, :err_buffer, :done
	attr_reader :echo
	def initialize(options={})
		super
		@startcmd = "#{@localBinDir}/#{cmd}"
		@session = Net::SSH.start(options["hostname"], options["user"])
	end

	def start
		debug("",2,@name)
		ssh = @session
		@channel = ssh.open_channel do |ch| 
			ch.exec "cd #{@startdir} && #{@startcmd}" do |ch, success| 
				abort "could not execute command" unless success
				ch.on_data do |chn, data|
					debug("got stdout:\n #{data}", 4)
					#ch.send_data "something for stdin\n"
					@buffer << data
				end
				ch.on_extended_data do |chn, type, data|
					debug("got stderr: #{data}", 4)
					@err_buffer << data
				end
				ch.on_close do |ch|
					debug("channel is closing!",2, @name)
					#	unless @done then raise "Command exited unexpectedly" end
				end
			end
		end
		read_loop(5)
		check_terminal
	end

	def read_loop(timeout = @timeout, io = nil, buf = nil )
		debug("",5,@name)
		start_timer
		@session.loop(timeout) { not timeout?(timeout) }
	end

	def command(cmd)
		debug(cmd,2,@name)
		if cmd[-2..-1] != "\n" 
			cmd += "\n"
		end
		@channel.send_data(cmd)
		@channel.process
	end

	def exec(command)
		debug("",2,@name)
		@session.exec!(command).to_s
	end
	def close
		debug("",2,@name)
		@done = true
		@session.close
	end
	alias exit close
	alias kill close
end

# IOC on remote machine via SSH
class IOCSSH < SSHCommand
	include DebugPrint
	DEFAULT_TIMEOUT = 2
	attr_accessor :buffer, :err_buffer, :done
	def initialize(options={})
		super
		@startdir = "#{@topDir}/#{@bootDir}"
		@startcmd = "#{@localBinDir}/#{@ioc} #{@cmd}"
		@session = Net::SSH.start(@hostname, options["user"])
		debug("opened connection to #{@hostname}: #{@session}", 2)
	end
end
end

