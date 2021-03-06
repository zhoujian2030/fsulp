/*
 * thread.c
 *
 *  Created on: Apr 27, 2018
 *      Author: j.zh
 */

#include "thread.h"
#include "lteLogger.h"

// -------------------------------
int ThreadCreate(void* pEntryFunc, ThreadHandle* pThreadHandle, ThreadParams* pThreadParams)
{
#ifdef OS_LINUX
    if (pThreadHandle == 0 || pEntryFunc == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer\n");
        return 1;
    }

    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pthread_attr_init failure\n");
        return 1;
    }
    
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    if (pThreadParams != 0) {
        if (pThreadParams->stackSize != 0) {
            pthread_attr_setstacksize(&attr, pThreadParams->stackSize);
        }

        if ((pThreadParams->priority > 0) && (pThreadParams->priority < 100)) {
            LOG_INFO(ULP_LOGGER_NAME, "Create RT thread, priority = %d, policy = %d\n", pThreadParams->priority, pThreadParams->policy);
            struct sched_param schedParam;
            schedParam.sched_priority = pThreadParams->priority;
            pthread_attr_setschedpolicy(&attr, pThreadParams->policy);
            pthread_attr_setschedparam(&attr, &schedParam);
        }
    }

    ret = pthread_create(pThreadHandle, &attr, pEntryFunc, 0);
    if (ret != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pthread_create failure\n");
        return 1;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "pthread_create success\n");

    return 0;

#else

    if (pThreadParams == 0 || pThreadHandle == 0 || pEntryFunc == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer\n");
        return 1;
    }

	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.priority  = pThreadParams->priority;
	taskParams.stackSize = pThreadParams->stackSize;
	taskParams.stack     = pThreadParams->stack;

	*pThreadHandle = Task_create((ti_sysbios_knl_Task_FuncPtr)pEntryFunc, &taskParams, 0);

    return 0;

#endif
}
