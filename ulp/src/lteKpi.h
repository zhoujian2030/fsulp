/*
 * lteKpi.h
 *
 *  Created on: May 02, 2018
 *      Author: J.ZH
 */

#ifndef LTE_KPI_H
#define LTE_KPI_H

typedef struct 
{
    unsigned int semLock;
    unsigned int mem;
    unsigned int activeRlcCtx;
    unsigned int lcIdArray[11];
    unsigned int macInd;
    unsigned int rlcInd;
    unsigned int pdcpInd;
    unsigned int idResp;
    unsigned int imsi;
} LteKpi;

extern LteKpi gLteKpi;

extern void KpiCountSem(unsigned char createFlag);
extern void KpiCountRlcUeCtx(unsigned char createFlag);

#endif 
