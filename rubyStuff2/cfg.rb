require 'yaml'
module Cfg
	class Config < Hash
		attr_accessor :top
		def Config.new(top = nil, options = nil)
			if options 
				c = Config.[](options)
			else 
				c = Config.[]
			end
			c.top = top
			c
		end

		def [](key)
			val = super
			if val.nil? && !@top.nil?
				val = @top[key]
			end
			val
		end
		def find(name)
			self.[](name)
		end

		def method_missing(method_id, *arg, &block)
			val = self.[](method_id.to_s)
			if val
				val
			else
				super
			end
		end
	end

	@@default, @@h, @@c = nil, nil, nil
	def Cfg.default
		@@default
	end
	def Cfg.h
		@@h
	end
	def Cfg.c
		@@c
	end

	def Cfg.load(filename)
		f = File.open(filename,"r")
		config = YAML::load(f)
		raise "Config file is empty or incorrect format!" unless config
		if config[:default].nil? or config[:hosts].nil? then 
			raise "Config file should contain :default and :hosts section (and optionally others))"
		end
		@@defaults = Config.new(nil, config[:default])
		hosts = Config.new
		config[:hosts].each_pair do |k,v|
			v["alias"] = k
			hosts[k] = Config.new(@@defaults, v)
		end
		@@h = hosts
		
		testCases = Config.new(@@defaults)
		config[:testCases].each_pair do |testCaseName,testCaseHash|
			caseConfig = Config.new(@@defaults)
			testCaseHash.each_pair do |key,val|
				if (EPICSTestUtils::IOC_NAMES.member?(key) || EPICSTestUtils::COMMAND_NAMES.member?(key)) && val.class == Hash 
					if val["host"].nil? 
						val["host"] = "localhost" 
					end
					iocHost = val["host"]	
					hostCfg = hosts[iocHost]
					raise "Host #{iocHost} undefined, referenced from #{name}.#{key}" if hostCfg.nil?
					caseConfig[key] = Config.new(hostCfg,val)
				else 
					caseConfig[key] = val
				end
			end
			testCases[testCaseName] = caseConfig
		end
		@@c = testCases
		f.close
		return @@defaults, @@h, @@c
	end
end

