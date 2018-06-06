/*
 * list.h
 *
 *  Created on: Apr 19, 2018
 *      Author: j.zh
 */

#ifndef LIST_H
#define LIST_H

#include "sync.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ListNode_ ListNode;
typedef struct _List_ List;

struct _ListNode_ {
    ListNode *next;
    ListNode *prev;
};

struct _List_ {
	ListNode node;
	unsigned int count;
    SEM_LOCK lock;
    unsigned char mtFlag;   // multi-thread support flag
} ; 

void ListInit(List* pList, unsigned char mtFlag);
void ListDeInit(List* pList);

unsigned int ListCount(const List *pList);

void ListInsertNodeToTail(List *pList, ListNode *pNode);
void ListInsertNodeToHead(List *pList, ListNode *pNode);
void ListPushNode(List *pList, ListNode *pNode);
void ListPushNodeHead(List *pList, ListNode *pNode);
ListNode* ListPopNode(List *pList);
void ListDeleteNode(List *pList, ListNode* pNode);

ListNode* ListGetFirstNode(List *pList);
ListNode* ListGetNextNode(ListNode* pNode);

#ifdef __cplusplus
}
#endif

#endif
