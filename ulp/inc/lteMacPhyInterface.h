/*
 * lteMacPhyInterface.h
 *
 *  Created on: Apr 16, 2018
 *      Author: j.zh
 */

#ifndef LTE_MAC_PHY_INTERFACE_H
#define LTE_MAC_PHY_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lteCommon.h" 

extern void PhyUlDataInd(unsigned char* pBuffer, unsigned short length);

#ifdef MAC_PHY_INTF_NEW
typedef struct {                   
	unsigned short sfn;
	unsigned char  sf;
	unsigned char  numOfPDUs;
	unsigned char  buffer[1];
} RxUlSchInd; 
#define UL_SCH_IND_MSG_HEADER_LEN	4

typedef struct {
    unsigned short rnti;
    unsigned short length;
	unsigned char  buffer[1]; 
} UlSchPdu;
#define UL_SCH_PDU_HEADER_LENGTH	4

#else

#define  DL_CONFIG_REQUEST                      (0x80)
#define  UL_CONFIG_REQUEST                      (0x81)
#define  SUBFRAME_INDICATION                    (0x82)
#define  UL_DCI_REQUEST                         (0x83)
#define  TX_REQUEST                             (0x84)
#define  HARQ_INDICATION                        (0x85)
#define  RX_ULSCH_INDICATION                    (0x87)
#define  RACH_INDICATION                        (0x88)
#define  SRS_INDICATION                         (0x89)
#define  RX_SR_INDICATION                       (0x8a)
#define  RX_CQI_INDICATION                      (0x8b)
#define  RX_ULCRC_INDICATION                    (0x8c)
#define  MSG_INVALID                            (0xFF)

typedef struct  PhyHlMsgHead
{
	UInt32 mNum;
	UInt32 tLen;
	UInt32 sno;
	UInt8  attr;
	UInt8  common;
	UInt16 opc;

	UInt32 UeId;
	UInt16 cRnti;
	UInt8  cellId;
	UInt8  reserved1;
	UInt32 handle;
	UInt32 reserved2;
}S_PhyHlMsgHead;

typedef struct UlIndHead
{
	UInt16    sfn;
	UInt8     sf;
	UInt8     numOfPDUs;

}S_UlIndHead;

typedef struct RxUlschIndHeadPdu
{
	UInt32  UeId;

	UInt16   RNTI;
	UInt8    RNTIType;
	UInt8    mcs;

	UInt32   bitLen;

	UInt16   wordLen;
	UInt8    CRCFlag;
	Int8     SNR;

	Int16    TA;
	UInt8    rbNum;
	UInt8    Reserved;

	Int32	prbPower;
	Int32	puschRssi;

    // UInt32  PDU[Length]

}S_RxUlschIndHeadPdu;

typedef struct {
	UInt16 rnti;
	UInt16 length;
	UInt8* buffer;
} UlSchPdu;

#endif

#ifdef __cplusplus
}
#endif

#endif
