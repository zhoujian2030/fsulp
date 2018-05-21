/*
 * sync.c
 *
 *  Created on: Apr 19, 2018
 *      Author: j.zh
 */

#include "sync.h"
#include "lteLogger.h"
#include "lteKpi.h"

#ifdef OS_LINUX
// --------------------------------
int SemInit(SEM_LOCK* pSem, unsigned int value) 
{
	if ( pthread_mutex_init(pSem, 0) == 0) {
        if ( value == 0 ) {
            if ( (pthread_mutex_lock(pSem)) == 0) {
				KpiCountSem(1);
                return 0;
            }
        } else if (value != 1 ) {
            LOG_ERROR(ULP_LOGGER_NAME, "not support value greater than 1\n");
        }
		KpiCountSem(1);
        return 0;
    } else {
        return 1;
    }
}

// --------------------------------
int SemDestroy(SEM_LOCK* pSem)
{
	KpiCountSem(0);
    return pthread_mutex_destroy(pSem);      
}

// --------------------------------
int SemWait(SEM_LOCK* pSem) 
{
    return pthread_mutex_lock(pSem);
}

// --------------------------------
int SemPost(SEM_LOCK* pSem)
{
    return pthread_mutex_unlock(pSem);
}

#else
// --------------------------------
int SemInit(SEM_LOCK* pSem, unsigned int value)
{
	ti_sysbios_knl_Semaphore_Params semParams;
	semParams.mode 	= ti_sysbios_knl_Semaphore_Mode_BINARY;
	Semaphore_Params_init(&semParams);
	*pSem = Semaphore_create(value, &semParams, NULL);
    if (*pSem == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "Semaphore_create failed\n");
        return 1;
    } else {
		KpiCountSem(1);
        return 0;
    }
}

// --------------------------------
int SemDestroy(SEM_LOCK* pSem)
{
	LOG_TRACE(ULP_LOGGER_NAME, "pSem = %p\n", pSem);
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "pSem null\n");
		return 1;
	}
	KpiCountSem(0);
	Semaphore_delete(pSem);
	return 0;
}

// --------------------------------
int SemWait(SEM_LOCK* pSem)
{
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "pSem null\n");
		return 1;
	}
	if(Semaphore_pend(*pSem, BIOS_WAIT_FOREVER)) {
		return 0;
	} else {
		return 1;
	}
}

// --------------------------------
int SemPost(SEM_LOCK* pSem)
{
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "pSem null\n");
		return 1;
	}

	Semaphore_post(*pSem);
	return 0;
}

#endif

