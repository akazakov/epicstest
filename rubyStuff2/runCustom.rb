#!/usr/bin/env ruby

require 'testConfig'
require 'alarmTest'
require 'caTest'
require 'linkinfoTest'
require 'putTest'
require 'softcallbackTest'
require 'softcallbackTestRemote'

config = TestConfig::AllTestsConfig.new("config.yml")	
tsi = RubTap::TestSuite.new
#tsi << TestAlarm.new(config)
#tsi << TestCA.new(config)
#tsi << TestPut.new(config)
#tsi << TestLinkInfoLocal.new(config)
#tsi << TestLinkInfoRemote.new(config)
#tsi << TestAlarm.new(config)
#tsi << TestSoftCallback.new(config)
#tsi << TestSoftCallbackRemote.new(config)
#tsi << TestSoftCallbackVxWorks.new(config)
#tsi << TestPutVxWorks.new(config)
tsi << TestAlarmVxWorks.new(config)
tsi.run
