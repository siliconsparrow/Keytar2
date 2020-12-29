// Various overrides and such to make C++ compile to a much smaller image.
// Essentially, the big bloats in C++ are exception handling and RTTI. Use the
// following compiler switches to turn them off:
//    -fno-exceptions -fno-rtti

#include <new>
#include <malloc.h>

//#include "Uart.h"

// This is to prevent an exception being thrown for a pure virtual function.
extern "C" void __cxa_pure_virtual() { while(1); }

// New and delete overrides public domain code by Eric Agan.

void* operator new(std::size_t size) {
    return malloc(size);
}

void* operator new[](std::size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) {
	/*
	static const char *HEXIT = "0123456789ABCDEF";

	char buf[32];
	const char *s = "DEL 0x........\r\n";
	for(int i = 0; ; i++)
	{
		buf[i] = s[i];
		if(s[i] == 0)
			break;
	}

	unsigned p = (unsigned)ptr;
	for(int i = 6; i < 8+6; i++)
	{
		buf[i] = HEXIT[p >> 28];
		p <<= 4;
	}
	Uart::getPort(0)->sendStr(buf);
*/
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

// Optionally you can override the 'nothrow' versions as well.
// This is useful if you want to catch failed allocs with your
// own debug code, or keep track of heap usage for example,
// rather than just eliminate exceptions.

void* operator new(std::size_t size, const std::nothrow_t&) {
    return malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) {
    return malloc(size);
}

void operator delete(void* ptr, const std::nothrow_t&) {
    free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) {
    free(ptr);
}
