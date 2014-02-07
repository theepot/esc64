require 'interpreter.rb'

module Descriptions
	include ESC64AsmDesc

	class Func < Instruction
		def initialize name, opcode, descr
			super name, opcode, false, "#{name} rd:reg, ra:reg, rb:reg", { "rd" => 0, "ra" => 1, "rb" => 2 }, [ 0, 0, 0 ], false, true, descr
		end
	end
	
	class Step < Instruction
		def initialize name, opcode, descr
			super name, opcode, false, "#{name} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ], false, true, descr
		end
	end
	
	class MoveReg < Instruction
		def initialize name, mnemonic, opcode, descr
			super name, opcode, false, "#{mnemonic} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ], false, false, descr
		end
	end
	
	class MoveImm < Instruction
		def initialize name, mnemonic, opcode, descr
			super name, opcode, true, "#{mnemonic} rd:reg, imm:imm", { "rd" => 0 }, [ 0, 0, 0, ], false, false, descr
		end
	end
	
	def self.def_movs suffix, op, opWide, cond
		name = "mov#{suffix}"
		f = "moves %s to rd. contition: #{cond}"
		[ MoveReg.new(name, name, op, f % ["rs"]), MoveImm.new("#{name}_imm", name, opWide, f % ["imm"]) ]
	end
	
	class JmpReg < Instruction
		def initialize name, mnemonic, opcode, descr
			super name, opcode, false, "#{mnemonic} rd:reg", { "rd" => 1 }, [ 7, 0, 0 ], true, false, descr
		end
	end
	
	class JmpImm < Instruction
		def initialize name, mnemonic, opcode, descr
			super name, opcode, true, "#{mnemonic} imm:imm", {}, [ 7, 0, 0 ], true, false, descr
		end
	end
	
	def self.def_jmps name, op, opWide, cond
		f = "jumps to %s. contition: #{cond}"
		[ JmpReg.new(name, name, op, f % ["rs"]), JmpImm.new("#{name}_imm", name, opWide, f % ["imm"]) ]
	end
	
	class Shift < Instruction
		def initialize name, opcode, descr
			super name, opcode, false, "#{name} rd:reg, rs:reg", { "rd" => 0, "rs" => 1 }, [ 0, 0, 0 ], false, true, descr
		end
	end
	
	class NotWide < Instruction
		def initialize mnem, opcode, bindings, affects_status, descr
			super nil, opcode, false, mnem, bindings, [ 0, 0, 0 ], false, affects_status, descr
			@uname = @pattern.mnem
		end
	end
		
	instructions =
	[
		Func.new("add",		0x01,	"rd = ra + rb"),
		Func.new("adc",		0x02,	"rd = ra + rb + carry"),
		Func.new("sub",		0x05,	"rd = ra - rb"),
		Func.new("sbc",		0x06,	"rd = ra - rb + 1 - carry"),
		Step.new("inc",		0x09,	"rd = ra + 1"),
		Step.new("dec",		0x0A,	"rd = ra - 1"),
		Func.new("or",		0x0B,	"rd = ra OR rb"),
		Func.new("xor",		0x0D,	"rd = ra XOR rb"),
		Func.new("and",		0x0F,	"rd = ra AND rb"),
		NotWide.new("not rd:reg, rs:reg", 0x11, { "rd" => 0, "rs" => 1 }, true, "ra = NOT rb"),
		Shift.new("shl",	0x12,	"rd = ra logically shifted left once"),
		Shift.new("shr",	0x21,	"rd = ra logically shifted right once"),
		
		def_movs("", 		0x30,	0x3F,	"always"),
		def_movs("ncnz",	0x31,	0x40,	"NOT carry AND NOT zero"),
		def_movs("ncz",		0x32,	0x41,	"NOT carry AND zero"),
		def_movs("nc",		0x33,	0x42,	"NOT carry"),
		def_movs("cnz",		0x34,	0x43,	"carry AND NOT zero"),
		def_movs("nz",		0x35,	0x44,	"NOT zero"),
		def_movs("nconz",	0x37, 	0x46,	"NOT carry OR NOT zero"),
		def_movs("cz",		0x38,	0x47,	"carry AND zero"),
		def_movs("z",		0x3A,	0x49,	"zero"),
		def_movs("ncoz",	0x3B,	0x4A,	"NOT carry OR zero"),
		def_movs("c",		0x3C,	0x4B,	"carry"),
		def_movs("conz",	0x3D,	0x4C,	"carry OR NOT zero"),
		def_movs("coz",		0x3E,	0x4D,	"carry OR zero"),
		
		def_jmps("jmp",		0x30,	0x3F,	"always"),
		def_jmps("jnc",		0x33,	0x42,	"NOT carry"),
		def_jmps("jcnz",	0x34,	0x43,	"carry AND NOT zero"),
		def_jmps("jnz",		0x35,	0x44,	"NOT zero"),
		def_jmps("jz",		0x3A,	0x49,	"zero"),
		def_jmps("jncoz",	0x3B,	0x4A,	"NOT carry OR zero"),
		def_jmps("jc",		0x3C,	0x4B,	"carry"),
		
		NotWide.new("cmp ra:reg, rb:reg", 0x4E, { "ra" => 1, "rb" => 2 }, true,		"subtracts rb from ra as unsigned words and sets the status bits accordingly"),
		NotWide.new("scmp ra:reg, rb:reg", 0x4F, { "ra" => 1, "rb" => 2 }, true,	"subtracts rb from ra as signed words and sets the status bits accordingly"),
		
		NotWide.new("sxt rd:reg, rs:reg", 0x50, { "rd" => 0, "rs" => 1 }, false,	"the lower byte in rs is sign extended to a word into rd"),
		
		NotWide.new("ld rd:reg, rs:reg", 0x51, { "rd" => 0, "rs" => 1 }, false,		"rd is loaded with the word at the address specified by rs"),
		NotWide.new("ldb rd:reg, rs:reg", 0x52, { "rd" => 0, "rs" => 1 }, false,	"rd is loaded with the byte at the address specified by rs"),
		
		NotWide.new("st rd:reg, rs:reg", 0x54, { "rd" => 1, "rs" => 2 }, false,		"rs is stored at the address specified by rd"),
		NotWide.new("stb rd:reg, rs:reg", 0x55, { "rd" => 1, "rs" => 2 }, false,	"the lower byte in rs is stored at the address specified by rd"),
		
		NotWide.new("call r:reg", 0x57, { "r" => 1 }, false,						"jumps to address r. the current address+2 is pushed on the stack"),
		Instruction.new("call_imm", 0x58, true, "call addr:imm", { }, [ 0, 0, 0 ], false, false, "jumps to address imm. the current address+2 is pushed on the stack"),
		NotWide.new("in rd:reg, rs:reg", 0x59, { "rd" => 0, "rs" => 1 }, false,		"rd is loaded with the value retreived from an io-device at address rs"),
		NotWide.new("out rd:reg, rs:reg", 0x5A, { "rd" => 1, "rs" => 2 }, false,	"rs is written to an io-device at address rd"),
		NotWide.new("push r:reg", 0x5B, { "r" => 1 }, false,						"pushes r on the stack"),
		NotWide.new("pop r:reg", 0x5C, { "r" => 0 }, false,							"pops a word from the stack into r"),
		Instruction.new("ret", 0x5C, false, "ret", {}, [ 7, 0, 0 ], true, false,	"pops a word from the stack into the program counter"),
		NotWide.new("halt", 0x7F, {}, false, "halts execution of the processor")
	]
	
	aliass =
	[
		Alias.new("moveq",	"movz"),
		Alias.new("movne",	"movnz"),
		Alias.new("movlt",	"movnc"),
		Alias.new("movgt",	"movcnz"),
		Alias.new("movle",	"movncoz"),
		Alias.new("movge",	"movc"),
		
		Alias.new("jeq",	"jz"),
		Alias.new("jne",	"jnz"),
		Alias.new("jlt",	"jnc"),
		Alias.new("jgt",	"jcnz"),
		Alias.new("jle",	"jncoz"),
		Alias.new("jge",	"jc")
	]
	
	directives =
	[
		Directive.new("data",		:section,		"ParseDataSection",	"starts a new data section"),
		Directive.new("bss",		:section,		"ParseBSSSection",	"starts a new BSS section"),
		Directive.new("org A",		:section_opt,	"ParseOrg",			"originates this section at address A"),
		Directive.new("align A",	:section_opt,	"ParseAlign",		"makes sure this section is aligned to A bytes"),
		Directive.new("pad P",		:other,			"ParsePad",			"inserts padding until the section is aligned to P from the start of the section"),
		Directive.new("resw N",		:other,			"ParseResW",		"reserves N words of space"),
		Directive.new("resb N",		:other,			"ParseResB",		"reserves N bytes of space"),
		Directive.new("global L",	:other,			"ParseGlobal",		"defines label L as a global label. not that this is the DEFINITION of the label\n" +
																		"this means that this is correct:\n" +
																		"\t.global label:\n" +
																		"and this is not:\n" +
																		"\t.global label\n" +
																		"\tlabel:"),
		Directive.new("word W",		:other,			"ParseWord",		"inserts a word of value W"),
		Directive.new("byte B",		:other,			"ParseByte",		"inserts a byte of value B")
	]
	
	ESC64AsmDesc.export instructions, aliass, directives
end

