/*
 * lteRlcMacInterface.h
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#ifndef LTE_RLC_MAC_INTERFACE_H
#define LTE_RLC_MAC_INTERFACE_H

#define MAX_NUM_UL_PKTS        106

typedef struct {
    unsigned char* rlcdataBuffer; 
    unsigned int  length;  
    unsigned char lcId;   
} RlcUlDataInfo;

typedef struct {
    RlcUlDataInfo rlcDataArray[MAX_NUM_UL_PKTS]; 
    unsigned char   numLCInfo; 
} RlcUlData;

typedef struct {
    unsigned short rnti;
    RlcUlData* rlcData;
} MacUeDataInd_t; 

extern void MacUeDataInd(MacUeDataInd_t* pMacDataInd);

#endif
