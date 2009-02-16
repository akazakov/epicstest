#!/usr/bin/env ruby

require 'testConfig'
require 'rubTap'

class MyTest < RubTap::TestCase
	@description = "Jopa"
	def test_supa
		return RubTap::OK, "Good"
	end
	def test_supa_dupa
		return RubTap::NOT_OK, "Bad # TODO"
	end
	def test_assert
		assert(false)
	end

	def test_assert2
		assert(true)
	end

	def test_assert_equal
		assert_equal(1,2)
	end
	def test_assert_equal2
		assert_equal("abc","abc")
	end
	def test_fail
		assert(false)
	end
end

class MySuite < RubTap::TestSuite
end

config = TestConfig::AllTestsConfig.new("config.yml")	
ts = MySuite.new
tt = MyTest.new
myConf = TestConfig::TestConfig.new(config.findTestConfig(tt.class.name))
puts myConf.findConfig("os")
tz = MyTest.new
ts.add_test_case(tt)
ts << tz
ts.run
tss = MySuite.new(tt)
tss.run
