/*
 * event.c
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#include "event.h"
#include "lteLogger.h"

// ----------------------------------
int EventInit(Event* pEvent)
{
    if (pEvent == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pEvent is null\n", __func__); 
        return 1;
    }

#ifdef OS_LINUX
    int result = pthread_mutex_init(&pEvent->mutex, 0);
    if (result == 0) {
        result = pthread_cond_init(&pEvent->condition, 0);
        if (result != 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to init pthread_cond_t\n", __func__); 
            pthread_mutex_destroy(&pEvent->mutex);
            return 1;
        }
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to init pthread_mutex_t\n", __func__); 
        return 1;
    }

    pEvent->setFlag = 0;

#else
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
	semParams.mode 	= Semaphore_Mode_BINARY;
	pEvent->sem = Semaphore_create(0, &semParams, 0);
#endif

    return 0;
}

// ----------------------------------
void EventDeInit(Event* pEvent)
{
    if (pEvent == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pEvent is null\n", __func__); 
        return;
    }

#ifdef OS_LINUX
    LOG_TRACE(ULP_LOGGER_NAME, "pEvent = %p, setFlag = %d\n", pEvent, pEvent->setFlag); 

    pthread_mutex_destroy(&pEvent->mutex);
    pthread_cond_destroy(&pEvent->condition);

#else
    Semaphore_delete(&pEvent->sem);
#endif
}

// ----------------------------------
int EventWait(Event* pEvent)
{
    if (pEvent == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pEvent is null\n", __func__); 
        return 1;
    }

#ifdef OS_LINUX
    LOG_TRACE(ULP_LOGGER_NAME, "pEvent = %p, setFlag = %d\n", pEvent, pEvent->setFlag); 

    int result = pthread_mutex_lock(&pEvent->mutex);
    if (result != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to lock on mutex\n", __func__);
        return 1;
    }

    while (!pEvent->setFlag) {       
        result = pthread_cond_wait(&pEvent->condition, &pEvent->mutex);
        if (result != 0) {
            LOG_ERROR(ULP_LOGGER_NAME,  "fail to wait on condition\n", __func__);
            pthread_mutex_unlock(&pEvent->mutex);
            return 1;
        }
    }

    pEvent->setFlag = 0;
    pthread_mutex_unlock(&pEvent->mutex);   

#else
    Semaphore_pend(pEvent->sem, BIOS_WAIT_FOREVER);
#endif 

    return 0; 
}

// ----------------------------------
int EventSend(Event* pEvent)
{
    if (pEvent == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pEvent is null\n", __func__); 
        return 1;
    }

#ifdef OS_LINUX
    LOG_TRACE(ULP_LOGGER_NAME, "pEvent = %p\n", pEvent);
    
    int result = pthread_mutex_lock(&pEvent->mutex);
    if (result != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to lock on mutex\n", __func__);
        return 1;
    }        

    pEvent->setFlag = 1;
    result = pthread_cond_signal(&pEvent->condition);
    if (result != 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to signal\n", __func__);
        return 1;
    }

    pthread_mutex_unlock(&pEvent->mutex);

#else
    Semaphore_post(pEvent->sem);
#endif 

    return 0; 
}
