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

#ifdef OS_LINUX
#include <pthread.h>

typedef pthread_t ThreadHandle;

// real-time schedule policy
// default is SCHED_OTHER (not real-time)
typedef enum {
    RT_SCHED_RR     = SCHED_RR,
    RT_SCHED_FIFO   = SCHED_FIFO
} RTSchedPolicy;

typedef struct {
    unsigned int    priority;
    RTSchedPolicy   policy;
    unsigned int    stackSize;
} ThreadParams;


#else

#include <ti/sysbios/knl/Task.h>

typedef Task_Handle ThreadHandle;

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
