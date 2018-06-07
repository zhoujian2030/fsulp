/*
 * lteResCleaner.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#include "lteResCleaner.h"
#include "lteLogger.h"
#include "lteCommon.h"
#include "lteRlc.h"
#include "thread.h"
#include "event.h"
#include "lteConfig.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gIntializedFlag, ".ulpdata");
#pragma DATA_SECTION(gCleanupEvent, ".ulpdata");
#pragma DATA_SECTION(gMaxIdleCount, ".ulpdata");
#endif
BOOL gIntializedFlag = FALSE;
Event gCleanupEvent;
unsigned int gMaxIdleCount = 10000; // 1ms * 10000 = 10s

extern List gRlcUeContextList;

void* ResCleanerEntryFunc(void* p);
void ExecuteCleanup();

#ifndef OS_LINUX
#define TASK_RESOURCE_CLEANER_PRIORITY		3
#define TASK_RESOURCE_CLEANER_STACK_SIZE	(32*1024)
#pragma DATA_SECTION(gTaskResCleanerStack, ".ulpdata");
UInt8 gTaskResCleanerStack[TASK_RESOURCE_CLEANER_STACK_SIZE];
#else
#define TASK_RESOURCE_CLEANER_PRIORITY		95
#endif

// ---------------------------------
void InitResCleaner(unsigned char startResCleanerFlag)
{
    if (startResCleanerFlag) {
        EventInit(&gCleanupEvent);
        gIntializedFlag = TRUE;

#ifdef OS_LINUX 
        if (gLteConfig.pollingInterval > 0) {
            gMaxIdleCount = gLteConfig.resCleanupTimer / gLteConfig.pollingInterval;
            if (gMaxIdleCount == 0) {
                gMaxIdleCount = 1000;
            }
        }

        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.priority = TASK_RESOURCE_CLEANER_PRIORITY;
        threadParams.policy = RT_SCHED_RR;
        threadParams.stackSize = 0;
        ThreadCreate((void*)ResCleanerEntryFunc, &threadHandle, &threadParams);

        LOG_DBG(ULP_LOGGER_NAME, "Create resource cleaner task, gMaxIdleCount = %d\n", gMaxIdleCount);
#else 
        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.stackSize = TASK_RESOURCE_CLEANER_STACK_SIZE;
        threadParams.stack = gTaskResCleanerStack;
        threadParams.priority = TASK_RESOURCE_CLEANER_PRIORITY;
        ThreadCreate((void*)ResCleanerEntryFunc, &threadHandle, &threadParams);

        LOG_DBG(ULP_LOGGER_NAME, "Create resource cleaner task, gMaxIdleCount = %d\n", gMaxIdleCount);
#endif
    }
}

// ---------------------------------
void NotifyResCleaner()
{
    if (gIntializedFlag) {
        EventSend(&gCleanupEvent);
    }
}

// ---------------------------------
void* ResCleanerEntryFunc(void* p)
{
    LOG_TRACE(ULP_LOGGER_NAME, "Entry\n");
    
    while (1) {
        EventWait(&gCleanupEvent);

        ExecuteCleanup();
    }

    return 0;
}

// ---------------------------------
void ExecuteCleanup()
{
    UInt32 rlcCtxCount = ListCount(&gRlcUeContextList);
    RlcUeContext *pRlcUeCtx, *pNextRlcUeCtx;
    
    // LOG_TRACE(ULP_LOGGER_NAME, "rlcCtxCount = %d\n", rlcCtxCount);

    if (rlcCtxCount > 0) {
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
        while (pRlcUeCtx != 0) {
            pNextRlcUeCtx = (RlcUeContext*)ListGetNextNode(&pRlcUeCtx->node);
             
            if (pRlcUeCtx->idleCount >= gMaxIdleCount) {
                LOG_DBG(ULP_LOGGER_NAME, "clean RLC UE context, rnti = %d\n", pRlcUeCtx->rnti);
                RlcDeleteUeContext(pRlcUeCtx);
            } else {
                RlcUpdateUeContextTime(pRlcUeCtx, 1);
            }

            pRlcUeCtx = pNextRlcUeCtx;
        }
    }
}
