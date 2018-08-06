/*
 * lteRrcPdcpInterface.h
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#ifndef LTE_RRC_PDCP_INTERFACE_H
#define LTE_RRC_PDCP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lteCommon.h"

extern void PdcpUeSrbDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size, UlReportInfoList* pUlRptInfoList); 

#ifdef __cplusplus
}
#endif

#endif
