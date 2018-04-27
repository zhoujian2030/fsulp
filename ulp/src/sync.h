/*
 * sync.h
 *
 *  Created on: Apr 19, 2018
 *      Author: j.zh
 */

#ifndef SYNC_H
#define SYNC_H

#ifdef OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#else
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OS_LINUX
typedef pthread_mutex_t SEM_LOCK;

#else
typedef Semaphore_Handle SEM_LOCK;

#endif

int SemInit(SEM_LOCK* pSem, unsigned int value);
int SemDestroy(SEM_LOCK* pSem);
int SemWait(SEM_LOCK* pSem);
int SemPost(SEM_LOCK* pSem);

#ifdef __cplusplus
}
#endif

#endif
