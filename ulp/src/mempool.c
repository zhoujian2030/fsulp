/*
 * mempool.c
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#include <stdlib.h>
#include <string.h>
#include "mempool.h"
#include "lteLogger.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gMemBufSize0, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize1, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize2, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize3, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize4, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize5, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize6, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize7, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize8, ".ulpdata");
#pragma DATA_SECTION(gMemBufSize9, ".ulpdata");
#pragma DATA_SECTION(gMemPoolConfig, ".ulpdata");
#pragma DATA_SECTION(gMemPool, ".ulpdata");
#pragma DATA_SECTION(NUM_MEM_BLOCK, ".ulpdata");
#endif
unsigned char gMemBufSize0[NUM_SIZE_0][SIZE_0] = {{0}, {0}};
unsigned char gMemBufSize1[NUM_SIZE_1][SIZE_1] = {{0}, {0}};
unsigned char gMemBufSize2[NUM_SIZE_2][SIZE_2] = {{0}, {0}};
unsigned char gMemBufSize3[NUM_SIZE_3][SIZE_3] = {{0}, {0}};
unsigned char gMemBufSize4[NUM_SIZE_4][SIZE_4] = {{0}, {0}};
unsigned char gMemBufSize5[NUM_SIZE_5][SIZE_5] = {{0}, {0}};
unsigned char gMemBufSize6[NUM_SIZE_6][SIZE_6] = {{0}, {0}};
unsigned char gMemBufSize7[NUM_SIZE_7][SIZE_7] = {{0}, {0}};
unsigned char gMemBufSize8[NUM_SIZE_8][SIZE_8] = {{0}, {0}};
unsigned char gMemBufSize9[NUM_SIZE_9][SIZE_9] = {{0}, {0}};

MemPoolConfig gMemPoolConfig[MAX_NUM_POOL_SIZE] = {
    {SIZE_0 - MEM_NODE_SIZE, NUM_SIZE_0, &gMemBufSize0[0][0]},
    {SIZE_1 - MEM_NODE_SIZE, NUM_SIZE_1, &gMemBufSize1[0][0]},
    {SIZE_2 - MEM_NODE_SIZE, NUM_SIZE_2, &gMemBufSize2[0][0]},
    {SIZE_3 - MEM_NODE_SIZE, NUM_SIZE_3, &gMemBufSize3[0][0]},
    {SIZE_4 - MEM_NODE_SIZE, NUM_SIZE_4, &gMemBufSize4[0][0]},
    {SIZE_5 - MEM_NODE_SIZE, NUM_SIZE_5, &gMemBufSize5[0][0]},
    {SIZE_6 - MEM_NODE_SIZE, NUM_SIZE_6, &gMemBufSize6[0][0]},
    {SIZE_7 - MEM_NODE_SIZE, NUM_SIZE_7, &gMemBufSize7[0][0]},
    {SIZE_8 - MEM_NODE_SIZE, NUM_SIZE_8, &gMemBufSize8[0][0]},
    {SIZE_9 - MEM_NODE_SIZE, NUM_SIZE_9, &gMemBufSize9[0][0]}
};

MemPool gMemPool;

unsigned int NUM_MEM_BLOCK = 0;

inline MemNode* CheckNode(void* pBuffer);
inline int findMatchPool(unsigned int size);

#ifdef ISSUE_DEBUG
void DumpPoolMemory()
{
    unsigned int i;
    for (i=0; i<NUM_SIZE_7; i++) {
        LOG_TRACE(ULP_LOGGER_NAME, "MemNode = %p\n", &gMemBufSize7[i][0]);
        LOG_BUFFER(&gMemBufSize7[i][0], 24);
    }
}
#endif

// ----------------------------------
void InitMemPool()
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int length;
    MemNode* pMem = 0;
    NUM_MEM_BLOCK = 0;
    for (i=0; i<MAX_NUM_POOL_SIZE; i++) {
        ListInit(&gMemPool.pool[i], 1);
        length = 0;
        for (j=0; j<gMemPoolConfig[i].num; j++) {
            pMem = (MemNode*)(gMemPoolConfig[i].pBufferStart + length);
            length = length + gMemPoolConfig[i].size + MEM_NODE_SIZE;
            memset(pMem, 0, sizeof(MemNode));
            pMem->size = gMemPoolConfig[i].size;
            pMem->poolId = i;
            pMem->magicNo = MAGIC_NUMBER;
            pMem->pData = (unsigned char*)pMem + MEM_NODE_SIZE;
            pMem->length = 0;
            pMem->isInUsed = 0;

            ListPushNode(&gMemPool.pool[i], &pMem->node);
        }
        NUM_MEM_BLOCK += gMemPoolConfig[i].num;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "NUM_MEM_BLOCK = %d\n", NUM_MEM_BLOCK); 
}

// --------------------------------
unsigned int MemGetNumMemBlock()
{
    return NUM_MEM_BLOCK;
}

// --------------------------------
extern unsigned int MemGetAvailableMemBlock()
{
    unsigned int count = 0, i;
    for(i=0; i<MAX_NUM_POOL_SIZE; i++) {
        count += ListCount(&gMemPool.pool[i]);
    }

    return count;
}

// --------------------------------
inline int findMatchPool(unsigned int size) {
    unsigned int i = 0;
    for(i=0; i<MAX_NUM_POOL_SIZE; i++) {
        if (size <= gMemPoolConfig[i].size) {
            // LOG_TRACE(ULP_LOGGER_NAME, "find poolId = %d, pool size = %d, actual size = %d\n", 
            //     i, gMemPoolConfig[i].size, size);
            return i;
        }
    }

    return -1;
}

// ----------------------------------
unsigned char* MemAlloc(unsigned int length) 
{
    int poolId = findMatchPool(length);
    if (poolId < 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory from pool, length = %d\n", length);
        return 0;
    }

    MemNode* pNode = 0;
    while (poolId < MAX_NUM_POOL_SIZE) {
        pNode = (MemNode*)ListPopNode(&gMemPool.pool[poolId]);
#ifdef ISSUE_DEBUG
        if (poolId == 7) {
            LOG_INFO(ULP_LOGGER_NAME, "Alloc pNode = %p in pool[%d]\n", pNode, poolId);
        }
#endif
        if (pNode != 0) {
            break;
        }

        LOG_DBG(ULP_LOGGER_NAME, "No available node in pool[%d]\n", poolId);

        poolId++;
    }   

    if (pNode != 0) {
        // LOG_TRACE(ULP_LOGGER_NAME, "length = %d, pBuffer = %p, mem size = %d\n", 
        //     length, pNode->pData, pNode->size);
        pNode->length = length;
        pNode->isInUsed = 1;
        return pNode->pData;
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "no available node, length = %d\n", length);
        return 0;
    }    
}

// ---------------------------
inline MemNode* CheckNode(void* pBuffer)
{
    if (pBuffer == 0) {
         LOG_ERROR(ULP_LOGGER_NAME, "pBuffer is null\n");
         return 0;
    }

    MemNode* pNode = (MemNode*)((unsigned char*)pBuffer - MEM_NODE_SIZE);
    if ((pNode == 0) || (pNode->magicNo != MAGIC_NUMBER) || (pNode->poolId >= MAX_NUM_POOL_SIZE)) {
        LOG_ERROR(ULP_LOGGER_NAME, "invalid MemNode = %p\n", pNode);
        return 0;
    }    

    return pNode;
}


// ----------------------------------
void MemFree(void* pBuffer) 
{
    MemNode* pNode = CheckNode(pBuffer);
    if (pNode) {
        // TODO need to add lock here to protect isInUsed ?
        if (pNode->isInUsed) {
            // LOG_TRACE(ULP_LOGGER_NAME, "free memory, length = %d, poolId = %d\n", 
            //     pNode->length, pNode->poolId);
#ifdef ISSUE_DEBUG
            if (pNode->poolId == 7) {
                LOG_INFO(ULP_LOGGER_NAME, "Free pNode = %p in pool[%d]\n", pNode, pNode->poolId);
            }
#endif
            pNode->length = 0;
            pNode->isInUsed = 0;
            ListPushNode(&gMemPool.pool[pNode->poolId], &pNode->node);
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "pNode = %p is already free\n", pNode);
        }
    }
}

// ----------------------------------
unsigned int MemGetLength(void* pBuffer)
{
    MemNode* pNode = CheckNode(pBuffer);

    if (pNode) {
        return pNode->length;
    }

    return 0;
}

// ----------------------------------
unsigned int MemGetSize(void* pBuffer)
{
    MemNode* pNode = CheckNode(pBuffer);

    if (pNode) {
        return pNode->size;
    }

    return 0;
}

// ----------------------------------
unsigned char* MemJoin(unsigned char* pSrcBuffer, unsigned char* pDstBuffer)
{
    LOG_TRACE(ULP_LOGGER_NAME, "pSrcBuffer = %p, pDstBuffer = %p\n", pSrcBuffer, pDstBuffer);

    MemNode* pSrcNode = CheckNode(pSrcBuffer);
    MemNode* pDstNode = CheckNode(pDstBuffer);    
    unsigned char* pRetBuffer = 0;

    if (pSrcNode != 0 && pDstNode != 0) {
        if (pSrcNode->size >= (pSrcNode->length + pDstNode->length)) {
            LOG_TRACE(ULP_LOGGER_NAME, "copy data to pSrcBuffer\n");
            memcpy(pSrcBuffer + pSrcNode->length, pDstBuffer, pDstNode->length);
            pSrcNode->length += pDstNode->length;
            MemFree(pDstBuffer);
            pRetBuffer = pSrcBuffer;
        } else if (pDstNode->size >= (pSrcNode->length + pDstNode->length)) {
            memcpy(pDstBuffer + pSrcNode->length, pDstBuffer, pDstNode->length);
            memcpy(pDstBuffer, pSrcBuffer, pSrcNode->length);
            pDstNode->length += pSrcNode->length;
            MemFree(pSrcBuffer);
            pRetBuffer = pDstBuffer;
        } else {
            pRetBuffer = MemAlloc(pSrcNode->length + pDstNode->length);
            if (pRetBuffer != 0) {
                memcpy(pRetBuffer, pSrcNode->pData, pSrcNode->length);
                memcpy(pRetBuffer + pSrcNode->length, pDstNode->pData, pDstNode->length);
            }
            MemFree(pSrcBuffer);
            MemFree(pDstBuffer);
        }
    } else {
        if (pSrcNode != 0) {
            MemFree(pSrcBuffer);
        }
        if (pDstNode != 0) {
            MemFree(pDstBuffer);
        }
    }

    return pRetBuffer;
}

// ---------------------------------
int MemRemove(unsigned char* pBuffer, unsigned int where, unsigned int count)
{
    LOG_TRACE(ULP_LOGGER_NAME, "pBuffer = %p, where = %d, count = %d\n", 
        pBuffer, where, count);

    MemNode* pNode = CheckNode(pBuffer);

    if (pNode == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "invalid pBuffer = %p\n", pBuffer);
        return 0;
    }

    if (where >= pNode->length) {
        LOG_WARN(ULP_LOGGER_NAME, "no data to be removed, length = %d, where = %d\n", 
            pNode->length, where);
        return 0;
    }

    if (count > (pNode->length - where)) {
        count = pNode->length - where;
        LOG_WARN(ULP_LOGGER_NAME, "count is too big, change to %d\n", count);
    } else {
        memmove(pBuffer + where, pBuffer + where + count, pNode->length - where - count);
    }
    pNode->length -= count;

    return count;
}

// ---------------------------------
void MemDumpStatus()
{
    LOG_DBG(ULP_LOGGER_NAME, "----------------------------\n");
    LOG_DBG(ULP_LOGGER_NAME, "Total Mem:    %d\n", NUM_MEM_BLOCK);
    LOG_DBG(ULP_LOGGER_NAME, "Pool %02d:    %d\n", NUM_MEM_BLOCK);

    unsigned int totalUsed = 0, count = 0, i;
    for(i=0; i<MAX_NUM_POOL_SIZE; i++) {
        count = ListCount(&gMemPool.pool[i]);
        totalUsed += count;
        LOG_DBG(ULP_LOGGER_NAME, "Pool %02d:    %d\n", NUM_MEM_BLOCK);
    }
    LOG_DBG(ULP_LOGGER_NAME, "Total Used:   %d\n", totalUsed);
    LOG_DBG(ULP_LOGGER_NAME, "----------------------------\n");
}
