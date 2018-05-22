/*
 * lteKpi.h
 *
 *  Created on: May 02, 2018
 *      Author: J.ZH
 */

#ifndef LTE_KPI_H
#define LTE_KPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    unsigned int semLock;
    unsigned int mem;
    unsigned int activeRlcCtx;
    unsigned int lcIdArray[11];
    unsigned int macInd;
    unsigned int rlcInd;
    unsigned int pdcpInd;
    unsigned int rrcSetupCompl;
    unsigned int idResp;
    unsigned int attachReq; 
    unsigned int extServReq; 
    unsigned int detachReq; 
    unsigned int imsi;
    unsigned int mTmsi;
} LteKpi;

extern LteKpi gLteKpi;

extern void KpiInit();
extern void KpiShow();
extern void KpiRefresh();

extern void KpiCountSem(unsigned char createFlag);
extern void KpiCountRlcUeCtx(unsigned char createFlag);

#ifdef __cplusplus
}
#endif

#endif 
