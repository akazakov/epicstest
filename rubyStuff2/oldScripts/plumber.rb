require 'debugPrint'
module Plumber
	#current level has to be >= escape_level to print
	#debug levels 0 1 2. ... 
	class GenericCommand
		include DebugPrint
		def initialize(command, shellcmd = nil)
				STDOUT.sync = true
				if shellcmd.nil?  then
					@pipe = IO.popen(command,"r+")
				else 
					@pipe = IO.popen(shellcmd + " " + command, "r+")
				end
				@pipe.sync = true
				@debug_level = 0
		end

		def pipe
			@pipe
		end

		# read everything from the stream. 
		# at the end sleep for timer and make sure that stream is still empty
		# by default sleep 2 sec
		# also sleep 0.1 at a time
		def read_all(timer = 1)
			response = ''
			buf = ''
			really_empty = false
			start_time = Time.now
			loop do
				begin
					response << @pipe.read_nonblock(1024)
					debug(response,5)
					really_empty = false
				rescue Errno::EAGAIN
					if really_empty then
						if (Time.now - start_time) > timer	then 
							break 
						else 
							sleep(0.1)
							next
						end
					else
						#come here only first time
						really_empty = true
						sleep(0.1)
						next
					end
				rescue EOFError
					break
				end
			end
			debug(response,4)
			response
		end

		def read_all_lines(arg = 1)
			read_all(arg).to_a
		end

		def command(str)
			@pipe.puts str
		end

		def exit
			Process.kill("TERM", @pipe.pid)
			@pipe.close
		end

		def kill
			Process.kill("KILL",@pipe.pid)
			@pipe.close
		end

		def suspend 
			Process.kill("STOP",@pipe.pid)
		end

		def resume
			Process.kill("CONT",@pipe.pid)
		end

	end
	class Ioc < GenericCommand
		#MEMO: when we run on iocsh with readline we do not care a lot about syncronization between reads and writes
		# but on serial line, we have to wait until the command is finished on the other end 
		# and only then issue the next one.
		def initialize(command, debug_level = 0, connection_type = :shell, port = "/dev/ttyS0") 
			@debug_level = debug_level
			case connection_type
			when :shell	
				STDOUT.sync = true
				@pipe = IO.popen(command,"r+")
				@pipe.sync = true
				sleep(5)
				check_echo
				check_terminal
				@vxWorks = false
			when :serial
				@vxWorks = true
				#@pipe = IO.popen("/home/phoebus/KAZAKOV/tmp/usr/bin/cu -l #{port}", "r+")
				@pipe = File.open("#{port}", "r+")
				command.each_line do |line|
					@pipe.puts line
					sleep(1)
				end
				sleep(5)
				check_echo
				check_terminal
			else
				raise "Unknow connection type"
			end #case ends
		end

		def read_all_lines(arg = 1)
			read_all(arg).to_a
		end

		def check_terminal
			read_all(3)
			command("")
			response = read_all_lines(3)
			debug(response,3,"invitation: #{__FILE__} #{__LINE__}")
			@invitation = response[-1]
		end

		def check_echo
			@command_read_back = false
			#make sure that the stream is empty
			#and then send pwd and read it back
			read_all(3)
			command("pwd")
			response = read_all_lines
			if response[0] =~ /^ *pwd/ then @command_read_back = true end
			debug(response,3,"echo: #{__FILE__} #{__LINE__}")
		end

		#put a command and get a response
		def talk_lines(command = nil, timeout = 1)
			line = ''
			response = ''
					if command then @pipe.puts command end
			resp_array = read_all_lines(timeout)
			debug(resp_array,3,"#{__FILE__} #{__LINE__}")
			if resp_array[-1] == @invitation then resp_array.delete_at(-1) end
			debug(resp_array,3,"#{__FILE__} #{__LINE__}")
			if @command_read_back then 
				#remove first string
				resp_array.delete_at(0)
			end
			debug(resp_array,3,"#{__FILE__} #{__LINE__}")
			resp_array
		end

		#returns a string
		def talk(command = nil, timeout  = 1) 
			response =  talk_lines(command, timeout)
			response.to_s
		end
	end

	class RemoteIoc < Ioc
		def initialize(cmd, debug_level = 0) 
			@debug_level = debug_level
			STDOUT.sync = true
			@pipe = IO.popen(cmd,"r+")
			@pipe.sync = true
			@vxWorks = false
		end
		def startIoc(cmd)
			debug(read_all(1),2)
			command(cmd)
			debug(read_all(1),2)
			check_echo
			check_terminal
		end
	end
end
