require 'EPICSTestUtils'

assert(true)
assert(1==1)
assert do 
	true
end
assert do 
	1 == 1 
end

a = [1,1,1,1,0]
a.each do |i| 
	p = Proc.new do
		if i == 1 then 
			puts "#{i}"
			next true
		end
	end
	puts p.call
end
