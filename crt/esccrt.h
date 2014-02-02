#ifndef ESCCRT_INCLUDED
#define ESCCRT_INCLUDED

void __exit(int code);
void* __cmemcpy(void* dest, const void* src, unsigned n);
void __assert_fail(void);

#endif
