/*
 * chipAlloc.h
 *
 *  Created on: 6 May 2020
 *      Author: adam
 */

#ifndef CHIPALLOC_H_
#define CHIPALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FRAMEBUFFER_ADDR ((char *)0xC0000000)
#define FRAMEBUFFER_SIZE 0x7F800

unsigned getBytesFree();

void SDmallocInit();
void *SDmalloc(unsigned size);
void *SDrealloc (void* ptr, unsigned size);
void SDfree(void *ptr);
unsigned SDgetBytesFree();

#ifdef __cplusplus
}
#endif

#endif /* CHIPALLOC_H_ */
