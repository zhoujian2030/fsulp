/*
 * lteUlpWorker.h
 *
 *  Created on: June 08, 2018
 *      Author: j.zh
 */

#ifndef LTE_ULP_WORKER_H
#define LTE_ULP_WORKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"
#include "lteCommon.h"

typedef struct {
    ListNode node;
    UInt32 length;
    UInt8* pBuffer;
} PhyDataIndNode;

void InitUlpWorker(unsigned char startUlpWorkerFlag);
void NotifyUlpWorker();

#ifdef __cplusplus
}
#endif

#endif 
