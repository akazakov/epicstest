require 'EPICSTestUtils'

con = EPICSTestUtils::SH.new
puts con.exec('ls')

puts '========='
con = EPICSTestUtils::SSH.new({'host'=>'burdock.linac.kek.jp','user'=>'tyoma'})
puts con.exec('caget mrkai')
