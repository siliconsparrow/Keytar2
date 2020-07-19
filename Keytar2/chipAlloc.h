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

void *allocSRam(int size);
//void *allocSDRam(int size);
//void freeSDRam(void *block);

#ifdef __cplusplus
}
#endif

#endif /* CHIPALLOC_H_ */
