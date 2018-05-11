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
#include "baseType.h"

typedef struct {
    BOOL   imsiPresent;
    BOOL   mTmsiPresent;
    UInt16 spare;
    UInt32 mTmsi;
    UInt8  imsi[16]; // only 15 bytes data are valid
} RrcUeIdentity;

typedef struct {
    ListNode node;
    UInt16 rnti;
    RrcUeIdentity ueIdentity;
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
