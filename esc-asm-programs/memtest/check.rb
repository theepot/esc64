mem = []

File.open("ramdump.lst") do |f|
	f.each_line do |line|
		mem << line.to_i(2)
	end
end

for i in 15...0x8000
	x = i ^ 0xFFFF
	if x != mem[i]
		puts "i=#{i}, x=#{x}, mem[i]=#{mem[i]}"
	end
end

