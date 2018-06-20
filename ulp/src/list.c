/*
 * list.c
 *
 *  Created on: Apr 20, 2018
 *      Author: j.zh
 */

#include "list.h"
#include "lteLogger.h"
#include <stdlib.h>
#ifdef OS_LINUX
#include <execinfo.h>
#endif

void PushTail(List* pList, ListNode* pNode);
void PushHead(List *pList, ListNode *pNode);
ListNode* PopHead(List* pList);
unsigned int DeleteNode(List* pList, ListNode* pNode);
void PrintBackTrace(void);

// ----------------------------------
void ListInit(List* pList, unsigned char mtFlag) 
{
    if (pList != 0) {
        pList->count = 0;
        pList->node.next = 0;
        pList->node.prev = 0;
        pList->mtFlag = mtFlag;
        if (mtFlag) {
            SemInit(&pList->lock, 1);
        }
    }
}

// ----------------------------------
void ListDeInit(List* pList) 
{
    if (pList != 0) {
        if (pList->mtFlag) {
            SemDestroy(&pList->lock);
        }
        ListInit(pList, 0);
    }
}

// ----------------------------------
unsigned int ListCount(const List *pList) 
{
    if (pList != 0) {
        return pList->count;
    } 

    return 0;
}

// ----------------------------------
void ListInsertNodeToTail(List *pList, ListNode *pNode)
{
    if ((pList != 0) && (pNode != 0)) {
        if (pList->mtFlag) {
            SemWait(&pList->lock);
            PushTail(pList, pNode);       
            SemPost(&pList->lock);
        } else {
            PushTail(pList, pNode);      
        }
    }
}

// ----------------------------------
void ListInsertNodeToHead(List *pList, ListNode *pNode)
{
    if ((pList != 0) && (pNode != 0)) {
        if (pList->mtFlag) {
            SemWait(&pList->lock);
            PushHead(pList, pNode);
            SemPost(&pList->lock);
        } else {
        	PushHead(pList, pNode);
        }
    }
}

// ----------------------------------
void ListPushNode(List *pList, ListNode *pNode)
{
	ListInsertNodeToTail(pList, pNode);
}

// ----------------------------------
void ListPushNodeHead(List *pList, ListNode *pNode)
{
	ListInsertNodeToHead(pList, pNode);
}

// ----------------------------------
ListNode* ListPopNode(List *pList)
{
    ListNode* pNode = 0;    
    if (pList != 0)
    {
        if (pList->mtFlag) {
            SemWait(&(pList->lock));
            pNode = PopHead(pList);
            SemPost(&(pList->lock));
        } else {
            pNode = PopHead(pList);
        }
    }

    return pNode;
}

// ----------------------------------
void ListDeleteNode(List *pList, ListNode* pNode)
{
    ListNode* tmpNode = 0;
    unsigned int found = 0, i = 0;

    if ((pList != 0) && (pNode != 0)) {
        if(pList->count > 0) {
            if (pList->mtFlag) {
                SemWait(&(pList->lock));
            }

            tmpNode = pList->node.next;
            for (i=0; i<pList->count; i++) {
                if (tmpNode == pNode) {
                    found = 1;
                    break;
                }
                tmpNode = tmpNode->next;
            }

            if (found) {
                DeleteNode(pList, tmpNode);
            } else {
                LOG_ERROR(ULP_LOGGER_NAME, "pNode = %p is not found in pList = %p\n", pNode, pList);
            }

            if (pList->mtFlag) {
                SemPost(&(pList->lock));
            }
        }
    }
}

// ----------------------------------
ListNode* ListGetFirstNode(List *pList) 
{
    if (pList != 0) {
        return pList->node.next;
    }

    return 0;
}

// ----------------------------------
ListNode* ListGetNextNode(ListNode* pNode) 
{
    if (pNode != 0) {
        return pNode->next;
    }

    return 0;
}

// ----------------------------------
void PushTail(List* pList, ListNode* pNode)
{
    if(( pList->node.next == 0 )||(pList->count == 0)||( pList->node.prev == 0 )) {
        pList->node.next = pNode;
        pList->node.prev = pNode;
        pNode->next = 0;
        pNode->prev = 0;
        pList->count = 1;
    } else {
        pNode->next = 0;
        pNode->prev = pList->node.prev;
        pList->node.prev->next = pNode;
        pList->node.prev = pNode;
        pList->count++;
    }
}

// -----------------------------------
void PushHead(List *pList, ListNode *pNode)
{
	if(( pList->node.next == 0 )||(pList->count == 0)||( pList->node.prev == 0 )) {
		pList->node.next = pNode;
		pList->node.prev = pNode;
		pNode->next = 0;
		pNode->prev = 0;
		pList->count = 1;
	} else {
		pNode->prev = 0;
		pNode->next = pList->node.next;
		pList->node.next->prev = pNode;
		pList->node.next = pNode;
		pList->count++;
	}
}

// -----------------------------------
ListNode* PopHead(List* pList)
{
	ListNode* pNode = 0;

	pNode = pList->node.next;
	if (pNode) {
		DeleteNode( pList, pNode );
	}

	return pNode;
}

// -----------------------------------
unsigned int DeleteNode(List* pList, ListNode* pNode)
{
    unsigned int ret = 0;
#ifdef ISSUE_DEBUG
    static int failureCount = 0;
#endif

    if (pList->count == 0) {
        pList->node.next = 0;
        pList->node.prev = 0;
        ret = 1;
        LOG_ERROR(ULP_LOGGER_NAME, "list count is 0, pNode = %p\n", pNode);
    } else if(pList->count == 1) {
        if((pNode->prev != 0) || (pNode->next != 0) || (pList->node.prev != pNode) || (pList->node.next != pNode)) {
            LOG_ERROR(ULP_LOGGER_NAME, "invalid pNode = %p, pNode->prev = %p, pNode->next = %p, pList->node.prev = %p, pList->node.next = %p\n", 
                pNode, pNode->prev, pNode->next, pList->node.prev, pList->node.next);
            ret = 1;
        } else {
            pList->node.next = 0;
            pList->node.prev = 0;
            pList->count = 0;
            pNode->prev = 0;
            pNode->next = 0;
        }
    } else {
        if((pNode->next == 0) && (pNode->prev != 0)) {
            /** Last in List **/
            if((pList->node.prev != pNode) || (pList->node.prev->prev != pNode->prev)) {
                LOG_ERROR(ULP_LOGGER_NAME, "invalid pNode = %p, pNode->prev = %p, pList->node.prev = %p, pList->node.prev->prev = %p\n", 
                    pNode, pNode->prev, pList->node.prev, pList->node.prev->prev);
                ret = 1;
            } else {
                pList->node.prev = pNode->prev;
                pNode->prev->next = 0;
                pList->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        } else if((pNode->next != 0) && (pNode->prev == 0)) {
            /** First in List **/
            if((pList->node.next != pNode) || (pList->node.next->next != pNode->next)) {
                LOG_ERROR(ULP_LOGGER_NAME, "invalid pNode = %p, pNode->next = %p, pList->node.next = %p, pList->node.next->next = %p\n", 
                    pNode, pNode->next, pList->node.next, pList->node.next->next);
                ret = 1;
            } else {
                pList->node.next = pNode->next;
                pNode->next->prev = 0;
                pList->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        } else if((pNode->next != 0)&&(pNode->prev != 0)) {
            /** middle in List **/
            if((pNode->next->prev != pNode)||(pNode->prev->next != pNode)) {
                LOG_ERROR(ULP_LOGGER_NAME, "invalid pNode = %p, pNode->next->prev = %p, pNode->prev->next = %p\n", 
                    pNode, pNode->next->prev, pNode->prev->next);
                ret = 1;
            } else {
                pNode->next->prev = pNode->prev;
                pNode->prev->next = pNode->next;
                pList->count--;
                pNode->prev = 0;
                pNode->next = 0;
            }
        }
        else
        {
            LOG_ERROR(ULP_LOGGER_NAME, "invalid pNode = %p, pNode->next = %p, pNode->prev = %p, pList->count = %d\n", 
                pNode, pNode->next, pNode->prev, pList->count);

#ifdef ISSUE_DEBUG
            failureCount++;
            if (failureCount > 3) {
                exit(1); 
            }
#endif

            ret = 1;
        }
    }

    if (ret == 1) {
        PrintBackTrace();
    }

    return ret;
}

// -----------------------------------
#define MAX_FRAMES_IN_BT_ADD 10
void PrintBackTrace(void)
{
#ifdef OS_LINUX
    int i, nptrs;
    void *buffer[MAX_FRAMES_IN_BT_ADD];
    char **strings;

    nptrs = backtrace(buffer, MAX_FRAMES_IN_BT_ADD);
    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
    * would produce similar output to the following: */
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == 0) {
        perror("backtrace_symbols");
    }
    for (i = 0; i < nptrs; i++)
        LOG_ERROR(ULP_LOGGER_NAME, "%s\n", strings[i]);

    free(strings);
#endif
}
