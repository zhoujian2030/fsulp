/*
 * lteRrcPdcpInterface.h
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#ifndef LTE_RRC_PDCP_INTERFACE_H
#define LTE_RRC_PDCP_INTERFACE_H

extern void PdcpUeSrbDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size); 

#endif