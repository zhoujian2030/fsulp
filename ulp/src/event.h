/*
 * event.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OS_LINUX
#include <pthread.h>

typedef struct {
    pthread_cond_t condition;
    pthread_mutex_t mutex;
    unsigned char setFlag;
} Event;

#else 

typedef struct {

} Event;

#endif

int EventInit(Event* pEvent);
void EventDeInit(Event* pEvent);
int EventWait(Event* pEvent);
int EventSend(Event* pEvent);


#ifdef __cplusplus
}
#endif

#endif
