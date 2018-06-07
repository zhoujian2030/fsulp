/*
 * lteTrigger.c
 *
 *  Created on: June 1, 2018
 *      Author: j.zhou
 */

#include <unistd.h>
#include "lteTrigger.h"
#include "lteLogger.h"
#include "thread.h"
#include "lteConfig.h"
#include "lteUlpMgr.h"

#define TASK_LTE_TRIGGER_PRIORITY    90

// ---------------------------------
void* LteTriggerEntryFunc(void* p)
{
    LOG_TRACE(ULP_LOGGER_NAME, "Entry\n");

    unsigned int usleepTime = gLteConfig.pollingInterval * 1000;

    while (1) {
        UlpOneMilliSecondIsr();
        usleep(usleepTime);
    }

    return 0;
}

// --------------------------------
void StartLteTrigger()
{
    ThreadHandle threadHandle;
    ThreadParams threadParams;
    threadParams.priority = TASK_LTE_TRIGGER_PRIORITY;
    threadParams.policy = RT_SCHED_RR;
    threadParams.stackSize = 0;
    ThreadCreate((void*)LteTriggerEntryFunc, &threadHandle, &threadParams);

    LOG_DBG(ULP_LOGGER_NAME, "Create LTE Trigger task, pollingInterval = %dms\n", gLteConfig.pollingInterval);

    void *status;
    int result = pthread_join(threadHandle, &status);
    if (0 != result) {
        LOG_ERROR(ULP_LOGGER_NAME, "Error. return code from pthread_join is %d\n", result);
    }
}
