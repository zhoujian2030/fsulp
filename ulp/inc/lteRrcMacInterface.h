/*
 * lteRrcMacInterface.h
 *
 *  Created on: Aug 07, 2018
 *      Author: j.zh
 */

#ifndef LTE_RRC_MAC_INTERFACE_H
#define LTE_RRC_MAC_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lteCommon.h"

extern void MacUeCcchDataInd(unsigned short rnti, unsigned char* pData, unsigned short size, UlReportInfoList* pUlRptInfoList); 

#ifdef __cplusplus
}
#endif

#endif
