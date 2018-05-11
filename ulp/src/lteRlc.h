/*
 * lteRlc.h
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#ifndef LTE_RLC_H
#define LTE_RLC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"
#include "sync.h"

#define RLC_MAX_PDU_LENGTH 18844

#define SRB_0_LCID 0
#define SRB_1_LCID 1
#define SRB_2_LCID 2

#define MAX_LC_ID   11

#define RLC_FAILURE 0
#define RLC_SUCCESS 1

typedef struct {
    unsigned char dc;
    unsigned char rf;
    unsigned char p;
    unsigned char fi;
    unsigned char e;
    unsigned char lsf;
    unsigned short sn;
    unsigned short so;
    unsigned short soEnd;
} AmdHeader;

typedef enum {
    RS_FREE = 0 ,
    RS_IN_USE = 1 ,
    RS_READY = 2
} RingNodeStatus;

typedef enum {
    PDU_AM_COMPLETE = 0,
    PDU_AM_SEGMENT,
    PDU_AM_SEGMENT_COMPLETE
} RlcAmPduStatus; 

typedef enum {
    AM_PDU_MAP_SDU_FULL = 0 ,
    AM_PDU_MAP_SDU_START,
    AM_PDU_MAP_SDU_END,
    AM_PDU_MAP_SDU_MID
} RlcSduStatus; 

typedef struct {
    RingNodeStatus status;
    void* data;
} RingNode;

typedef struct {
    unsigned short size;
    RingNode rNodeArray[512];
} Ring;

typedef struct {
    RlcAmPduStatus status;
    List segList; 
} AmdPdu;


typedef struct {
    ListNode node;
    unsigned int soStart;
    unsigned int soEnd;
    Queue dfeQ; 
    unsigned char lsf;
} AmdPduSegment;

typedef struct {
    unsigned int size;
    unsigned char *pData;
} RlcAmBuffer;

typedef struct {
    ListNode node;
    RlcSduStatus status;
    RlcAmBuffer buffer;
} AmdDFE;

typedef struct {
    unsigned short sn;
    RlcAmBuffer rawSdu;
} RlcAmRawSdu;

typedef struct {
    unsigned short rnti;
    unsigned char lcId;
    Ring amdPduRing;

    RlcAmRawSdu rxRawSdu;
} RxAMEntity;

typedef struct {
    ListNode node;
    unsigned int idleCount;
    SEM_LOCK lockOfCount;
    unsigned short rnti;
    RxAMEntity* rxAMEntityArray[MAX_LC_ID]; 
} RlcUeContext;

// -------------------------------
extern void InitRlcLayer();

// extern List gRlcUeContextList;
extern RlcUeContext* RlcGetUeContext(unsigned short rnti);
extern RlcUeContext* RlcCreateUeContext(unsigned short rnti);
extern void RlcDeleteUeContext(RlcUeContext* pRlcUeCtx);
extern int RlcGetUeContextCount();
extern void RlcUpdateUeContextTime(RlcUeContext* pRlcUeCtx, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif

