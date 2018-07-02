/*
 * lteTrigger.c
 *
 *  Created on: June 1, 2018
 *      Author: j.zhou
 */

#include <unistd.h>
#include "testTrigger.h"
#include "logger.h"
#include "thread.h"
#include "hbt.h"

// ---------------------------------
void* TestTriggerEntryFunc(void* p)
{
    unsigned int usleepTime = 3000 * 1000;

    while (1) {
        NotifyHeartbeatTester();
        usleep(usleepTime);
    }

    return 0;
}

// --------------------------------
void StartTestTrigger()
{
    ThreadHandle threadHandle;
    ThreadParams threadParams;
    threadParams.priority = 88;
    threadParams.policy = RT_SCHED_RR;
    threadParams.stackSize = 0;
    ThreadCreate((void*)TestTriggerEntryFunc, &threadHandle, &threadParams);

    LOG_MSG(HB_LOGGER_NAME, DEBUG, "Create Test Trigger task\n");

    void *status;
    int result = pthread_join(threadHandle, &status);
    if (0 != result) {
        LOG_MSG(HB_LOGGER_NAME, ERROR, "Error. return code from pthread_join is %d\n", result);
    }
}
