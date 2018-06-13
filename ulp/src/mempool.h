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

#include "list.h"

#define MAX_NUM_POOL_SIZE   10
#define MAGIC_NUMBER    0x86868686

#ifdef __x86_64__
#define SIZE_0      (128*2)
#define SIZE_1      (512*2)
#define SIZE_2      (1024*2)
#define SIZE_3      (1536*2)
#define SIZE_4      (3*1024*2)
#define SIZE_5      (5*1024*2)
#define SIZE_6      (8*1024*2)
#define SIZE_7      (10*1024*2)
#define SIZE_8      (32*1024*2)
#define SIZE_9      (128*1024*2)
#else 
#define SIZE_0      128
#define SIZE_1      512
#define SIZE_2      1024
#define SIZE_3      1536
#define SIZE_4      (3*1024)
#define SIZE_5      (5*1024)
#define SIZE_6      (8*1024)
#define SIZE_7      (16*1024)
#define SIZE_8      (48*1024)
#define SIZE_9      (128*1024)
#endif

#define NUM_SIZE_0      (20*1024)
#define NUM_SIZE_1      (8*1024)
#define NUM_SIZE_2      (2*1024)
#define NUM_SIZE_3      (2*1024)
#define NUM_SIZE_4      (2*1024)
#define NUM_SIZE_5      (2*1024)
#define NUM_SIZE_6      128
#define NUM_SIZE_7      32
#define NUM_SIZE_8      16
#define NUM_SIZE_9      8

typedef struct {
    unsigned short length;  // actual length of data buffer
    unsigned char* pData;
} MemBuf;

typedef struct {
    ListNode node;
    unsigned int  magicNo;
    unsigned int  poolId;
    unsigned short size;    // size of allocated memory
    unsigned short length;  // actual length of data buffer
    unsigned char* pData;
} MemNodeHead;

#define MEM_NODE_SIZE  sizeof(MemNode)
typedef struct {
    ListNode node;
    unsigned int  magicNo;
    unsigned short poolId;
    unsigned short isInUsed;
    unsigned short size;    // size of allocated memory
    unsigned short length;  // actual length of data buffer
    unsigned char* pData;
} MemNode;

typedef struct {
    unsigned int size;
    unsigned int num;
    unsigned char* pBufferStart;
} MemPoolConfig;

typedef struct {
    List pool[MAX_NUM_POOL_SIZE];
} MemPool;

// -------------------------------
// Init memory pool
extern void InitMemPool();

// Allocated a memory from pool
extern unsigned char* MemAlloc(unsigned int length);

// Free the memory back to pool
extern void MemFree(void* pBuffer);

// Get the actual used length of the memory
extern unsigned int MemGetLength(void* pBuffer);

// Get the size of the memory
extern unsigned int MemGetSize(void* pBuffer);

// Join two memory together
extern unsigned char* MemJoin(unsigned char* pSrcBuffer, unsigned char* pDstBuffer);

// Remove some bytes from memory
extern int MemRemove(unsigned char* pBuffer, unsigned int where, unsigned int count);

// Get mempool status
extern unsigned int MemGetNumMemBlock();
extern unsigned int MemGetAvailableMemBlock();
extern void MemDumpStatus();

#ifdef __cplusplus
}
#endif

#endif
