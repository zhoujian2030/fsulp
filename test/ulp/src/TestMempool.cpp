/*
 * TestMempool.cpp
 *
 *  Created on: Apr 25, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestMempool.h"
#include "mempool.h"
#include "list.h"

using namespace std;

extern unsigned int gNumMemAllocated;

extern unsigned char gMemBufSize0[NUM_SIZE_0][SIZE_0]; 
extern unsigned char gMemBufSize1[NUM_SIZE_1][SIZE_1];
extern unsigned char gMemBufSize2[NUM_SIZE_2][SIZE_2];
extern unsigned char gMemBufSize3[NUM_SIZE_3][SIZE_3];
extern unsigned char gMemBufSize4[NUM_SIZE_4][SIZE_4];
extern unsigned char gMemBufSize5[NUM_SIZE_5][SIZE_5];
extern unsigned char gMemBufSize6[NUM_SIZE_6][SIZE_6];
extern unsigned char gMemBufSize7[NUM_SIZE_7][SIZE_7];
extern unsigned char gMemBufSize8[NUM_SIZE_8][SIZE_8];
extern unsigned char gMemBufSize9[NUM_SIZE_9][SIZE_9];

extern MemPool gMemPool;

extern unsigned char gLogLevel;

// ------------------------
TEST_F(TestMempool, Allocate_Memory_Success) {
    gLogLevel = 0;
    InitMemPool();

    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[4]), NUM_SIZE_4);

    // allocated first memory from pool 0
    unsigned int allocatedSize = SIZE_0 - MEM_NODE_SIZE;
    unsigned char* buffer1 = MemoryAlloc(allocatedSize);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0-1);
    ASSERT_TRUE(buffer1 != 0);

    MemNode* pNode = (MemNode*)(buffer1 - MEM_NODE_SIZE);
    EXPECT_EQ(pNode->pData, buffer1);
    EXPECT_EQ((unsigned char*)pNode, &gMemBufSize0[0][0]);
    EXPECT_EQ(pNode->length, allocatedSize);
    EXPECT_EQ(pNode->size, SIZE_0 - MEM_NODE_SIZE);
    EXPECT_EQ((int)pNode->poolId, 0);

    // allocated second memory from pool 0
    allocatedSize = SIZE_0 - MEM_NODE_SIZE - 10;
    unsigned char* buffer2 = MemoryAlloc(allocatedSize);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0-2);
    ASSERT_TRUE(buffer2 != 0);

    pNode = (MemNode*)(buffer2 - MEM_NODE_SIZE);
    EXPECT_EQ(pNode->pData, buffer2);
    EXPECT_EQ((unsigned char*)pNode, &gMemBufSize0[1][0]);
    EXPECT_EQ(pNode->length, allocatedSize);
    EXPECT_EQ(pNode->size, SIZE_0 - MEM_NODE_SIZE);
    EXPECT_EQ((int)pNode->poolId, 0);

    // allocated 3rd memory from pool 4
    allocatedSize = SIZE_3 - MEM_NODE_SIZE + 1;
    unsigned char* buffer3 = MemoryAlloc(allocatedSize);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[4]), NUM_SIZE_4-1);
    ASSERT_TRUE(buffer3 != 0);

    pNode = (MemNode*)(buffer3 - MEM_NODE_SIZE);
    EXPECT_EQ(pNode->pData, buffer3);
    EXPECT_EQ((unsigned char*)pNode, &gMemBufSize4[0][0]);
    EXPECT_EQ(pNode->length, allocatedSize);
    EXPECT_EQ(pNode->size, SIZE_4 - MEM_NODE_SIZE);
    EXPECT_EQ((int)pNode->poolId, 4); 

    printf("buffer1 = %p\n", buffer1);
    printf("buffer2 = %p\n", buffer2);
    printf("buffer3 = %p\n", buffer3);

    // free memory
    MemoryFree(buffer1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    MemoryFree(buffer2);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    MemoryFree(buffer3);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[4]), NUM_SIZE_4);
}

// ------------------------
TEST_F(TestMempool, Allocate_Memory_Failure_In_1st_Pool) {
    gLogLevel = 1;

    InitMemPool();

    unsigned i = 0;
    unsigned char* pBuffer = 0;
    unsigned int size = SIZE_5 - MEM_NODE_SIZE - 1;
    for (i = 0; i<NUM_SIZE_5; i++) {
        pBuffer = MemoryAlloc(size);
        ASSERT_TRUE(pBuffer != 0);
        EXPECT_EQ(ListCount(&gMemPool.pool[5]), NUM_SIZE_5 - i - 1);
    }
    EXPECT_EQ((int)ListCount(&gMemPool.pool[5]), 0);

    pBuffer = MemoryAlloc(size);
    ASSERT_TRUE(pBuffer != 0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[6]), NUM_SIZE_6 - 1);
}

// ------------------------
TEST_F(TestMempool, Allocate_Memory_Failure) {
    gLogLevel = 1;

    InitMemPool();

    unsigned char* pBuffer = MemoryAlloc(SIZE_9);
    ASSERT_TRUE(pBuffer == 0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[9]), NUM_SIZE_9);
}

// ------------------------
TEST_F(TestMempool, Join_Memory) {
    gLogLevel = 0;

    InitMemPool();

    // 
    string str1 = "123456789012345678901234567890";
    string str2 = "abcdefghijklmnopqrstuvwxyz";
    unsigned char* pBuffer1 = MemoryAlloc(str1.length());
    unsigned char* pBuffer2 = MemoryAlloc(str2.length());
    memcpy(pBuffer1, str1.c_str(), str1.length());
    memcpy(pBuffer2, str2.c_str(), str2.length());
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 2);
    unsigned char* pJoinBuffer = MemoryJoin(pBuffer1, pBuffer2);
    ASSERT_TRUE(pJoinBuffer == pBuffer1);
    EXPECT_EQ(MemoryGetLength(pJoinBuffer), str1.length() + str2.length());
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    string expectStr = str1 + str2;
    string joinStr((char*)pJoinBuffer);
    EXPECT_EQ(expectStr, joinStr);
    printf("join string = %s\n", pJoinBuffer);
    MemoryFree(pJoinBuffer);

    //
    unsigned int length0 = SIZE_0 - MEM_NODE_SIZE;
    unsigned int length1 = SIZE_1 - MEM_NODE_SIZE - length0;
    pBuffer1 = MemoryAlloc(length0);
    pBuffer2 = MemoryAlloc(length1);
    memset(pBuffer1, 'a', length0);
    memset(pBuffer2, 'b', length1);
    unsigned char* pExpectBuffer = MemoryAlloc(length0 + length1);
    memcpy(pExpectBuffer, pBuffer1, length0);
    memcpy(pExpectBuffer + length0, pBuffer2, length1);

    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1 - 2);
    pJoinBuffer = MemoryJoin(pBuffer1, pBuffer2);
    ASSERT_TRUE(pJoinBuffer == pBuffer2);
    EXPECT_EQ(MemoryGetLength(pJoinBuffer), length0 + length1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1 - 2);
    EXPECT_EQ(memcmp(pExpectBuffer, pJoinBuffer, length0 + length1), 0);

    MemoryFree(pJoinBuffer);
    MemoryFree(pExpectBuffer);
    
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1);
}

