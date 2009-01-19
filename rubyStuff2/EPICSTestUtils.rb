# = EPICS TEST 
# Requirements to use: 
# 1. Install  EPICS and all the required libraries on ALL target computers intended for testing
# 2. Install test packag on all computers 
# 3. Make sure that PATH variable conatins epics/bin and test/bin directories. IMPORTANT! 
# you may need to edit your shell startup scripst or  *~/.ssh/environment* file and make sure that /etc/sshd_conf has +PermitUserEnvironment+ set to +yes+
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

require 'rubygems'
require 'net/ssh'
require 'yaml'
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
	# * @test - test array 
	#
	# to add tests you can use '<<' operator 
	# 
	class TestCase
		include DebugPrint
		attr_accessor :formatter, :ioc, :command
		attr_reader :options, :global_options, :title
		def initialize(formatter, options = {})
			@formatter=formatter
			@test=[]
			@ioc=[]
			@command=[]
			@title = self.class.name
			@global_options = options
			@options = Cfg.find(options,@title)
			@debug_level = Cfg.find(@options, 'debug_level')
		end

		def << (test)
			@test << test
		end

		#
		# Deafult configurator for IOCs
		#
		def setup_iocs
			IOC_NAMES.each do |cur| 
				unless @options.has_key?(cur) then break end
				options = @options[cur]
				if options["type"].nil? || options["type"] == "SH"
					@ioc << IOCLocal.new(options)
				elsif options["type"] == "SSH"
					@ioc << IOCSSH.new(options)
				else
					raise "Unknown connection type"
				end
			end
		end

		#
		# Default configurator for commands
		def setup_commands
			COMMAND_NAMES.each do |cur| 
				debug(@options,3)
				unless @options.has_key?(cur) then break end
				options = @options[cur]
				debug("#{options}",3)
				if options["type"].nil? || options["type"] == "SH"
					@command << SH.new(options)
				elsif options["type"] == "SSH"
					@command << SSH.new(options)
				else
					raise "Unknown connection type"
				end
			end
		end

		def start_iocs
			@ioc.each { |ioc| ioc.start }
		end

		def stop_iocs
			@ioc.each { |ioc| ioc.stop }
		end

		def setup
			setup_iocs
			setup_commands
			start_iocs
		end

		def teardown
			stop_iocs
		end

		def run
			setup
			formatter.header(self)
			@test.each do |test|
				test.run
				formatter.report(test)
			end
			formatter.footer(self)
			teardown
		end

		def test_count
			@test.size
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

		def footer(context)
			puts(context.title, "is over")
		end
	end

	class TAPFormatter < Formatter
		def header(context)
			puts "# #{context.title} starting"
			puts "1..#{context.test_count}"
		end
		def report(context)
			if context.result == :OK then puts "ok - #{context.description} # #{context.explanation}" 
			else puts "not_ok - #{context.description} # #{context.result} #{context.explanation}"
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
		@@title = "Test Title"
		@@description = "Test Description"
		@@explanation = "Test is not implemented yet"
		attr_accessor :formatter
		def initialize(formatter,test_case)
			@formatter = formatter
					@test_case = test_case
			@result = :TODO
		end
		def title
			@@title
		end
		def description
			@@description
		end
		def explanation
			@@explanation
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

	# This is a template class
	# IOC and SimpleCommand derive from it
	class ExternalCommand
		attr_accessor :connector
		def initialize(connector)
			@connector = connector
		end
		def start
			@connector.connect
		end
		def stop
			@connector.disconnect
		end
		def read
			@connector.read
		end
		def write(message)
			@connector.write(message)
		end
		def read_line
			@connector.read_line
		end
	end

	# Template to represent different types of connections:
	class Connector
		def connect
		end

		def disconnect
		end

		def read
		end

		def write(message)
		end

		def readlines
		end
	end

	# Run command in shell
	class SH 
		include DebugPrint
		def initialize(options = {})
		end
		def exec(command)
			`#{command}`
		end
	end

	# Run command over ssh
	class SSH
		def initialize(options = {})
			@connection = Net::SSH.start(options["host"], options["user"])
		end
		def exec(command)
			@connection.exec!(command).to_s
		end
		def close
			@connection.close
		end
	end

	class IOC
		def start
		end
		def stop
		end
		def read
		end
		def command
		end
	end

	# 
	# Config handling Stuff
	#
	module Cfg
		def Cfg.load(filename)
			f = File.open(filename,"r")
			config = YAML::load(f)
			raise "Config file is empty or incorrect format!" unless config
			if config[:default].nil? or config[:hosts].nil? then 
				raise "Config file should contain :default and :hosts section (and optionally others))"
			end
			f.close
			config
		end

		def Cfg.find(options,key)
			if options[key].nil? 
				raise "#{key} undefined!" 
			end
			options[key]
		end
	end

	# 
	# Class to wrap-up IOC run on local machine
	#
	class IOCLocal < IOC
		DEFAULT_TIMEOUT = 2
		def initialize(options={})
			topDir = Cfg.find(options,"topDir")
			binDir = Cfg.find(options,"binDir")
			bootDir = Cfg.find(options,"bootDir")
			hostArch = options["hostArch"].nil? ? EPICS_HOST_ARCH : options["hostArch"]
			cmd = Cfg.find(options,"cmd")
			ioc = Cfg.find(options,"ioc")
			@localBinDir = "#{topDir}/#{binDir}"
			@startdir = "#{topDir}/#{bootDir}"
			@startcmd = "#{topDir}/#{binDir}/#{hostArch}/#{ioc} #{cmd}"
			@buffer = ''
			@timeout = options[:timeout].nil? ? DEFAULT_TIMEOUT : options[:timeout]
		end
		def start
			savedir = Dir.pwd
			Dir.chdir(@startdir)
			@pipe = IO.popen(@startcmd,"r+")
			Dir.chdir(savedir)
			sleep(5)
			read_loop
		end
		def read
			read_loop
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

	end

	class IOCSSH < IOC
		include DebugPrint
		DEFAULT_TIMEOUT = 2
		attr_accessor :buffer, :err_buffer
		def initialize(options={})
			topDir = Cfg.find(options,"topDir")
			binDir = Cfg.find(options,"binDir")
			bootDir = Cfg.find(options,"bootDir")
			hostArch = options["hostArch"].nil? ? EPICS_HOST_ARCH : options["hostArch"]
			cmd = Cfg.find(options,"cmd")
			ioc = Cfg.find(options,"ioc")
			@localBinDir = "#{topDir}/#{binDir}"
			@startdir = "#{topDir}/#{bootDir}"
			@startcmd = "#{topDir}/#{binDir}/#{hostArch}/#{ioc} #{cmd}"
			@buffer = ''
			@timeout = options[:timeout].nil? ? DEFAULT_TIMEOUT : options[:timeout]
			@session = Net::SSH.start(options["host"], options["user"])
			@buffer = ''
			@err_buffer = ''
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
					end
				end
			end
			read_loop(5)
		end

		def start_timer
			@start_time = Time.now
		end

		def timeout?(val = @timeout)
			(Time.now - @start_time) > val
		end

		def read_loop(timeout = @timeout)
			start_timer
			@session.loop(timeout) { not timeout?(timeout) }
		end

		def command(cmd)
			@channel.send_data(cmd)
		end

		def exec(command)
		end

		def close
			@session.close
		end
	end
end
