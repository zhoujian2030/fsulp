/*
 * sync.c
 *
 *  Created on: Apr 19, 2018
 *      Author: j.zh
 */

#include "sync.h"
#include "lteLogger.h"

#ifdef OS_LINUX
// --------------------------------
int SemInit(SEM_LOCK* pSem, unsigned int value) 
{
	if ( pthread_mutex_init(pSem, 0) == 0) {
        if ( value == 0 ) {
            if ( (pthread_mutex_lock(pSem)) == 0) {
                return 0;
            }
        } else if (value != 1 ) {
            LOG_ERROR(ULP_LOGGER_NAME, "[%s], not support value greater than 1\n", __func__);
        }
        return 0;
    } else {
        return 1;
    }
}

// --------------------------------
int SemDestroy(SEM_LOCK* pSem)
{
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
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], Semaphore_create failed\n", __func__);
        return 1;
    } else {
        return 0;
    }
}

// --------------------------------
int SemDestroy(SEM_LOCK* pSem)
{
	LOG_TRACE(ULP_LOGGER_NAME, "[%s], pSem = %p\n", __func__, pSem);
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "[%s], pSem null\n", __func__);
		return 0;
	}

	Semaphore_delete(pSem);
	return 1;
}

// --------------------------------
int SemWait(SEM_LOCK* pSem)
{
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "[%s], pSem null\n", __func__);
		return 0;
	}
	if(Semaphore_pend(*pSem, BIOS_WAIT_FOREVER)) {
		return 1;
	}
	else {
		return 0;
	}
}

// --------------------------------
int SemPost(SEM_LOCK* pSem)
{
	if (0 == *pSem) {
		LOG_ERROR(ULP_LOGGER_NAME, "[%s], pSem null\n", __func__);
		return 0;
	}

	Semaphore_post(*pSem);
	return 1;
}

#endif

