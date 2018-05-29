/*
 * TestEvent.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestEvent.h"
#include "UlpTestCommon.h"
#include "thread.h"
#include "event.h"
#include "lteLogger.h"

Event gTestEvent;
int gTestCount = 0;

using namespace std;

// ------------------
void* TestEntryFunc(void* p)
{
    cout << "TestEntryFunc1 running" << endl;

    while (1) {

        EventWait(&gTestEvent);

        gTestCount++;
        // cout << "gTestCount = " << gTestCount << endl;      
    }

    return 0;
}


// -------------------------------
TEST_F(TestEvent, Basic_wait_Send) {
    LteLoggerSetLogLevel(1);
    gTestCount = 0;
    EventInit(&gTestEvent);
    ThreadHandle threadHandle;
    ThreadCreate((void*)TestEntryFunc, &threadHandle, 0);

    unsigned int i;
    int ExpectTestCount = 0;

    for (i=0; i<10; i++) {
        usleep(100);
        ASSERT_EQ(gTestCount, ExpectTestCount);
    }

    for (i=0; i<100; i++) {
        EventSend(&gTestEvent);
        usleep(50000);
        ExpectTestCount++;
        ASSERT_EQ(gTestCount, ExpectTestCount);
    }

    EventDeInit(&gTestEvent);
}
