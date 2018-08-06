/*
 * lteUlpOamInterface.h
 *
 *  Created on: June 08, 2018
 *      Author: j.zh
 */

#ifndef LTE_ULP_OAM_INTERFACE_H
#define LTE_ULP_OAM_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lteCommon.h"

#define LTE_ULP_DATA_IND_HEAD_LEHGTH        4
#define LTE_UE_ID_IND_MSG_HEAD_LEHGTH       4
#define LTE_OAM_DATA_REQ_HEAD_LEHGTH        4
#define LTE_UE_ESTAB_IND_MSG_HEAD_LEHGTH    4

#define MAX_NUM_UE_INFO_REPORT  32

#pragma pack(4)

typedef struct {
    UInt16 rnti;
    BOOL   imsiPresent;
    BOOL   mTmsiPresent;

    BOOL   detachFlag;
    UInt8  spare[3];

    UInt32 mTmsi;

    UInt8  imsi[16]; // only 15 bytes data are valid
} UeIdentity;

typedef struct {
    UInt64 timestamp;
    Int32  prbPower;
    UInt16 rnti;
    Int16  ta;
} UeEstablishInfo;

typedef struct {
    UInt32 count;
    UeEstablishInfo ueEstabInfoArray[MAX_NUM_UE_INFO_REPORT];
} UeEstablishIndMsg;

typedef struct {
    Int32 prbPower;
#ifdef PPC_LINUX
    char ip[16];
#endif
} TargetUeInfoMsg;

typedef struct {
    UInt32 count;
    UeIdentity ueIdentityArray[MAX_NUM_UE_INFO_REPORT];
} UeIdentityIndMsg;

typedef struct {
    UInt32 spare;
} HeartbeatRespMsg;

typedef enum {
    MSG_ULP_HEARTBEAT_REQ       = 0x80,
    MSG_ULP_HEARTBEAT_RESP      = 0x81,
    MSG_ULP_UE_IDENTITY_IND     = 0x82,
    MSG_ULP_TARGET_UE_INFO      = 0x83,

    MSG_ULP_UE_ESTABLISH_IND    = 0xa0
} E_UlpOamIndMsgType;

typedef struct {
    unsigned short msgType;
    unsigned short length;
    union {
        HeartbeatRespMsg    heartbeatResp;
        UeIdentityIndMsg    ueIdentityInd;
        TargetUeInfoMsg     targetUeInfoInd;
        UeEstablishIndMsg   ueEstablishInd;
    } u;
} LteUlpDataInd;

#define MAX_UDP_OAM_DATA_BUFFER     (sizeof(LteUlpDataInd) + 4)

typedef struct {
    unsigned short msgType;
    unsigned short length;
    char* data;
} LteOamDataReq;



#ifdef __cplusplus
}
#endif

#endif 
