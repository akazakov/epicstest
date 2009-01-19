require 'yaml'
module TestConfig
	class TestConfig
		def initialize(hash)
			@config = hash
		end
		def findConfig(key)
			if @config[key] then @config[key] 
			else throw :nil_config_string end
		end
	end
	class AllTestsConfig
		def initialize(filename)
			f = File.open(filename,"r")
			@config = YAML::load(f)
			if @config[:default].nil? then 
				raise "Config file should contain :default section (and optionally others))"
			end
			f.close
		end

		def findTestConfig(test_name)
				if @config[test_name.to_sym] then return @config[test_name.to_sym]  
				else  return @config[:default] 
				end
		end
	end
end
