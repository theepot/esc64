require 'set'
require 'optparse'
require 'htmlentities'

module ESC64AsmDesc
	DEBUG = false

	ARG_ID_TO_NAME	= [ "reg", "imm" ]
	ARG_C_NAMES		= [ "ARG_T_REG", "ARG_T_EXPR" ]
	ARG_NAME_TO_ID	= {}
	for i in 0...ARG_ID_TO_NAME.count
		ARG_NAME_TO_ID[ARG_ID_TO_NAME[i]] = i
	end
	KEYWORD_REGEX	= /\A[_a-zA-Z]\w*\z/
	SUBTYPE_MAP = { :section => "DIR_SUBT_SECTION", :section_opt => "DIR_SUBT_SECTION_OPT", :other => "DIR_SUBT_OTHER" }

	class Error < StandardError
		def initialize msg
			@msg = msg
		end
		
		def inspect; @msg end
		def to_s; inspect end
	end

	#base class for all instructions
	class Instruction
		attr_accessor :uname
		attr_accessor :opcode
		attr_accessor :wide
		attr_accessor :pattern
		attr_accessor :bindings
		attr_accessor :default_operands
		attr_accessor :pseudo
		attr_accessor :affects_status
		attr_accessor :descr
	
		def initialize uname, opcode, wide, pattern, bindings, default_operands, pseudo, affects_status, descr
			@uname = uname
			@opcode = opcode
			@wide = wide
			@pattern = Pattern.new pattern
			@bindings = bindings
			@default_operands = default_operands
			@pseudo = pseudo
			@descr = descr
			@affects_status = affects_status
		end
			
		def inspect
			"Instruction(uname=#{@uname}, opcode=#{@opcode}, pattern=#{@pattern}, bindings=#{@bindings}, default_operands=#{@default_operands})"
		end
		
		def legal?
			raise Error.new "Illegal unique name `#{@uname}'" unless KEYWORD_REGEX =~ @uname
			raise Error.new "Illegal opcode: #{@opcode}" if @opcode < 0 or @opcode > 127
			raise Error.new "Too many bindings" if @bindings.count > 3
			args = Set.new
			for i in 0...(@pattern.args.count)
				a = @pattern.args[i]
				raise Error.new "Multiple arguments with name `#{a.name}'" unless args.add? a.name
				raise Error.new "No binding for argument #{i}: `#{a}'" unless a.type == ARG_NAME_TO_ID["imm"] or @bindings.key? a.name
			end
			
			for k, v in @bindings
				raise Error.new "Invalid binding (#{k} => #{v})" if v < 0 or v > 2
			end
			
			raise Error.new "Illegal number of default operands (#{@default_operands.count})" if @default_operands.count != 3
			for i in 0...3
				op = @default_operands[i]
				if op < 0 or op > 7
					Error.new "Illegal value for small operand #{i} (#{op})"
				end
			end
		end
		
		def get_numerical_bindings
			if @pattern.args.count == 0 then return nil end
			numb = Array.new @bindings.count
			
			for i in 0...numb.count
				a = @pattern.args[i]
				if a.type == ARG_NAME_TO_ID["imm"] then next end
				n = @bindings[a.name]
				numb[i] = 6 - n * 3
			end
						
			numb
		end
		
		def get_inst_word
			(@opcode << 9) | (@default_operands[0] << 6) | (@default_operands[1] << 3) | @default_operands[2]
		end
		
		def get_c_name
			"#{@uname.upcase}_INSTWORD"
		end
		
		def reverse_bindings
			#[arg_num -> offset]
			rev_bind = []
			for arg in @pattern.args
				if arg.type == ARG_NAME_TO_ID["imm"]
					rev_bind << 0xFF
				else
					rev_bind << (6 - @bindings[arg.name] * 3)
				end
			end
			return rev_bind
		end
							
		def html_descr
			def yesno x; x ? "yes" : "no" end
			return	"<tr>\n" +
					"	<td class=\"mnemonic\">#{@pattern}</td>\n"+
					"	<td>opcode: #{sprintf "0x%02X", @opcode}</td>\n" +
					"	<td>wide: #{yesno @wide}</td>\n" +
					"	<td>affects status: #{yesno @affects_status}</td>\n" +
					"</tr>\n" +
					"<tr>\n" +
					"	<td colspan=\"4\">#{@descr}</td>\n"
					"</tr>\n"
		end
		
		def to_s; inspect end
	end

	#alias for instruction
	class Alias
		attr_accessor :name
		attr_accessor :target
		
		def initialize name, target
			@name = name
			@target = target
		end
		
		def legal?
			raise Error.new "Illegal name `#{@name}'" unless KEYWORD_REGEX =~ @name
			raise Error.new "Illegal target `#{@target}'" unless KEYWORD_REGEX =~ @target
		end
		
		def inspect; "Alias(name=#{@name}, target=#{@target}" end
		def to_s; inspect end
		
		def html_descr
			return	"<tr>\n" +
					"	<td>#{@name}</td>\n" +
					"	<td>#{@target}</td>\n" +
					"</tr>\n"
		end
	end

	#base class for all directives
	class Directive
		attr_reader :handler
		attr_reader :name
		attr_reader :args
		attr_reader :subtype
		attr_reader :descr

		def initialize name, subtype, handler, descr
			@handler = handler
			@subtype = subtype
			m = /(\w+)(\s+(.*))?/.match name
			@name = m[1].strip
			a = m[3]
			@args = a == nil ? "" : a.strip
			@descr = descr
			legal?
		end
		
		def inspect
			"Directive()"
		end
		
		def legal?
			raise Error.new "name has illegal value (#{@name})" unless KEYWORD_REGEX =~ @name
			raise Error.new "handler has illegal value (#{@handler})" unless KEYWORD_REGEX =~ @handler
			raise Error.new "invalid sub-type (#{@subtype})" unless SUBTYPE_MAP.has_key? @subtype
		end
		
		def subtype_c_name
			SUBTYPE_MAP[@subtype]
		end
		
		def c_desc_name
			"DIR_DESC_#{name.upcase}"
		end
		
		def html_descr
			return	"<tr>\n" +
					"	<td class=\"mnemonic\">.#{@name} #{@args}</td>\n" +
					"	<td>type: #{@subtype}</td>\n" +
					"</tr>\n" +
					"<tr>\n" +
					"	<td colspan=\"2\">#{HTMLEntities.new.encode @descr}</td>\n" +
					"</tr>\n"
		end
		
		def to_s; inspect end
	end

	class Arg
		attr_reader :name
		attr_reader :type

		def initialize name, type
			@name = name
			@type = type
		end

		def inspect
			"Arg(name=#{@name}, type=#{@type})"
		end
		
		def to_s
			"#{name}:#{ARG_ID_TO_NAME[type]}"
		end
	end

	class Pattern
		attr_reader :mnem
		attr_reader :args

		def initialize pat
			m = /(\w+)(\W+.+)?/.match pat
			@mnem = m[1].strip
			raise Error.new "Illegal mnemonic `#{@mnem}'" unless KEYWORD_REGEX =~ @mnem
			@args = []
			if m[2]
				m[2].strip.split(/\W*,\W*/).each do |a|
					a = /(\w+)\W*:\W*(\w+)/.match a
					raise Error.new "Invalid type" unless ARG_NAME_TO_ID.key? a[2]
					t = ARG_NAME_TO_ID[a[2]]
					raise Error.new "Illegal name" unless KEYWORD_REGEX =~ a[1]
					args << Arg.new(a[1], t)
				end
			end
		end
		
		def has_same_arg_types_as o
			if @args.count != o.args.count
				return false
			end
			
			for i in 0...(@args.count)
				if @args[i].type != o.args[i].type
					return false
				end
			end
			
			return true
		end
	
		def inspect
			"Pattern(mnem=#{@mnem}, args=#{@args})"
		end
		
		def to_s
			s = "#{mnem}"
			if args.count > 0
				s << " #{args[0]}"
				for i in 1...(args.count)
					s << ", #{args[i]}"
				end
			end
			return s
		end
	end

	class Interpreter
		def initialize
			@instructions = nil
			@directives = nil
			@aliass = nil
			
			@unames = Set.new
			@alias_names = Set.new
			@mnem_map = Hash.new
			@dirnames = Set.new
		end
		
		def export_instructions insts
			@instructions = insts
			@nopseudo = @instructions.dup.delete_if { |i| i.pseudo }
			for i in @nopseudo
				if @nopseudo.count { |j| j.opcode == i.opcode } > 1
					puts "ducplicated opcodes in pseudo: #{i.opcode}"
				end
			end
			
			for inst in @instructions
				begin
					legal_inst? inst
				rescue Error => x
					puts "while defining instruction `#{inst.uname}'"
					puts "\t#{x}"
				end
			end
			@unames = nil
		end
		
		def export_aliass aliass
			@aliass = aliass
			for a in aliass
				begin
					add_alias a
				rescue Error => x
					puts "while defining alias `#{a.name}'"
					puts "\t#{x}"
				end
			end
		end
		
		def export_directives dirs
			@directives = dirs
			for dir in @directives
				begin
					legal_dir? dir
				rescue Error => x
					puts "while defining directives `#{dir.name}'"
					puts "\t#{x}"
				end
			end
			@dirnames = nil
		end
		
		def legal_inst? inst		
			inst.legal?
		
			#check if uname is unique
			if not @unames.add? inst.uname
				raise Error.new "instruction `#{inst.uname}' is not unique"
			end
		
			#add to mnem_map
			if @mnem_map.key? inst.pattern.mnem
				iar = @mnem_map[inst.pattern.mnem]
				for other in iar
					if inst.pattern.has_same_arg_types_as other.pattern
						raise Error.new "`#{inst}' and `#{other}' have same patterns"
					end
				end
			
				iar << inst
			else
				@mnem_map[inst.pattern.mnem] = [ inst ]
			end
		end
		
		def add_alias a
			a.legal?
			
			#check if alias is unique
			if not @alias_names.add? a.name
				raise Error.new "alias `#{a.name}' is not uniqie"
			end
			
			#check that target exists
			raise Error.new "can't find target `#{a.target}' for alias `#{a.name}'" unless @mnem_map.key? a.target
			
			#check that alias doesn't already have an associated instruction
			raise Error.new "alias `#{a.name}' alread has an associated instruction" if @mnem_map.key? a.name
		end
		
		def legal_dir? dir
			#check if name is unique
			if not @dirnames.add? dir.name
				raise Error.new "`#{dir.name}' is not unique"
			end
		end
		
		def get_token_id_name name
			raise Error.new "Illegal name #{name}" unless KEYWORD_REGEX =~ name
			"TOKEN_ID_#{name.upcase}"
		end
		
		def emit_gperf_file filename
			File.open(filename, "w") do |f|
				f.puts "%{"
				emit_gperf_verbatim f
				f.puts "%}"
				emit_gperf_decls f
				f.puts "%%"
				emit_gperf_keywords f
				f.puts "%%"
				emit_gperf_funcs f
			end
		end
		
		def c_decomp_inst f, inst, index
			while index < inst.opcode
				f.write "\t{ 0 },\n"
				index = index + 1
			end
			rev_bind = inst.reverse_bindings
			
			if rev_bind.count > 0
				s = "{ #{rev_bind[0]}"
				(1...rev_bind.count).each { |i| s << ", #{rev_bind[i]}" }
				s << " }"
				s = "(const byte_t[])#{s}"
			else
				s = "NULL"
			end
			
			#debug
			x = "\t{ \"#{inst.pattern.mnem}\", \"#{inst.uname}\", #{inst.wide ? 1 : 0}, #{rev_bind.count}, #{s} }"
			#puts "c_decomp_inst: x=`#{x}'"
			f.write x
			
			#f.write "\t{ #{inst.pattern.mnem}, #{inst.uname}, #{inst.wide}, #{rev_bind.count}, #{s} }\t//#{inst.opcode}"
		end
		
		def emit_c_instr_info filename
			sorted = @nopseudo.sort { |a, b| a.opcode <=> b.opcode }
			
			File.open(filename + ".h", "w") do |f|
				f.write "/*GENERATED OUT OF descriptions.rb*/\n"
				f.write "#ifndef INSTR_INFO_INCLUDED\n"
				f.write "#define INSTR_INFO_INCLUDED\n\n"
				f.write "#ifndef __cplusplus\n"
				f.write "#include <stdbool.h>\n"
				f.write "#endif\n\n"
				
				f.write "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n"
				
				f.write "typedef enum opcode_t {\n"
				for inst in sorted
					f.write "\top_#{inst.uname.downcase}\t\t=#{sprintf("0x%02X", inst.opcode)},\n"
				end
				f.write "} opcode_t;\n"
				
				f.write "\n"
				
				f.write "typedef struct instr_info_t {\n"
				f.write "\topcode_t opcode; /*zero when operation does not exist*/\n"
				f.write "\tbool wide;\n"
				f.write "\tconst char* name; /*zero when operation does not exist*/\n"
				f.write "} instr_info_t;\n"
				
				f.write "\n"
				
				f.write "extern const instr_info_t instr_info[];\n"
				
				f.write "\n"
				
				f.write "#ifdef __cplusplus\n}\n#endif\n"
				f.write "\n#endif\n"
			end
			
			File.open(filename + ".c", "w") do |f|
				f.write "/*GENERATED OUT OF descriptions.rb*/\n"
				f.write "#ifndef __cplusplus\n"
				f.write "#include <stdbool.h>\n"
				f.write "#endif\n\n"
				
				f.write "#include <#{filename}.h>\n\n"
				
				f.write "const instr_info_t instr_info[] = {\n"

				def print_info_initializer f, inst, index
					while index < inst.opcode
						f.write "\t{.opcode = 0, .name = 0},\n"
						index = index + 1
					end
					
					f.write "\t{.opcode = op_#{inst.uname.downcase}, .wide=#{inst.wide}, .name = \"#{inst.uname}\"},\n"
					
				end
				
				print_info_initializer f, sorted[0], 0
				n = sorted[0].opcode + 1
				
				for i in 1...sorted.count
					print_info_initializer f, sorted[i], n
					n = sorted[i].opcode + 1
				end

				
				f.write "};\n"
				
			end
			
		end
		
		def emit_html_doc path
			if @instructions.count == 0
				return
			end
			
			File.open(path, "w") do |f|
				f.write "<html>\n"
				f.write "<head><link rel=\"stylesheet\" type=\"text/css\" href=\"mooiecss.css\"></head>\n"
				f.write "<body>\n"
				
				#instructions
				f.write "<h2>Instructions</h2>\n"
				f.write "<table>\n"
				f.write "\t#{@instructions[0].html_descr}"
				for i in 1...(@instructions.count)
					f.write "\t<tr></tr>\n"
					f.write "\t#{@instructions[i].html_descr}"
				end
				f.write "</table>\n\n"
				
				#aliasses
				f.write "<h2>Aliasses</h2>\n"
				f.write "<table>\n"
				f.write	"<tr class=\"tableheader\"><td>Name</td><td>Target</td></tr>\n"
				f.write "\t#{@aliass[0].html_descr}"
				for i in 1...(@aliass.count)
					f.write "\t<tr></tr>\n"
					f.write "\t#{@aliass[i].html_descr}"
				end
				f.write "</table>\n\n"
				
				#directives
				f.write "<h2>Directives</h2>\n"
				f.write "<table>\n"
				f.write "\t#{@directives[0].html_descr}"
				for i in 1...(@directives.count)
					f.write "\t<tr></tr>\n"
					f.write "\t#{@directives[i].html_descr}"
				end
				f.write "</table>\n\n"
				
				f.write "</body>\n"
				f.write "</html>\n"
			end
		end
		
		def emit_c_decomp filename
			File.open(filename, "w") do |f|
				f.write "#include <esc64asm/decomp.h>\n\n"
				
				sorted = @nopseudo.sort { |a, b| a.opcode <=> b.opcode }
				
				f.write "static const DecompInfo DECOMP_INFO[] =\n{\n"
				c_decomp_inst f, sorted[0], 0
				n = sorted[0].opcode + 1
				
				for i in 1...sorted.count
					f.write ",\n"
					c_decomp_inst f, sorted[i], n
					n = sorted[i].opcode + 1
				end
				f.write "\n};"
			end
		end
		
		def emit_py_disasm filename
			File.open(filename, "w") do |f|
				info = disasm_info
				
				f.write "class InstInfo:\n"
				f.write "\tdef __init__(self, uname, wide):\n"
				f.write "\t\tself.uname = uname\n"
				f.write "\t\tself.wide = wide\n\n"
				
				f.write "disasm_info = [\\\n"
				f.write "\t#{py_disasm_info info[0]}"
				
				for i in 1..0x7F
					f.write ",\\\n\t#{py_disasm_info info[i]}"
				end
				
				f.write "\\\n]\n"
			end
		end
		
		def py_disasm_info i
			if i == nil
				"None"
			else
				"InstInfo(\"#{i.uname}\", #{i.wide ? "True" : "False"})"
			end
		end
				
		def disasm_info
			if @disasm_info
				@disasm_info
			else
				gen_disasm_info
			end
		end
		
		def gen_disasm_info
			sorted = @nopseudo.sort { |a, b| a.opcode <=> b.opcode }
			n = 0
			@disasm_info = []
			for i in 0..0x7F
				if sorted[n].opcode == i
					@disasm_info << sorted[n]
					n = n + 1
				else
					@disasm_info << nil
				end
			end
			@disasm_info
		end
		
		def emit_gperf_verbatim f
			f.write "///// typedefs /////\n"
			f.write "typedef struct DirHandlerWrapper_ DirHandlerWrapper;\n"
			
#			f.write "///// constants /////\n"
#			if ARG_C_NAMES.count > 0
#				f.write "enum InstArgTypes_\n{\n"
#				f.write "\t#{ARG_C_NAMES[0]}\t= 0"
#				for i in 1...ARG_C_NAMES.count
#					f.write ",\n\t#{ARG_C_NAMES[i]}\t= #{i}"
#				end
#				f.write "\n};\n"
#			end

			f.write "///// instruction descriptors /////\n"
			emit_gperf_inst_descs f
			
			f.write "///// directive handlers /////\n"
			if @directives and @directives.count > 0
				for d in @directives
					f.write "extern void #{d.handler}(void);\n"
				end
			
				f.write "static const DirectiveHandler DIRECTIVE_HANDLERS[] =\n{\n"
				f.write "\t#{@directives[0].handler}"
				for i in 1...@directives.count
					f.write ",\n\t"
					f.write "#{@directives[i].handler}"
				end
				f.write "\n};\n"
			end
			
			f.write "///// directive descriptors /////\n"
			emit_gperf_dir_descs f
		end
		
		def emit_gperf_decls f
			f.write "struct Keyword_ { const char* keyword; KeywordType type; const void* info; };\n"
			f.write "struct DirHandlerWrapper_ { DirectiveHandler handler; };\n"
		end

		def emit_gperf_keywords f
			emit_gperf_insts f
			emit_gperf_aliass f
			emit_gperf_dirs f
		end

		def mnem_to_c_inst_desc mnem
			"INST_DESC_#{mnem.upcase}"
		end
		
		def mnem_to_c_instword mnem
			"#{mnem.upcase}_INSTWORD"
		end

		def emit_gperf_inst_descs f
			for mnem, instlist in @mnem_map
				sorted = instlist.sort { |a, b| compare_arglists a.pattern.args, b.pattern.args }
				args_root = create_args_tree sorted, 0, 0, sorted.count
				
				if DEBUG
					puts "\narg tree dump begin (mnem=#{mnem})"
					puts args_root
					puts "arg tree dump end\n"
				end
						
				ar = Array.new (args_root.count + 1) { 0 }
				ar[0] = args_root.count
				args_tree_to_array ar, 1, args_root
				
#				for inst in instlist
#					f.write "static const uword_t #{inst.get_c_name} = #{inst.get_inst_word};\n"
#				end
				f.write "static const byte_t #{mnem_to_c_inst_desc mnem}[] = { #{ar[0]}"
				(1...ar.count).each { |i| f.write ", #{ar[i]}" }
				f.write " };\n"
			end
		end
		
		def emit_gperf_dir_descs f
			for i in 0...@directives.count
				d = @directives[i]
				f.write "static const byte_t #{d.c_desc_name}[] = { #{d.subtype_c_name}, #{i} };\n"
			end
		end

		def emit_gperf_insts f
			for mnem, instlist in @mnem_map
				f.write "\"#{mnem}\", KEYWORD_TYPE_INST, #{mnem_to_c_inst_desc mnem}\n"
			end
		end

		def emit_gperf_aliass f
			for a in @aliass
				f.write "\"#{a.name}\", KEYWORD_TYPE_INST, #{mnem_to_c_inst_desc a.target}\n"
			end
		end

		def emit_gperf_dirs f
			for d in @directives
				f.write "\"#{d.name}\", KEYWORD_TYPE_DIR, #{d.c_desc_name}\n"
			end
		end

		def args_tree_to_array ar, parent_next_index, tree
			for node in tree
				ar[parent_next_index] = ar.count
				parent_next_index += 1
				ar << node.nexts.count
				ar << ARG_C_NAMES[node.argtype] if node.nexts.count > 0
				
				if node.nexts.count == 0
					w = node.inst.get_inst_word
					ar << sprintf("0x%X", (w >> 8) & 0xFF)
					ar << sprintf("0x%X", w & 0xFF)
					#s = "((const byte_t*)&#{node.inst.get_c_name})"
					#ar << "#{s}[0]" << "#{s}[1]"
					ar << (node.inst.wide ? "1" : "0")
					numb = node.inst.get_numerical_bindings
					if numb then numb.each { |b| ar << b } end
				else
					nexts_index = ar.count
					node.nexts.count.times { ar << 0 }
					args_tree_to_array ar, nexts_index, node.nexts
				end
			end
		end
		
		class InstNode
			attr_accessor :nexts
			# if leaf (i.e.: nexts.count == 0)
			attr_accessor :inst
			# if branch (i.e.: nexts.count > 0)
			attr_accessor :argtype
			
			def initialize
			end
			
			def tree_str d
				s = " " * d
				if @nexts.count > 0
					s << "branch(argtype=#{ARG_ID_TO_NAME[@argtype]})\n"
					for node in @nexts
						s << node.tree_str(d + 2)
					end
				else
					s << "leaf(inst=#{@inst}\n"
				end
				s
			end
			
			def inspect
				tree_str 0
			end
			
			def to_s; inspect end
		end
				
		def create_args_tree l, d, a, b
			nodes = []
			while a < b
				i = find_args_range l, d, a, b
				node = InstNode.new
				if d < l[a].pattern.args.count
					node.nexts = create_args_tree l, d + 1, a, i
					node.argtype = l[a].pattern.args[d].type
				else
					node.nexts = []
					node.inst = l[a]
				end
				nodes << node
				a = i
			end
	
			nodes
		end

		def find_args_range l, d, a, b
			if a + 1 == b then return b end

			for i in (a + 1)...b
				x = d < l[a].pattern.args.count ? l[a].pattern.args[d].type : nil
				y = d < l[i].pattern.args.count ? l[i].pattern.args[d].type : nil
		
				if x != y
					return i
				end
			end
	
			return b
		end

		def emit_gperf_funcs f
			#nothing
		end
				
		def compare_arglists a, b
			n = [ a.count, b.count ].min
			
			for i in 0...n
				x = a[i].type <=> b[i].type
				if x != 0 then
					return x
				end
			end
			
			a.count <=> b.count
		end
				
				
		def dump_instructions
			puts "Instruction dump\n{"
			
			for inst in @instructions
				puts "\t#{inst}"
			end
			
			puts "}"
		end
	end
		
	def self.export instructions, aliass, directives
		i = Interpreter.new
		i.export_instructions instructions.flatten
		i.export_aliass aliass
		i.export_directives directives.flatten
	
		gperf_path = nil
		decomp_c_path = nil
		c_instr_info_path = nil
		py_disasm_path = nil
		html_doc_path = nil
		verbose = false
		
		OptionParser.new do |opts|
			opts.on("-g PATH", "--emit-gperf PATH", "gperf file path") do |p|
				gperf_path = p
			end
			
			opts.on("-d PATH", "--emit-decomp PATH", "decompilation C code path") do |p|
				decomp_c_path = p
			end
			
			opts.on("-p PATH", "--emit-c-instr-info PATH", "C opcode defines") do |p|
				c_instr_info_path = p
			end
			
			opts.on("-y PATH", "--emit-python-disasm PATH", "disassembly info for python") do |p|
				py_disasm_path = p
			end
			
			opts.on("-c PATH", "--emit-html-doc PATH", "documentation in HTML form") do |p|
				html_doc_path = p
			end
			
			opts.on("-v", "--verbose", "be verbose") do |v|
				verbose = true
			end
		end.parse!
	
		if verbose
			puts "gperf_path=#{gperf_path}"
			i.dump_instructions
		end
		
		if gperf_path != nil
			i.emit_gperf_file gperf_path
		end
		
		if decomp_c_path != nil
			i.emit_c_decomp decomp_c_path
		end
		
		if py_disasm_path != nil
			i.emit_py_disasm py_disasm_path
		end
		
		if c_instr_info_path != nil
			i.emit_c_instr_info c_instr_info_path
		end
		
		if html_doc_path != nil
			i.emit_html_doc html_doc_path
		end
	end
end
