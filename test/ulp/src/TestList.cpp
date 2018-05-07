/*
 * gTestList.cpp
 *
 *  Created on: Apr 26, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestList.h"
#include "list.h"
#include "thread.h"
#include "sync.h"

using namespace std;

extern unsigned int gLogLevel;

List gTestList;
SEM_LOCK gTestLock; 

// ------------------------
TEST_F(TestList, Basic_Push_Pop) {
    gLogLevel = 0;
    ListInit(&gTestList, 0);

    EXPECT_EQ(gTestList.mtFlag, 0);
    EXPECT_EQ((int)ListCount(&gTestList), 0);

    ListNode* pNode1 = new ListNode();
    ListPushNode(&gTestList, pNode1);
    EXPECT_EQ((int)ListCount(&gTestList), 1);

    ListNode* pNode2 = new ListNode();
    ListPushNode(&gTestList, pNode2);
    EXPECT_EQ((int)ListCount(&gTestList), 2);

    ListNode* pNode3 = ListPopNode(&gTestList);
    EXPECT_TRUE(pNode3 == pNode1);
    EXPECT_EQ((int)ListCount(&gTestList), 1);
    
    ListPushNode(&gTestList, pNode3);
    EXPECT_EQ((int)ListCount(&gTestList), 2);

    ListNode* pNode4 = ListPopNode(&gTestList);    
    EXPECT_TRUE(pNode4 == pNode2);
    EXPECT_EQ((int)ListCount(&gTestList), 1);

    ListNode* pNode5 = ListPopNode(&gTestList);    
    EXPECT_TRUE(pNode5 == pNode1);
    EXPECT_EQ((int)ListCount(&gTestList), 0);    

    ListNode* pNode6 = ListPopNode(&gTestList);    
    EXPECT_TRUE(pNode6 == 0);
    EXPECT_EQ((int)ListCount(&gTestList), 0);    

    ListDeInit(&gTestList);
}

// ------------------------
TEST_F(TestList, Get_And_Delete_List_Node) {
    gLogLevel = 0;
    ListInit(&gTestList, 0);
    EXPECT_EQ(gTestList.mtFlag, 0);
    ListNode* pNodeArray[10];
    unsigned int i;
    for(i=0; i<10; i++) {
        pNodeArray[i] = new ListNode();
        ListPushNode(&gTestList, pNodeArray[i]);
    }
    EXPECT_EQ((int)ListCount(&gTestList), 10);
    ListNode* pNode = ListGetFirstNode(&gTestList);
    EXPECT_TRUE(pNode == pNodeArray[0]);
    for (i=1; i<10; i++) {
        pNode = ListGetNextNode(pNode);
        EXPECT_TRUE(pNode == pNodeArray[i]);
    }    
    pNode = ListGetNextNode(pNode);
    EXPECT_TRUE(pNode == 0);

    for (i=0; i<9; i++) {
        ListDeleteNode(&gTestList, pNodeArray[i]);
        EXPECT_EQ(ListCount(&gTestList), 10-i-1);
        pNode = ListGetFirstNode(&gTestList);
        EXPECT_TRUE(pNode == pNodeArray[i+1]);
    }
    ListDeleteNode(&gTestList, pNode);
    EXPECT_EQ((int)ListCount(&gTestList), 0);
}

// ------------------
void* TestEntryFunc1(void* p)
{
    cout << "TestEntryFunc1 running" << endl;

    ListNode* pNode = 0;
    int count = 1000;
    while (1) {

        if (count) {
            pNode = new ListNode();
            ListPushNode(&gTestList, pNode);
            // cout << pthread_self() << " : ListPushNode ListCount = " << ListCount(&gTestList) << endl;
            count--;

            usleep(1);
            continue;
        }
        break;        
    }

    return 0;
}

// ------------------
void* TestEntryFunc2(void* p)
{
    cout << "TestEntryFunc2 running" << endl;

    ListNode* pNode = 0;
    while (1) {
        if (ListCount(&gTestList)) {
            pNode = ListPopNode(&gTestList);
            // cout << pthread_self() << " : ListPopNode ListCount = " << ListCount(&gTestList) << endl;
            EXPECT_TRUE(pNode != 0);
        }
        usleep(1);
    }

    return 0;
}

// ------------------
void* TestEntryFunc3(void* p)
{
    cout << "TestEntryFunc3 running" << endl;

    ListNode* pNode = 0;
    while (1) {
        // 2 thread pop the same list, event check ListCount, it could get null node
        if (ListCount(&gTestList)) {
            pNode = ListPopNode(&gTestList);
            // cout << pthread_self() << " : ListPopNode ListCount = " << ListCount(&gTestList) << endl;
        }
        usleep(1);
    }
    
    return 0;
}

// ------------------------
TEST_F(TestList, Multi_Thread_2_Thread_Push_1_Thread_Pop) {
    gLogLevel = 0;
    ListInit(&gTestList, 1);

    EXPECT_EQ(gTestList.mtFlag, 1);

    ThreadHandle threadHandle1;
    ThreadHandle threadHandle2;
    ThreadHandle threadHandle3;

    ThreadCreate((void*)TestEntryFunc1, &threadHandle1, 0);
    ThreadCreate((void*)TestEntryFunc1, &threadHandle2, 0);
    ThreadCreate((void*)TestEntryFunc2, &threadHandle3, 0);
    cout << "threadHandle1 = " << threadHandle1 << ", threadHandle2 = " << threadHandle2 << ", threadHandle3 = " << threadHandle3 << endl;

    sleep(5);
    EXPECT_EQ((int)ListCount(&gTestList), 0);

    ListDeInit(&gTestList);

    pthread_cancel(threadHandle1);
    pthread_cancel(threadHandle2);
    pthread_cancel(threadHandle3);
}

// ------------------------
TEST_F(TestList, Multi_Thread_1_Thread_Push_2_Thread_Pop) {
    gLogLevel = 0;
    ListInit(&gTestList, 1);

    EXPECT_EQ(gTestList.mtFlag, 1);

    ThreadHandle threadHandle1;
    ThreadHandle threadHandle2;
    ThreadHandle threadHandle3;

    ThreadCreate((void*)TestEntryFunc1, &threadHandle1, 0);
    ThreadCreate((void*)TestEntryFunc3, &threadHandle2, 0);
    ThreadCreate((void*)TestEntryFunc3, &threadHandle3, 0);
    cout << "threadHandle1 = " << threadHandle1 << ", threadHandle2 = " << threadHandle2 << ", threadHandle3 = " << threadHandle3 << endl;

    sleep(5);
    EXPECT_EQ((int)ListCount(&gTestList), 0);

    ListDeInit(&gTestList);    
    
    pthread_cancel(threadHandle1);
    pthread_cancel(threadHandle2);
    pthread_cancel(threadHandle3);
}

