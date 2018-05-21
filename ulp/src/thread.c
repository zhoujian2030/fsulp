/*
 * thread.c
 *
 *  Created on: Apr 27, 2018
 *      Author: j.zh
 */

#include "thread.h"
#ifdef OS_LINUX
#include "CLogger.h"
#else
#include "lteLogger.h"
#endif

// -------------------------------
int ThreadCreate(void* pEntryFunc, ThreadHandle* pThreadHandle, ThreadParams* pThreadParams)
{
#ifdef OS_LINUX
    if (pThreadHandle == 0 || pEntryFunc == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer\n", __func__);
        return 1;
    }

    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pthread_attr_init failure\n", __func__);
        return 1;
    }
    
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    if (pThreadParams != 0) {
        if (pThreadParams->stackSize != 0) {
            pthread_attr_setstacksize(&attr, pThreadParams->stackSize);
        }

        // TODO set priority
    }

    ret = pthread_create(pThreadHandle, &attr, pEntryFunc, 0);
    if (ret != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pthread_create failure\n", __func__);
        return 1;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "pthread_create success\n", __func__);

    return 0;

#else

    if (pThreadParams == 0 || pThreadHandle == 0 || pEntryFunc == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer\n", __func__);
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
