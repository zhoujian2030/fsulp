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
    unsigned int imsi;
    unsigned int mTmsi;
    unsigned int idResp;
    unsigned int rrcSetupCompl;
    unsigned int attachReq; 
    unsigned int tauReq;
    unsigned int servReq; 
    unsigned int extServReq; 
    unsigned int detachReq; 
    unsigned int lcIdArray[11];
    unsigned int activeRlcCtx;
    unsigned int semLock;
    unsigned int mem;
    unsigned int macInd;
    unsigned int rlcInd;
    unsigned int pdcpInd;
} LteKpi;

#ifdef OS_LINUX
typedef enum {
    KPI_NO_REPORT   = 0,
    KPI_REPORT_FILE = 1,
    KPI_REPORT_UDP  = 2
} KpiReportType;

#define MAX_KPI_FILE_NAME_LENGTH    128

typedef struct {
    unsigned int reportType;

    unsigned int reportFilePeriod;              // for report type 1
    char fileName[MAX_KPI_FILE_NAME_LENGTH];    

    unsigned short udpPort;                     // for report type 2
} LteKpiConfig;

extern void NotifyKpi();

#endif

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
