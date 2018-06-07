/*
 * queue.h
 *
 *  Created on: June 5, 2018
 *      Author: j.zh
 */

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

typedef struct _Node_ QNode;
typedef struct _Queue_ Queue;

struct _Node_ {
    QNode *next;
    QNode *prev;
};

struct _Queue_ {
	QNode node;
	unsigned int count;
    pthread_mutex_t mutex;
} ; 

void QueueInit(Queue* pQueue);
void QueueDeInit(Queue* pQueue);

unsigned int QueueCount(const Queue *pQueue);

void QueuePushNode(Queue *pQueue, QNode *pNode);
void QueuePushNodeHead(Queue *pQueue, QNode *pNode);
QNode* QueuePopNode(Queue *pQueue);


#ifdef __cplusplus
}
#endif

#endif
