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
    unsigned int activeRrcCtx;
    unsigned int semLock;
    unsigned int mem;
    unsigned int crcCorrect;
    unsigned int crcError;
    unsigned int macInd;
    unsigned int rlcInd;
    unsigned int pdcpInd;
#if (defined HEARTBEAT_DEBUG) || (defined PHY_DEBUG)
    unsigned int heartbeatResp;
    unsigned int rrcSetupComplRbNum[10];
    unsigned int idRespRbNum[10];
#endif
} LteKpi;

#ifdef OS_LINUX
typedef enum {
    KPI_NO_REPORT           = 0,
    KPI_REPORT_FILE         = 1,
    KPI_REPORT_DETAIL_FILE  = 2, 
    KPI_REPORT_UDP          = 3
} KpiReportType;

#define MAX_KPI_FILE_NAME_LENGTH    128

typedef struct {
    unsigned int reportType;

    unsigned int reportFilePeriod;                  // for report type 1 & 2
    char kpiFileName[MAX_KPI_FILE_NAME_LENGTH];     // for report type 1 & 2
    char detailFilePath[MAX_KPI_FILE_NAME_LENGTH];  // for report type 2

    unsigned short udpPort;                         // for report type 3

    unsigned char reportDebugInfoFlag;              // 
} LteKpiConfig;

extern void NotifyKpi();

#endif

extern LteKpi gLteKpi;

extern void KpiInit();
extern void KpiShow();
extern void KpiRefresh();

extern void KpiCountSem(unsigned char createFlag);

#ifdef __cplusplus
}
#endif

#endif 
