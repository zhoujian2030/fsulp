/*
 * lteResCleaner.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#include "lteResCleaner.h"
#include "lteLogger.h"
#include "baseType.h"
#include "lteRlc.h"
#include "thread.h"
#include "event.h"

BOOL gIntializedFlag = FALSE;
Event gCleanupEvent;

extern List gRlcUeContextList;

void* ResCleanerEntryFunc(void* p);
void ExecuteCleanup();

#ifndef OS_LINUX
#define TASK_RESOURCE_CLEANER_PRIORITY		2
#define TASK_RESOURCE_CLEANER_STACK_SIZE	(32*1024)
#pragma DATA_SECTION(gTaskResCleanerStack, ".ulpata");
UInt8 gTaskResCleanerStack[TASK_RESOURCE_CLEANER_STACK_SIZE];
#endif

// ---------------------------------
void InitResCleaner(unsigned char startResCleanerFlag)
{
    if (startResCleanerFlag) {
        EventInit(&gCleanupEvent);
        gIntializedFlag = TRUE;

#ifdef OS_LINUX 
        ThreadHandle threadHandle;
        ThreadCreate((void*)ResCleanerEntryFunc, &threadHandle, 0);
        LOG_DBG(ULP_LOGGER_NAME, "[%s], Create resource cleaner task\n", __func__);
#else 
        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.stackSize = TASK_RESOURCE_CLEANER_STACK_SIZE;
        threadParams.stack = gTaskResCleanerStack;
        threadParams.priority = TASK_RESOURCE_CLEANER_PRIORITY;
        ThreadCreate((void*)ResCleanerEntryFunc, &threadHandle, &threadParams);
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
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], Entry\n", __func__);
    
    while (1) {
        EventWait(&gCleanupEvent);

        ExecuteCleanup();
    }
}

// ---------------------------------
void ExecuteCleanup()
{
    UInt32 rlcCtxCount = ListCount(&gRlcUeContextList);
    RlcUeContext* pRlcUeCtx;
    
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rlcCtxCount = %d\n", __func__, rlcCtxCount);

    if (rlcCtxCount > 0) {
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
        while (pRlcUeCtx != 0) {
            if (pRlcUeCtx->idleCount >= MAC_IDLE_COUNT) {
                LOG_DBG(ULP_LOGGER_NAME, "[%s], clean RLC UE context, rnti = %d\n", __func__, pRlcUeCtx->rnti);
                RlcDeleteUeContext(pRlcUeCtx);
            } else {
                RlcChangeUeContextCount(pRlcUeCtx, 1);
            }

            pRlcUeCtx = (RlcUeContext*)ListGetNextNode(&pRlcUeCtx->node);
        }
    }
}
