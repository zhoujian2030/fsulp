/*
 * mempool.h
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#ifndef MEM_POOL_H
#define MEM_POOL_H

unsigned char* AllocMemory(unsigned int size);
void FreeMemory(void* pBuffer);

#endif