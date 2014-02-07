#ifndef ESCIO_INCLUDED
#define ESCIO_INCLUDED

void io_out(unsigned addr, unsigned data);
void io_outa(unsigned addr, const void* p, unsigned amount, int dobyte);
#define io_outac(a, p, n)	io_outa((a), (p), (n), 1)
#define io_outai(a, p, n)	io_outa((a), (p), (n), 0)
void io_outs(unsigned addr, const char* s);
unsigned io_in(unsigned addr);
void io_ina(unsigned addr, void* p, unsigned amount, int dobyte);
#define io_inac(a, p, n)	io_ina((a), (p), (n), 1)
#define io_inai(a, p, n)	io_ina((a), (p), (n), 0)

#endif
