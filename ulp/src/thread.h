/*
 * thread.h
 *
 *  Created on: Apr 27, 2018
 *      Author: j.zh
 */

#ifndef THREAD_H
#define THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#include <pthread.h>

typedef pthread_t ThreadHandle;

// typedef void* (*ENTRY_FUNC)(void*);

typedef struct {
    unsigned int priority;
    unsigned int stackSize;
} ThreadParams;


#else

#include <ti/sysbios/knl/Task.h>

typedef Task_Handle ThreadHandle;

typedef pthread_t ThreadHandle;

typedef struct {
    unsigned int priority;
    unsigned int stackSize;
    unsigned char* stack;
} ThreadParams;

#endif

// Create and start thread, return 1 if success, 0 if error
int ThreadCreate(void* pEntryFunc, ThreadHandle* pThreadHandle, ThreadParams* pThreadParams);


#ifdef __cplusplus
}
#endif

#endif
