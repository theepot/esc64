#!/usr/bin/ruby

require 'set'

crtprefix = ARGV[0]

crtmap =
{
#	"__mul16"	=> "esccrt-mul16.o",
#	"__sdiv16"	=> "esccrt-sdiv16.o"
}

objpaths = Set.new
syms = Set.new

for i in 1...ARGV.count
	obj = ARGV[i]
	if objpaths.add?(obj) == nil
		next
	end
	syms.merge(`esc-objdump -u #{obj}`.split.uniq)
end

for s in syms
	o = crtmap.fetch(s, nil)
	if o != nil
		objpaths.add("#{crtprefix}/#{o}")
	end
end

for o in objpaths
	print " #{o}"
	$stderr.puts "genobjpaths obj: #{o}"
end
