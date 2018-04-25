/*
 * mempool.c
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#include <stdlib.h>
#include "mempool.h"
#ifdef OS_LINUX
#include "CLogger.h"
#endif

unsigned int gNumMemAllocated = 0;

typedef struct {
    unsigned short size;

} MemoryNode;

// ----------------------------------
unsigned char* AllocMemory(unsigned int size) 
{
    void* pBuffer = malloc(size);
    gNumMemAllocated++;
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], gNumMemAllocated = %d, size = %d, pBuffer = %p\n", __func__, gNumMemAllocated, size, pBuffer);
    return pBuffer;
}

// ----------------------------------
void FreeMemory(void* pBuffer) 
{
    gNumMemAllocated--;
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], gNumMemAllocated = %d, pBuffer = %p\n", __func__, gNumMemAllocated, pBuffer);
    free(pBuffer);
}

// ----------------------------------
