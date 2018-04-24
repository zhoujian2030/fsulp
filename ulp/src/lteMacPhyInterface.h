/*
 * lteMacPhyInterface.h
 *
 *  Created on: Apr 16, 2018
 *      Author: j.zh
 */

#ifndef LTE_MAC_PHY_INTERFACE_H
#define LTE_MAC_PHY_INTERFACE_H

extern void MacUlSchDataInd(unsigned char* pBuffer, unsigned short length);

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
#define UL_SCH_PDU_HEADER_LENGTH	2


#endif
