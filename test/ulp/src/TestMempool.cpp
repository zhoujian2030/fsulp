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

extern unsigned int gLogLevel;

// ------------------------
TEST_F(TestMempool, Allocate_Memory_Success) {
    gLogLevel = 0;
    InitMemPool();

    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[4]), NUM_SIZE_4);

    // allocated first memory from pool 0
    unsigned int allocatedSize = SIZE_0 - MEM_NODE_SIZE;
    unsigned char* buffer1 = MemAlloc(allocatedSize);
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
    unsigned char* buffer2 = MemAlloc(allocatedSize);
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
    unsigned char* buffer3 = MemAlloc(allocatedSize);
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
    EXPECT_EQ(MemGetNumMemBlock() - 3, MemGetAvailableMemBlock());
    MemFree(buffer1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    MemFree(buffer2);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    MemFree(buffer3);
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
        pBuffer = MemAlloc(size);
        ASSERT_TRUE(pBuffer != 0);
        EXPECT_EQ(ListCount(&gMemPool.pool[5]), NUM_SIZE_5 - i - 1);
    }
    EXPECT_EQ((int)ListCount(&gMemPool.pool[5]), 0);

    pBuffer = MemAlloc(size);
    ASSERT_TRUE(pBuffer != 0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[6]), NUM_SIZE_6 - 1);
}

// ------------------------
TEST_F(TestMempool, Allocate_Memory_Failure) {
    gLogLevel = 1;

    InitMemPool();

    unsigned char* pBuffer = MemAlloc(SIZE_9);
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
    unsigned char* pBuffer1 = MemAlloc(str1.length());
    unsigned char* pBuffer2 = MemAlloc(str2.length());
    memcpy(pBuffer1, str1.c_str(), str1.length());
    memcpy(pBuffer2, str2.c_str(), str2.length());
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 2);
    unsigned char* pJoinBuffer = MemJoin(pBuffer1, pBuffer2);
    ASSERT_TRUE(pJoinBuffer == pBuffer1);
    EXPECT_EQ(MemGetLength(pJoinBuffer), str1.length() + str2.length());
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    string expectStr = str1 + str2;
    char* tempBuff = new char[MemGetLength(pJoinBuffer) + 1];
    memcpy(tempBuff, pJoinBuffer, MemGetLength(pJoinBuffer));
    tempBuff[MemGetLength(pJoinBuffer)] = '\0';
    string joinStr(tempBuff);
    EXPECT_EQ(expectStr, joinStr);
    printf("join string = %s\n", pJoinBuffer);
    MemFree(pJoinBuffer);

    //
    unsigned int length0 = SIZE_0 - MEM_NODE_SIZE;
    unsigned int length1 = SIZE_1 - MEM_NODE_SIZE - length0;
    pBuffer1 = MemAlloc(length0);
    pBuffer2 = MemAlloc(length1);
    memset(pBuffer1, 'a', length0);
    memset(pBuffer2, 'b', length1);
    unsigned char* pExpectBuffer = MemAlloc(length0 + length1);
    memcpy(pExpectBuffer, pBuffer1, length0);
    memcpy(pExpectBuffer + length0, pBuffer2, length1);

    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0 - 1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1 - 2);
    pJoinBuffer = MemJoin(pBuffer1, pBuffer2);
    ASSERT_TRUE(pJoinBuffer == pBuffer2);
    EXPECT_EQ(MemGetLength(pJoinBuffer), length0 + length1);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[0]), NUM_SIZE_0);
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1 - 2);
    EXPECT_EQ(memcmp(pExpectBuffer, pJoinBuffer, length0 + length1), 0);

    MemFree(pJoinBuffer);
    MemFree(pExpectBuffer);
    
    EXPECT_EQ((int)ListCount(&gMemPool.pool[1]), NUM_SIZE_1);
}

// ------------------------
TEST_F(TestMempool, Mem_Remove) {
    gLogLevel = 0;

    InitMemPool();

    unsigned char origData[] = {
        0xb0, 0x03, 0x02, 0x26, 0x80, 0xf2, 0x4e, 0x80, 0x00, 0x00, 
        0x00, 0x00
    };
    unsigned char expectData[] = {
        0x02, 0x26, 0x80, 0xf2, 0x4e, 0x80, 0x00, 0x00, 0x00, 0x00
    };

    unsigned char* pBuffer = MemAlloc(sizeof(origData));
    memcpy(pBuffer, origData, sizeof(origData));
    EXPECT_EQ(MemRemove(pBuffer, 0, 2), 2);
    EXPECT_EQ(MemGetLength(pBuffer), sizeof(expectData));
    EXPECT_EQ(memcmp(expectData, pBuffer, sizeof(expectData)), 0);
}