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
#include <malloc.h>
#include <stdint.h>
#include <string.h>

#define MALLOC_SRAM

extern char _heap_start; // set by linker
extern char _heap_end;   // set by linker

#define SDRAM_START ((char *)(0xC0000000 + FRAMEBUFFER_SIZE))
#define SDRAM_END   ((char *)0xC0800000)

void *allocSDRam(int size);
void *allocSRam(int size);

// Expand the heap. Called by malloc and new.
void *_sbrk(int incr)
{
#if defined(MALLOC_SRAM)
	void *result = allocSRam(incr);
#elif defined(MALLOC_SDRAM)
	void *result = allocSDRam(incr);
#else
	#error "Must define either MALLOC_SRAM or MALLOC_SDRAM"
#endif
	if(result == 0)
	{
		errno = ENOMEM;
		return (char *)-1;
	}
	return result;
}

// Return amount of free RAM that has not been allocated to the heap yet.
unsigned getUnusedHeap()
{
#ifdef MALLOC_SRAM
	return (uint8_t *)&_heap_end - (uint8_t *)allocSRam(0);
#else
	return (uint8_t *)SDRAM_END - (uint8_t *)allocSDRam(0);
#endif
}

unsigned getBytesFree()
{
	// Get the free space available from the libc memory manager.
	struct mallinfo meminfo;
	meminfo = mallinfo();

	// Add it to the amount of free heap space.
	return meminfo.fordblks + getUnusedHeap();
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

/*
 // This is replaced by the SDmalloc and friends functions below.
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
*/

// *********
// ** Pool allocator for the SDRAM memory space.

//#define SDRAM_SMALL_BLOCKS

#ifdef SDRAM_SMALL_BLOCKS
// Configuration - how many bins of each size should be allocated for each pool.
struct SDPool { uint32_t blockSize; uint32_t nBlocks; uint32_t addr; };
struct SDPool sdPools[] = {
		{ 16,  1024, 0 },
		{ 32,  1024, 0 },
		{ 64,  128, 0 },
		{ 256, 128, 0 },
		{ 0, 0, 0 } // Indicates end of configuration.
};
#endif // SDRAM_SMALL_BLOCKS

#define SDBLOCKFREE         0x80000000
#define SDBLOCK0            ((uint32_t)SDRAM_START + sizeof(uint32_t))
#define SDBLOCKINFO(addr)   (((uint32_t *)(addr))[-1])
#define SDBLOCKSIZE(addr)   (SDBLOCKINFO(addr)&~SDBLOCKFREE)
#define SDBLOCKISFREE(addr) (0!=(SDBLOCKINFO(addr)&SDBLOCKFREE))
#define SDNEXTBLOCK(addr)   (((uint32_t)addr)+SDBLOCKSIZE(addr)+sizeof(uint32_t))

unsigned g_sdBytesFree = 0;

void SDmallocInit()
{
	uint32_t       addr = SDBLOCK0;

#ifdef SDRAM_SMALL_BLOCKS
	struct SDPool *pool;

	for(int i = 0; ; i++) {
		pool = &sdPools[i];
		pool->addr = addr;
		if(pool->blockSize == 0)
			break;

		for(int j = 0; j < pool->nBlocks; j++) {
			SDBLOCKINFO(addr) = SDBLOCKFREE | pool->blockSize;
			addr += sizeof(uint32_t) + pool->blockSize;
			g_sdBytesFree += pool->blockSize;
		}
	}

	pool->addr = addr;
#endif // SDRAM_SMALL_BLOCKS

	SDBLOCKINFO(addr) = SDBLOCKFREE | ((uint32_t)SDRAM_END - addr);
	g_sdBytesFree += SDBLOCKSIZE(addr);
}

void *SDmalloc(unsigned size)
{
	uint32_t addr;

#ifdef SDRAM_SMALL_BLOCKS
	// Size must be aligned.
	size = (size + 3) & (~3);

	// See if there is a pool of the right size.
	for(int i = 0; ; i++) {
		struct SDPool *pool = &sdPools[i];
		addr = pool->addr;
		if(pool->blockSize == 0)
			break; // End of pools.

		// Does the requested block belong in the current pool?
		if(size <= pool->blockSize) {
			// Find a free bin within the current pool.
			for(int j = 0; j < pool->nBlocks; j++) {
				if(SDBLOCKISFREE(addr)) {
					SDBLOCKINFO(addr) = pool->blockSize;
					g_sdBytesFree += pool->blockSize;
					return (void *)addr; // Allocation successful.
				}
				addr += pool->blockSize + sizeof(uint32_t);
			}
		}
	}
#else
	// Small blocks are allocated using malloc(). It's more efficient than my algorithm but it does consume SRAM.
	if(size < 1024) {
		return malloc(size);
	} else {
		addr = SDBLOCK0;

		// Size must be aligned.
		size = (size + 3) & (~3);
	}
#endif // SDRAM_SMALL_BLOCKS

	// No pool space available. Try a regular allocation.
	uint32_t addrFound = 0;
	uint32_t addrFoundSize = 0x0FFFFFFF;
	while(addr < (uint32_t)SDRAM_END) {
		// Is the current block free?
		if(SDBLOCKISFREE(addr)) {
			// Can the current block be merged with the next one?
			while(1) {
				uint32_t addrNext = SDNEXTBLOCK(addr);
				if(addrNext >= (uint32_t)SDRAM_END)
					break; // No more blocks.
				if(!SDBLOCKISFREE(addrNext))
					break; // Next block is in use, cannot be merged.

				// Merge blocks.
				SDBLOCKINFO(addr) += SDBLOCKSIZE(addrNext) + sizeof(uint32_t);
				g_sdBytesFree += sizeof(uint32_t);
			}

			// Does the requested amount fit in the current free block?
			uint32_t sz = SDBLOCKSIZE(addr);
			if(size <= sz) {
				if(sz < addrFoundSize) {
					addrFound = addr;
					addrFoundSize = sz;
				}
			}
		}
		addr = SDNEXTBLOCK(addr);
	}

	// Return the best fitting block.
	if(addrFound != 0) {
		// Adjust the block size to that of the requested allocation.
		//uint32_t bsz = SDBLOCKSIZE(addrFound);
		if(addrFoundSize > size + sizeof(uint32_t)) {
			uint32_t addrNext = addrFound + size + sizeof(uint32_t);
			SDBLOCKINFO(addrFound) = size;
			SDBLOCKINFO(addrNext) = SDBLOCKFREE | (addrFoundSize - size - sizeof(uint32_t));
			g_sdBytesFree -= sizeof(uint32_t);
		} else {
			// Block was already the exact size. No need to adjust.
			SDBLOCKINFO(addrFound) &= ~SDBLOCKFREE;
		}
		g_sdBytesFree -= size;
		return (void *)addrFound;
	}

	return 0; // Block cannot be allocated.
}

// Allocate a new block of SDRAM and copy the existing contents across.
void *SDreallocTheHardWay(void *ptr, unsigned size)
{
	void *result = SDmalloc(size);
	if(result > 0) {
		unsigned origSize = SDBLOCKSIZE(ptr);
		memcpy(result, ptr, size > origSize ? origSize : size);
		SDfree(ptr);
	}
	return result;
}

void *SDrealloc(void *ptr, unsigned size)
{
#ifdef SDRAM_SMALL_BLOCKS
	// Size must be aligned.
	size = (size + 3) & (~3);

	// Are we in a pool?
	for(int i = 0; ; i++) {
		struct SDPool *pool = &sdPools[i];
		if(pool->blockSize == 0)
			break; // End of pools.

		// Are we in this pool?
		if((uint32_t)ptr < sdPools[i + 1].addr) {
			// Yes. Does the block still fit within the pool?
			if(size < pool->blockSize) {
				return ptr; // Yes. no need to move it.
			} else {
				// Block is too big an cannot stay in this pool. Move it.
				return SDreallocTheHardWay(ptr, size);
			}
		}
	}
#else
	if((char *)ptr < SDRAM_START) {
		return realloc(ptr, size);
	}

	// Size must be aligned.
	size = (size + 3) & (~3);

#endif // SDRAM_SMALL_BLOCKS

	// Not in a pool, check if there is enough free space that we can leave the pointer in place.
	while(SDBLOCKSIZE(ptr) < size) {
		uint32_t addrNext = SDNEXTBLOCK(ptr);
		if(!SDBLOCKISFREE(addrNext)) {
			// Cannot enlarge the block enough. We'll have to move it.
			return SDreallocTheHardWay(ptr, size);
		}

		// Merge next block.
		SDBLOCKINFO(ptr) = SDBLOCKSIZE(ptr) + sizeof(uint32_t) + SDBLOCKSIZE(addrNext);
		g_sdBytesFree +=4;
	}

	// Block enlarged, probably by too much, so trim it down.
	unsigned bsz = SDBLOCKSIZE(ptr);
	if(bsz >= size + 16) {
		uint32_t addrNext = (uint32_t)ptr + size + sizeof(uint32_t);
		SDBLOCKINFO(ptr) = size;
		SDBLOCKINFO(addrNext) = SDBLOCKFREE | (bsz - size - sizeof(uint32_t));
		g_sdBytesFree -= sizeof(uint32_t);
	}

	// Block adjusted successfully. Leave it in place.
	return ptr;
}

void SDfree(void *ptr)
{
#ifndef SDRAM_SMALL_BLOCKS
	if((char *)ptr < SDRAM_START) {
		free(ptr);
		return;
	}
#endif // !SDRAM_SMALL_BLOCKS
	g_sdBytesFree += SDBLOCKSIZE(ptr);
	SDBLOCKINFO(ptr) |= SDBLOCKFREE;
}

unsigned SDgetBytesFree()
{
	return g_sdBytesFree;
}
