/*
 * queue.c
 *
 *  Created on: June 5, 2018
 *      Author: j.zh
 */

#include "queue.h"
#include "logger.h"

static void PushTail(Queue* pQueue, QNode* pNode);
static void PushHead(Queue* pQueue, QNode* pNode);
static QNode* PopHead(Queue* pQueue);
static unsigned int DeleteNode(Queue* pQueue, QNode* pNode);

// ----------------------------------
void QueueInit(Queue* pQueue) 
{
    if (pQueue != 0) {
        pQueue->count = 0;
        pQueue->node.next = 0;
        pQueue->node.prev = 0;
        pthread_mutex_init(&pQueue->mutex, 0);
        // printf("QueueInit success\n");
    }
}

// ----------------------------------
void QueueDeInit(Queue* pQueue) 
{
    if (pQueue != 0) {
        pQueue->count = 0;
        pQueue->node.next = 0;
        pQueue->node.prev = 0;
        pthread_mutex_destroy(&pQueue->mutex);   
    }
}

// ----------------------------------
unsigned int QueueCount(const Queue *pQueue) 
{
    if (pQueue != 0) {
        return pQueue->count;
    } 

    return 0;
}

// ----------------------------------
void QueuePushNode(Queue *pQueue, QNode *pNode)
{
    if ((pQueue != 0) && (pNode != 0)) {
        pthread_mutex_lock(&pQueue->mutex);
        PushTail(pQueue, pNode);       
        pthread_mutex_unlock(&pQueue->mutex);
    }
}

// ----------------------------------
void QueuePushNodeHead(Queue *pQueue, QNode *pNode)
{
    if ((pQueue != 0) && (pNode != 0)) {
        pthread_mutex_lock(&pQueue->mutex);
        PushHead(pQueue, pNode);
        pthread_mutex_unlock(&pQueue->mutex);
    }
}

// ----------------------------------
QNode* QueuePopNode(Queue *pQueue)
{
    QNode* pNode = 0;    
    if (pQueue != 0)
    {
        pthread_mutex_lock(&pQueue->mutex);
        pNode = PopHead(pQueue);
        pthread_mutex_unlock(&pQueue->mutex);
    }

    return pNode;
}

// ----------------------------------
static void PushTail(Queue* pQueue, QNode* pNode)
{
    if(( pQueue->node.next == 0 )||(pQueue->count == 0)||( pQueue->node.prev == 0 )) {
        pQueue->node.next = pNode;
        pQueue->node.prev = pNode;
        pNode->next = 0;
        pNode->prev = 0;
        pQueue->count = 1;
    } else {
        pNode->next = 0;
        pNode->prev = pQueue->node.prev;
        pQueue->node.prev->next = pNode;
        pQueue->node.prev = pNode;
        pQueue->count++;
    }
}

// -----------------------------------
static void PushHead(Queue* pQueue, QNode* pNode)
{
	if(( pQueue->node.next == 0 )||(pQueue->count == 0)||( pQueue->node.prev == 0 )) {
		pQueue->node.next = pNode;
		pQueue->node.prev = pNode;
		pNode->next = 0;
		pNode->prev = 0;
		pQueue->count = 1;
	} else {
		pNode->prev = 0;
		pNode->next = pQueue->node.next;
		pQueue->node.next->prev = pNode;
		pQueue->node.next = pNode;
		pQueue->count++;
	}
}

// -----------------------------------
static QNode* PopHead(Queue* pQueue)
{
	QNode* pNode = 0;

	pNode = pQueue->node.next;
	if (pNode) {
		DeleteNode( pQueue, pNode );
	}

	return pNode;
}

// -----------------------------------
static unsigned int DeleteNode(Queue* pQueue, QNode* pNode)
{
    unsigned int ret = 0;

    if (pQueue->count == 0) {
        pQueue->node.next = 0;
        pQueue->node.prev = 0;
        ret = 1;
        printf("ERROR! queue count is 0, pNode = %p\n", pNode);
    } else if(pQueue->count == 1) {
        if((pNode->prev != 0) || (pNode->next != 0) || (pQueue->node.prev != pNode) || (pQueue->node.next != pNode)) {
            printf("ERROR! invalid pNode = %p, pNode->prev = %p, pNode->next = %p, pQueue->node.prev = %p, pQueue->node.next = %p\n", 
                pNode, pNode->prev, pNode->next, pQueue->node.prev, pQueue->node.next);
            ret = 1;
        } else {
            pQueue->node.next = 0;
            pQueue->node.prev = 0;
            pQueue->count = 0;
            pNode->prev = 0;
            pNode->next = 0;
        }
    } else {
        if((pNode->next == 0) && (pNode->prev != 0)) {
            /** Last in queue **/
            if((pQueue->node.prev != pNode) || (pQueue->node.prev->prev != pNode->prev)) {
                printf("ERROR! invalid pNode = %p, pNode->prev = %p, pQueue->node.prev = %p, pQueue->node.prev->prev = %p\n", 
                    pNode, pNode->prev, pQueue->node.prev, pQueue->node.prev->prev);
                ret = 1;
            } else {
                pQueue->node.prev = pNode->prev;
                pNode->prev->next = 0;
                pQueue->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        } else if((pNode->next != 0) && (pNode->prev == 0)) {
            /** First in queue **/
            if((pQueue->node.next != pNode) || (pQueue->node.next->next != pNode->next)) {
                printf("ERROR! invalid pNode = %p, pNode->next = %p, pQueue->node.next = %p, pQueue->node.next->next = %p\n", 
                    pNode, pNode->next, pQueue->node.next, pQueue->node.next->next);
                ret = 1;
            } else {
                pQueue->node.next = pNode->next;
                pNode->next->prev = 0;
                pQueue->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        } else if((pNode->next != 0)&&(pNode->prev != 0)) {
            /** middle in queue **/
            if((pNode->next->prev != pNode)||(pNode->prev->next != pNode)) {
                printf("ERROR! invalid pNode = %p, pNode->next->prev = %p, pNode->prev->next = %p\n", 
                    pNode, pNode->next->prev, pNode->prev->next);
                ret = 1;
            } else {
                pNode->next->prev = pNode->prev;
                pNode->prev->next = pNode->next;
                pQueue->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        }
        else
        {
            printf("ERROR! invalid pNode = %p, pNode->next = %p, pNode->prev = %p, pQueue->count = %d\n", 
                pNode, pNode->next, pNode->prev, pQueue->count);
            ret = 1;
        }
    }

    return ret;
}
