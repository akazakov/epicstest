module DebugPrint
	attr_accessor :debug_level
	def debug(msg, esc, adr='')
		unless @debug_level.nil? then 
			if @debug_level >= esc then 
				puts "# ~~~~~~~~     " + this_method + "   "  + adr
				puts msg
			end
		end
	end

	def this_method
		   caller[1][/`([^']*)'/, 1]
	end
end
