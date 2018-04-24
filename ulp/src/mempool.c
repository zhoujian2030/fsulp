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

// ----------------------------------
unsigned char* AllocMemory(unsigned int size) 
{
    return malloc(size);
}

// ----------------------------------
void FreeMemory(void* pBuffer) 
{
    free(pBuffer);
}
