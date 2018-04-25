/*
 * mempool.h
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#ifndef MEM_POOL_H
#define MEM_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char* AllocMemory(unsigned int size);
void FreeMemory(void* pBuffer);

#ifdef __cplusplus
}
#endif

#endif