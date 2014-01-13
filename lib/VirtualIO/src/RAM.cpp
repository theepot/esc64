#include <boost/utility.hpp>
#include <cassert>
#include <VirtualIO.hpp>
#include <verilogmem.lex.h>
#include <cstdio>
#include <stdexcept>
#include <boost/format.hpp>


#include "RAM.hpp"

namespace virtual_io {

RAM::RAM(bool is_device, int first_addr, int last_addr) :
		first_addr(first_addr),
		last_addr(last_addr),
		is_device(is_device) {
	assert(first_addr <= last_addr);
	assert(first_addr >= 0);

	memory = new BitVector16[get_size()];

	//init all cells to X
	for(int i = 0; i < get_size(); i++) {
		memory[i].a = -1;
		memory[i].b = -1;
	}


}

RAM::~RAM() {
	delete[] memory;
}

bool RAM::read(int addr, bool csh, bool csl, bool select_dev, BitVector16* out_data) {
	if(addr >= first_addr && addr <= last_addr && is_device == select_dev) {
		out_data[0] = memory[first_addr + addr];
		//TODO: listen to the CS signals
		return true;
	} else {
		return false;
	}
}

bool RAM::write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev) {
	if(addr >= first_addr && addr <= last_addr && is_device == select_dev) {
		if(csh) {
			//Z bits become X
			memory[first_addr + addr].a = (data.a & 0xFF00) | (data.b & 0xFF00) | (memory[first_addr + addr].a & ~0xFF00);
			memory[first_addr + addr].b = (data.b & 0xFF00) | (memory[first_addr + addr].b & ~0xFF00);

		}
		if(csl) {
			//Z bits become X
			memory[first_addr + addr].a = (data.a & 0xFF) | (data.b & 0xFF) | (memory[first_addr + addr].a & ~0xFF);
			memory[first_addr + addr].b = (data.b & 0xFF) | (memory[first_addr + addr].b & ~0xFF);
		}

		return true;
	} else {
		return false;
	}
}

void RAM::load_from_verilog_file(FILE* f) {

	verilogmem_process_state_t ps;
	ps.word_width = 16;
	ps.format = VERILOGMEM_FORMAT_BINARY;

	start_scanner(f, &ps);

	int addr = 0;

	int token;
	while((token = verilogmemlex()) != 0) {
		switch(token) {
		case VERILOGMEM_TOKEN_BINARY_WORD:
			if(addr < first_addr || addr > last_addr) {
				throw std::runtime_error((boost::format(
							"virtual-io: RAM: ERROR: could not store word at address %d. line %d: \"%s\". reason: address out of range\n")
							% addr % ps.line_number % ps.token_text).str());
			}
			BitVector16 word;
			word.a = 0;
			word.b = 0;
			for(int n = 0; n < 16; ++n) {

				word.a <<= 1;
				word.b <<= 1;

				switch(ps.last_value[15 - n]) {
				case VERILOGMEM_BIT_0:
					break;
				case VERILOGMEM_BIT_1:
					word.a |= 1;
					break;
				case VERILOGMEM_BIT_X:
					word.a |= 1;
					word.b |= 1;
					break;
				case VERILOGMEM_BIT_Z:
					word.b |= 1;
					break;
				default:
					assert(0);
				}
			}

			memory[first_addr + addr] = word;
			addr++;

			break;
		case VERILOGMEM_TOKEN_HEXADECIMAL_WORD:
			assert(0);
			break;
		case VERILOGMEM_TOKEN_ERROR:
//			printf("virtual-io: RAM: ERROR: reading memory at line %d: \"%s\". reason: %s\n", ps.line_number, ps.token_text, ps.error_reason);
			throw std::runtime_error((boost::format(
					"virtual-io: RAM: ERROR: reading memory at line %d: \"%s\". reason: %s\n")
					% ps.line_number % ps.token_text % ps.error_reason).str());
//			break;
		case VERILOGMEM_TOKEN_ADDRESS:
			addr = ps.address;
			break;
		case VERILOGMEM_TOKEN_COMMENT:
		case VERILOGMEM_TOKEN_MULTILINE_COMMENT:
			break;
		default:
			printf("[%d] unimplemented token. token number %d\n", ps.line_number, token);
			assert(0);
			break;
		}
	}
}

} //namespace

