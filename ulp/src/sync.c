/*
 * sync.c
 *
 *  Created on: Apr 19, 2018
 *      Author: j.zh
 */

#include "sync.h"
#ifdef OS_LINUX
#include "CLogger.h"
#endif

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


#endif

