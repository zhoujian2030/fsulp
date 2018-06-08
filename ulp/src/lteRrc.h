/*
 * lteRrc.h
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#ifndef LTE_RRC_H
#define LTE_RRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"
#include "lteUlpOamInterface.h"

typedef struct {
    ListNode node;
    UInt16 rnti;
    UeIdentity ueIdentity;
} RrcUeContext;

extern void InitRrcLayer();

extern void RrcUeDataInd(RrcUeContext* pRrcUeCtx);

extern RrcUeContext* RrcGetUeContext(UInt16 rnti);
extern RrcUeContext* RrcCreateUeContext(UInt16 rnti);
extern void RrcDeleteUeContext(RrcUeContext* pRrcUeCtx);
extern int RrcGetUeContextCount();

#ifdef __cplusplus
}
#endif

#endif
