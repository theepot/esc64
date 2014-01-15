require 'interpreter.rb'

module Descriptions
	include ESC64AsmDesc

	class Func < Instruction
		def initialize name, opcode
			super name, opcode, false, "#{name} rd:reg, ra:reg, rb:reg", { "rd" => 0, "ra" => 1, "rb" => 2 }, [ 0, 0, 0 ]
		end
	end
	
	class Step < Instruction
		def initialize name, opcode
			super name, opcode, false, "#{name} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ]
		end
	end
	
	class MoveReg < Instruction
		def initialize name, mnemonic, opcode
			super name, opcode, false, "#{mnemonic} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ]
		end
	end
	
	class MoveImm < Instruction
		def initialize name, mnemonic, opcode
			super name, opcode, true, "#{mnemonic} rd:reg, imm:imm", { "rd" => 0 }, [ 0, 0, 0, ]
		end
	end
	
	def self.def_movs suffix, op, opWide
		name = "mov#{suffix}"
		[ MoveReg.new(name, name, op), MoveImm.new("#{name}_imm", name, opWide) ]
	end
	
	class Shift < Instruction
		def initialize name, opcode
			super name, opcode, false, "#{name} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ]
		end
	end
	
	class NotWide < Instruction
		def initialize mnem, opcode, bindings
			super nil, opcode, false, mnem, bindings, [ 0, 0, 0 ]
			@uname = @pattern.mnem
		end
	end
		
	instructions =
	[
		Func.new("add",		0x01),
		Func.new("adc",		0x02),
		Func.new("sub",		0x05),
		Func.new("sbc",		0x06),
		Step.new("inc",		0x09),
		Step.new("dec",		0x0A),
		Func.new("or",		0x0B),
		Func.new("xor",		0x0D),
		Func.new("and",		0x0F),
		NotWide.new("not rd:reg, rs:reg", 0x11, { "rd" => 0, "rs" => 1 }),
		Shift.new("shl",	0x12),
		Shift.new("shr",	0x21),
		def_movs("", 		0x30,	0x3F),
		def_movs("ncnz",	0x31,	0x40),
		def_movs("ncz",		0x32,	0x41),
		def_movs("nc",		0x33,	0x42),
		def_movs("cnz",		0x34,	0x43),
		def_movs("nz",		0x35,	0x44),
		def_movs("nconz",	0x37, 	0x46),
		def_movs("cz",		0x38,	0x47),
		def_movs("z",		0x3A,	0x49),
		def_movs("ncoz",	0x3B,	0x4A),
		def_movs("c",		0x3C,	0x4B),
		def_movs("conz",	0x3D,	0x4C),
		def_movs("coz",		0x3E,	0x4D),
		NotWide.new("cmp ra:reg, rb:reg", 0x4E, { "ra" => 1, "rb" => 2 }),
		NotWide.new("ldr rd:reg, rs:reg", 0x51, { "rd" => 0, "rs" => 1 }),
		NotWide.new("str rd:reg, rs:reg", 0x54, { "rd" => 1, "rs" => 2 }),
		NotWide.new("call r:reg", 0x57, { "r" => 1 }),
		Instruction.new("call_imm", 0x58, true, "call addr:imm", { }, [ 0, 0, 0 ]),
		NotWide.new("in rd:reg, rs:reg", 0x59, { "rd" => 0, "rs" => 1 }),
		NotWide.new("out rd:reg, rs:reg", 0x5A, { "rd" => 1, "rs" => 2 }),
		NotWide.new("push r:reg", 0x5B, { "r" => 1 }),
		NotWide.new("pop r:reg", 0x5C, { "r" => 0 }),
		NotWide.new("halt", 0x7F, {})
	]
	
	aliass =
	[
		Alias.new("moveq",	"movz"),
		Alias.new("movnq",	"movnz"),
		Alias.new("movlt",	"movnc"),
		Alias.new("movgt",	"movcnz"),
		Alias.new("movle",	"movncoz"),
		Alias.new("movge",	"movc")
	]
	
	directives =
	[
		Directive.new("data",	:section,		"ParseDataSection"),
		Directive.new("bss",	:section,		"ParseBSSSection"),
		Directive.new("org",	:section_opt,	"ParseOrg"),
		Directive.new("align",	:section_opt,	"ParseAlign"),
		Directive.new("pad",	:section_opt,	"ParsePad"),
		Directive.new("resw",	:other,			"ParseResW"),
		Directive.new("global",	:other,			"ParseGlobal"),
		Directive.new("word",	:other,			"ParseWord")
	]
	
	ESC64AsmDesc.export instructions, aliass, directives
end

