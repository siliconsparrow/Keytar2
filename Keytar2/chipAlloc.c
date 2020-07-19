/*
 * chipAlloc.c
 *
 *  Created on: 6 May 2020
 *      Author: adam
 */

// Memory management across the different RAM regions in the M7

// This is how I've divided up the RAM.
// 0x20000000 .. 0x2000FFFF ( 64k) TCM   - Tightly coupled memory for DMA transfers.
// 0x20010000 .. 0x2004BFFF (240k) SRAM  - Fast on-chip memory.
// 0x2004C000 .. 0x2004FFFF ( 16k) STACK - On-chip memory allocated to the program stack.
// 0xC0000000 .. 0xC07FFFFF (  8M) SDRAM - Off-chip memory. Big but slower.

#include "chipAlloc.h"
#include <errno.h>
#include <unistd.h>

extern char _heap_start; // set by linker
extern char _heap_end;   // set by linker

#define SDRAM_START ((char *)(0xC0000000 + FRAMEBUFFER_SIZE))
#define SDRAM_END   ((char *)0xC0800000)

void *allocSDRam(int size);
void *allocSRam(int size);

// Expand the heap. Called by malloc and new.
void *_sbrk(int incr)
{
	/*
	static char *heap_end = &_heap_start; // Previous end of heap or 0 if none
	char        *prev_heap_end;

	prev_heap_end  = heap_end;
	heap_end      += incr;
	if( heap_end > (&_heap_end)) {
		errno = ENOMEM;
		return (char*)-1;
	}

	return (void *)prev_heap_end;
*/
	void *result = allocSDRam(incr);
	if(result == 0)
	{
		errno = ENOMEM;
		return (char *)-1;
	}
	return result;
}

// Allocate a block of SRAM. Faster but there's less of it.
void *allocSRam(int size)
{
	static char *heapEnd = &_heap_start;

	char *blockStart = heapEnd;
	char *newBreak = heapEnd + size;
	if(newBreak > &_heap_end)
		return 0; // Not enough memory.

	heapEnd = newBreak;
	return (void *)blockStart;
}

void *allocSDRam(int size)
{
	static char *sdramEnd = SDRAM_START;

	char *blockStart = sdramEnd;
	char *newBreak = sdramEnd + size;
	if(newBreak > SDRAM_END)
		return 0; // Not enough memory.

	sdramEnd = newBreak;
	return (void *)blockStart;
}

//void freeSDRam(void *block)
//{
//	// TODO: Implement this!
//}
