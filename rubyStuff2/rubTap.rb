module RubTap
	EPICS_HOST_ARCH = ENV['EPICS_HOST_ARCH']
	if EPICS_HOST_ARCH.nil? then
		puts "please set EPICS_HOST_ARCH !!!"
		exit
	end

	require 'plumber'
	require 'debugPrint'

	OK = 0
	NOT_OK = 1

	def RubTap.bail_out(msg='')
			puts "Bail out! " << msg 
			exit
	end

  #
	# TestCase class represents test case and it speaks TAP
	# it can have arbitrary number of tests. Just add methods 
	# starting with test. Method run automatically looks for these 
	# methods and runs them. 
	# global_setup and global_teardoan are invoked once before and after all tests. 
	# setup and teardown are invoked before and after each test. 
	# Test functions should return two values: first is status (RubTap::OK RubTap::NOT_OK) and some comment 
	# Tests are sorted in dict manner. 
	# for func call sequence see run function
	class TestCase
		include DebugPrint
		#
		# The first message printed 
		def header
			puts "1..#{count_tests}"
			puts "# Test pack #{self.class}"
		end

		#
		# The last message printed
		def footer
			puts "# #{self.class}: Finita la comedia"
		end

		def initialize(allTestConfig=nil)
			@config = allTestConfig ? TestConfig::TestConfig.new(allTestConfig.findTestConfig(self.class.name)) : nil
			@debug_level = catch :nil_config_string do @config.findConfig("debug_level") end
			@pwd = Dir.pwd
			@topDir = @config.findConfig("TopDir")
			@binDir = @config.findConfig('BinDir')
			@localBinDir = "#{@topDir}/#{@binDir}/#{EPICS_HOST_ARCH}"
		  @bootDir = @config.findConfig("IocBootDir")
			@localCmd = @config.findConfig("Cmd")
			@localIocBinary = @config.findConfig("Ioc")
			@shellCmd = catch :nil_config_string do @config.findConfig("ShellCmd") end
			debug(@topDir + " " + @binDir + " " + @localBinDir + " " + @bootDir + " " + @localCmd + " " + @localIocBinary, 4)
		end

		def global_setup
		end

		# This function  reads common config 
		# and starts IOC. It is supposed to be called only once. 
		def common_setup
			puts "Common setup deprecated! use common_setup_local instead"
			common_setup_local
		end

		def common_setup_remote
			Dir.chdir(@topDir)
			Dir.chdir(@bootDir)
			remoteHostArch = catch :nil_config_string do @config.findConfig("remoteHostArch") end
			remoteHostArch = remoteHostArch.nil? ? EPICS_HOST_ARCH : remoteHostArch
			remoteTopDir = catch :nil_config_string do @config.findConfig("remoteTopDir") end
			remoteBootDir = catch :nil_config_string do @config.findConfig("remoteBootDir") end
			remoteBootDir = remoteBootDir.nil? ? @bootDir : remoteBootDir
			remoteCmd = catch :nil_config_string do @config.findConfig("remoteCmd") end
			remoteCmd = remoteCmd.nil? ? @localCmd : remoteCmd
			remoteIocBinary = catch :nil_config_string do @config.findConfig("remoteIoc") end
			remoteIocBinary = remoteIocBinary.nil? ? @localIocBinary : remoteIocBinary
			remoteTopDir = remoteTopDir.nil? ? @topDir : remoteTopDir
			remoteBinDir = "#{remoteTopDir}/#{@binDir}/#{remoteHostArch}"
			debug(@remoteTopDir,2)
			debug(@remoteBinDir,2)
			debug(remoteIocBinary,2)
			#if shellcmd is not defined, we bail out
			remoteShellCmd = catch :nil_config_string do @config.findConfig("remoteShellCmd") end
			shellcmd = remoteShellCmd.nil? ? @config.findConfig("ShellCmd") : remoteShellCmd
			startcmd = "#{remoteBinDir}/#{remoteIocBinary} #{remoteCmd}"
			ioc = Plumber::RemoteIoc.new(shellcmd,@debug_level)
			ioc.command("cd #{remoteTopDir}")
			ioc.command("cd #{remoteBootDir}")
			ioc.command("pwd")
			result = ioc.read_all
			debug(result,2)
			ioc.startIoc(startcmd)
			ioc
		end

		def common_setup_vxWorks
			Dir.chdir(@topDir)
			Dir.chdir(@bootDir)
			serialPort = @config.findConfig("serialPort")
			startCmd = "cd \"#{@topDir}/#{@bootDir}\"\n" + @localCmd
			ioc = Plumber::Ioc.new(startCmd,@debug_level,:serial,serialPort)
		end


		def common_setup_named(prefix='')
			Dir.chdir(@topDir)
			Dir.chdir(@bootDir)
			remoteHostArch = catch :nil_config_string do @config.findConfig(prefix + "HostArch") end
			remoteHostArch = remoteHostArch.nil? ? EPICS_HOST_ARCH : remoteHostArch
			remoteTopDir = catch :nil_config_string do @config.findConfig(prefix + "TopDir") end
			remoteTopDir = remoteTopDir.nil? ? @topDir : remoteTopDir
			remoteBootDir = catch :nil_config_string do @config.findConfig(prefix + "BootDir") end
			remoteCmd = catch :nil_config_string do @config.findConfig(prefix + "Cmd") end
			remoteCmd = remoteCmd.nil? ? @localCmd : remoteCmd
			remoteIocBinary = catch :nil_config_string do @config.findConfig(prefix + "Ioc") end
			remoteIocBinary = remoteIocBinary.nil? ? @localIocBinary : remoteIocBinary
			remoteBinDir = "#{remoteTopDir}/#{@binDir}/#{remoteHostArch}"
			debug(remoteTopDir,2)
			debug(remoteBinDir,2)
			debug(remoteIocBinary,2)
			#if shellcmd is not defined, we bail out
			shellCmd = catch :nil_config_string do @config.findConfig(prefix + "ShellCmd") end
			startcmd = "#{remoteBinDir}/#{remoteIocBinary} #{remoteCmd}"
			unless shellCmd.nil? then 
				ioc = Plumber::RemoteIoc.new(shellcmd,@debug_level)
				ioc.command("cd #{remoteTopDir}")
				ioc.command("cd #{remoteBootDir}")
				ioc.command("pwd")
				result = ioc.read_all
				debug(result,2)
				ioc.startIoc(startcmd)
				ioc
			else
				Dir.chdir(remoteTopDir)
				Dir.chdir(remoteBootDir)
				Plumber::Ioc.new(startcmd, @debug_level)
			end
		end

		def common_setup_local
			Dir.chdir(@topDir)
			Dir.chdir(@bootDir)
			@startcmd = "#{@localBinDir}/#{@localIocBinary} #{@localCmd}"
			shellcmd = catch :nil_config_string do @config.findConfig("ShellCmd") end
			if shellcmd.nil? then 
				Plumber::Ioc.new(@startcmd, @debug_level)
			else
				hostArch = catch :nil_config_string do @config.findConfig("HostArch") end
				hostArch = hostArch.nil? ? EPICS_HOST_ARCH : hostArch
				remoteBinDir = "#{@topDir}/#{@binDir}/#{hostArch}"
				startcmd = "#{remoteBinDir}/#{@localIocBinary} #{@localCmd}"
				debug(startcmd,2)
				ioc = Plumber::RemoteIoc.new(shellcmd,@debug_level)
				ioc.command("cd #{@topDir}")
				ioc.command("cd #{@bootDir}")
				ioc.command("pwd")
				result = ioc.read_all
				debug(result,2)
				ioc.startIoc(startcmd)
				ioc
			end
		end

		def common_teardown 
			Dir.chdir(@pwd)
		end
		
		def global_teardown
		end
		
	  def setup
		end

		def teardown
		end

		def find_tests
			self.methods.delete_if { |meth| meth !~ /^test/ }
		end
		
		def count_tests
			find_tests.size
		end

		# this is called before each test
		def local_header
			puts "# #{@current_test}"
		end

		def print_result(result, i)
			case result
			when nil
				puts "not ok #{i} #{@current_test}"
			when Fixnum
				puts result == OK ? "ok #{i} #{@current_test}":"not ok #{i} #{@current_test}"
			when Array
				puts result[0] == OK ? "ok #{i} #{result[1]}":"not ok #{i} #{result[1]}"
			else
				raise "#{result.class} Test #{i} should return Array [result,desc]" 
			end
		end

		def run
			test_methods = find_tests
			if test_methods.size == 0 then 
				RubTap.bail_out "No tests were defined"
			end
			test_methods.sort!
			result_list = Array.new
			i = 0
			header
			global_setup
			test_methods.each do |meth|
				@current_test = meth
				local_header
				setup
				result_list[i] = catch :assertion_faliure do
					self.send meth
				end
				teardown
				print_result(result_list[i], i+1)
				i += 1
			end
			global_teardown
			footer
			return result_list
		end

		#
		# When value == true continue, if false then raise an exception
		def assert(value)
			throw :assertion_faliure unless value
			OK
		end

		def assert_equal(arg1, arg2)
			throw :assertion_faliure unless arg1 == arg2
			OK
		end

	end
	
	class TestSuite
		def initialize
			@case_list = Array.new
			@result_list = Array.new
		end

		def suite_setup
		end

		def suite_teardown
		end

		def add_test_case(test_case)
			@case_list << test_case
		end

		def <<(test_case)
			add_test_case(test_case)
		end

		def run
			self.suite_setup
			i = 0
			@case_list.each do |test_case|
				begin 
					@result_list[i] = test_case.run
				rescue => ex 
					puts ex.instance_variables
					RubTap.bail_out "#{ex.class}: #{ex.message} #{ex.backtrace}"
				end
				i += 1
			end
			self.suite_teardown
		end
	end
end
