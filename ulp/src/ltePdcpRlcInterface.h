/*
 * ltePdcpRlcInterface.h
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#ifndef LTE_PDCP_RLC_INTERFACE_H
#define LTE_PDCP_RLC_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

extern void RlcUeDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size);

#ifdef __cplusplus
}
#endif

#endif
