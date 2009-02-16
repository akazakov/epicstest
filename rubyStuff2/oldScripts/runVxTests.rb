#!/usr/bin/env ruby

require 'testConfig'
require 'alarmTest'
require 'caTest'
require 'putTest'
require 'softcallbackTest'

config = TestConfig::AllTestsConfig.new("config.yml")
tsi = RubTap::TestSuite.new
#tsi << TestCA.new(config)
tsi << TestSoftCallbackVxWorks.new(config)
tsi << TestPutVxWorks.new(config)
tsi << TestAlarmVxWorks.new(config)
tsi.run
