/*
 * TestResCleaner.cpp
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestResCleaner.h"
#include "lteResCleaner.h"
#include "UlpTestCommon.h"
#include "thread.h"
#include "event.h"
#include "lteLogger.h"
#include "lteUlpMgr.h"
#include "lteRlc.h"

using namespace std;

extern List gRlcUeContextList;

// -------------------------------
TEST_F(TestResCleaner, Basic_Test) {
    gLogLevel = 0;
    InitUlpLayer(0, 1);
    usleep(10);
    unsigned int i;
    unsigned short rntiStart = 101;
    RlcUeContext* pRlcCtxArray[10];
    for (i=0; i<10; i++) {
        pRlcCtxArray[i] = RlcCreateUeContext(rntiStart++);
        pRlcCtxArray[i]->idleCount = MAC_IDLE_COUNT - 10 + i;
        printf("rnti = %d, idleCount = %d\n", pRlcCtxArray[i]->rnti, pRlcCtxArray[i]->idleCount);
    }
    ASSERT_EQ(RlcGetUeContextCount(), 10);

    // 1st cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<10; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 9 + i);
    }
    ASSERT_EQ(RlcGetUeContextCount(), 10);

    // 2nd cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<9; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 8 + i);
    }
    pRlcCtxArray[9] = RlcGetUeContext(rntiStart + 9);
    ASSERT_TRUE(pRlcCtxArray[9] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 9);

    // 3rd cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<8; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 7 + i);
    }
    pRlcCtxArray[8] = RlcGetUeContext(rntiStart + 8);
    ASSERT_TRUE(pRlcCtxArray[8] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 8);

    // 4th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<7; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 6 + i);
    }
    pRlcCtxArray[7] = RlcGetUeContext(rntiStart + 7);
    ASSERT_TRUE(pRlcCtxArray[7] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 7);

    // 5th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<6; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 5 + i);
    }
    pRlcCtxArray[6] = RlcGetUeContext(rntiStart + 6);
    ASSERT_TRUE(pRlcCtxArray[6] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 6);

    // 6th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<5; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 4 + i);
    }
    pRlcCtxArray[5] = RlcGetUeContext(rntiStart + 5);
    ASSERT_TRUE(pRlcCtxArray[5] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 5);

    // 7th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<4; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 3 + i);
    }
    pRlcCtxArray[4] = RlcGetUeContext(rntiStart + 4);
    ASSERT_TRUE(pRlcCtxArray[4] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 4);

    // 8th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<3; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 2 + i);
    }
    pRlcCtxArray[3] = RlcGetUeContext(rntiStart + 3);
    ASSERT_TRUE(pRlcCtxArray[3] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 3);

    // 9th cleanup
    NotifyResCleaner();
    usleep(100000);
    for (i=0; i<2; i++) {
        ASSERT_EQ(pRlcCtxArray[i]->idleCount, MAC_IDLE_COUNT - 1 + i);
    }
    pRlcCtxArray[2] = RlcGetUeContext(rntiStart + 2);
    ASSERT_TRUE(pRlcCtxArray[2] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 2);

    // 10th cleanup
    NotifyResCleaner();
    usleep(100000);
    ASSERT_EQ((int)pRlcCtxArray[0]->idleCount, MAC_IDLE_COUNT);
    pRlcCtxArray[1] = RlcGetUeContext(rntiStart + 1);
    ASSERT_TRUE(pRlcCtxArray[1] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 1);

    // 11th cleanup
    NotifyResCleaner();
    usleep(100000);
    pRlcCtxArray[0] = RlcGetUeContext(rntiStart);
    ASSERT_TRUE(pRlcCtxArray[0] == 0);
    ASSERT_EQ(RlcGetUeContextCount(), 0);
}