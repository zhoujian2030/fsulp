/*
 * lteKpi.c
 *
 *  Created on: May 02, 2018
 *      Author: J.ZH
 */

#include <string.h>
#include "lteKpi.h"
#include "lteLogger.h"
#include "mempool.h"
#include "lteRlc.h"
#include "lteRrc.h"
#ifdef OS_LINUX
#include "thread.h"
#include "event.h"
#include "lteConfig.h"
#include "file.h"
#endif

#ifndef OS_LINUX
#pragma DATA_SECTION(gLteKpi, ".ulpdata");
#endif
LteKpi gLteKpi = {0};

#define LTE_KPI


#ifdef OS_LINUX

#define KPI_TASK_PRIORITY   85
int gKpiNotifyCount = 1;

Event gKpiEvent;
int gKpiFileFd = -1;
int gDetailKpiFileFd = -1;

extern MemPool gMemPool;
extern MemPoolConfig gMemPoolConfig[MAX_NUM_POOL_SIZE];

static void ReportKpiToFile();

// ---------------------------------
void NotifyKpi()
{
    static int count = 0;
    count++;

    if (count == gKpiNotifyCount) {
        count = 0;
        EventSend(&gKpiEvent);
    }
}

// ---------------------------------
void* LteKpiEntryFunc(void* p)
{
    LOG_TRACE(ULP_LOGGER_NAME, "Entry\n");

    while (1) {
        if (gLteConfig.kpiConfig.reportType == KPI_REPORT_FILE ||
            gLteConfig.kpiConfig.reportType == KPI_REPORT_DETAIL_FILE) 
        {
            EventWait(&gKpiEvent);
            KpiRefresh();
            ReportKpiToFile();
        } else if (gLteConfig.kpiConfig.reportType == KPI_REPORT_UDP) {
            // TODO
            break;
        } else {
            break;
        }        
    }

    return 0;
}

// ---------------------------------
static void ReportKpiToFile()
{
    char kpiData[10240];
    int sumLength = 0;
    int varLength = 0;

    time_t timep;   
    struct tm *p;   
    time(&timep);   
    p = localtime(&timep);

    static LteKpi prevLteKpi = {0};

    varLength = sprintf(kpiData + sumLength, "Date: %04d-%02d-%02d %02d:%02d:%02d\n", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Name            Accumulate  Delta(%ds)\n", gLteConfig.kpiConfig.reportFilePeriod/1000);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "--------------------------------------\n");
    sumLength += varLength;
    
    varLength = sprintf(kpiData + sumLength, "IMSI            %10d  %8d\n", gLteKpi.imsi, gLteKpi.imsi - prevLteKpi.imsi);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "M-TMSI          %10d  %8d\n", gLteKpi.mTmsi, gLteKpi.mTmsi - prevLteKpi.mTmsi);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Identity Resp   %10d  %8d\n", gLteKpi.idResp, gLteKpi.idResp - prevLteKpi.idResp);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "RRC Setup Compl %10d  %8d\n", gLteKpi.rrcSetupCompl, gLteKpi.rrcSetupCompl - prevLteKpi.rrcSetupCompl);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "RRC Conn Req    %10d  %8d\n", gLteKpi.rrcConnReq, gLteKpi.rrcConnReq - prevLteKpi.rrcConnReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Attach Request  %10d  %8d\n", gLteKpi.attachReq, gLteKpi.attachReq - prevLteKpi.attachReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "TAU Request     %10d  %8d\n", gLteKpi.tauReq, gLteKpi.tauReq - prevLteKpi.tauReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Service Req     %10d  %8d\n", gLteKpi.servReq, gLteKpi.servReq - prevLteKpi.servReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Ext Service Req %10d  %8d\n", gLteKpi.extServReq, gLteKpi.extServReq - prevLteKpi.extServReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "Detach Request  %10d  %8d\n", gLteKpi.detachReq, gLteKpi.detachReq - prevLteKpi.detachReq);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "LcId0           %10d  %8d\n", gLteKpi.lcIdArray[0], gLteKpi.lcIdArray[0] - prevLteKpi.lcIdArray[0]);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "LcId1           %10d  %8d\n", gLteKpi.lcIdArray[1], gLteKpi.lcIdArray[1] - prevLteKpi.lcIdArray[1]);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "LcId2           %10d  %8d\n", gLteKpi.lcIdArray[2], gLteKpi.lcIdArray[2] - prevLteKpi.lcIdArray[2]);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "LcId3           %10d  %8d\n", gLteKpi.lcIdArray[3], gLteKpi.lcIdArray[3] - prevLteKpi.lcIdArray[3]);
    sumLength += varLength;
    varLength = sprintf(kpiData + sumLength, "LcId4           %10d  %8d\n", gLteKpi.lcIdArray[4], gLteKpi.lcIdArray[4] - prevLteKpi.lcIdArray[4]);
    sumLength += varLength;

    if (gLteConfig.kpiConfig.reportDebugInfoFlag) {
        varLength = sprintf(kpiData + sumLength, "Active Rlc Ctx  %10d  %8d\n", gLteKpi.activeRlcCtx, gLteKpi.activeRlcCtx - prevLteKpi.activeRlcCtx);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "Active Rrc Ctx  %10d  %8d\n", gLteKpi.activeRlcCtx, gLteKpi.activeRlcCtx - prevLteKpi.activeRrcCtx);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "Mem Used        %10d  %8d\n", gLteKpi.mem, gLteKpi.mem - prevLteKpi.mem);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "Crc Correct     %10d  %8d\n", gLteKpi.crcCorrect, gLteKpi.crcCorrect - prevLteKpi.crcCorrect);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "Crc Error       %10d  %8d\n", gLteKpi.crcError, gLteKpi.crcError - prevLteKpi.crcError);
        sumLength += varLength;
    }

#ifdef HEARTBEAT_DEBUG
    varLength = sprintf(kpiData + sumLength, "Heartbeat       %10d  %8d\n", gLteKpi.heartbeatResp, gLteKpi.heartbeatResp - prevLteKpi.heartbeatResp);
    sumLength += varLength;
#endif

#ifdef PHY_DEBUG
    unsigned int i;
    varLength = sprintf(kpiData + sumLength, "----------------------------\n");
    sumLength += varLength;
    for (i=0; i<19; i++) {
        if (gLteKpi.rrcSetupComplRbNum[i] > 0) {
            varLength = sprintf(kpiData + sumLength, "RRC Setup Compl RB %02d  %5d  %3d\n", i+1, 
                gLteKpi.rrcSetupComplRbNum[i], gLteKpi.rrcSetupComplRbNum[i] - prevLteKpi.rrcSetupComplRbNum[i]);
            sumLength += varLength;
        }
    }
    for (i=0; i<19; i++) {
        if (gLteKpi.idRespRbNum[i] > 0) {
            varLength = sprintf(kpiData + sumLength, "Identity Resp   RB %02d  %5d  %3d\n", i+1, 
                gLteKpi.idRespRbNum[i], gLteKpi.idRespRbNum[i] - prevLteKpi.idRespRbNum[i]);
            sumLength += varLength;
        }
    }
    varLength = sprintf(kpiData + sumLength, "----------------------------\n");
    sumLength += varLength;
#endif

    memcpy((void*)&prevLteKpi, (void*)&gLteKpi, sizeof(LteKpi));

    if (gLteConfig.kpiConfig.reportDebugInfoFlag) {
        UInt32 i;
        varLength = sprintf(kpiData + sumLength, "\nDetail Mempool Usage\n-----------------------\n");
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "Mem Size  Total  Used\n");
        sumLength += varLength;
        for (i=0; i<MAX_NUM_POOL_SIZE; i++) {   
            varLength = sprintf(kpiData + sumLength, "%8d  %5d  %4d\n", gMemPoolConfig[i].size, gMemPoolConfig[i].num, (gMemPoolConfig[i].num - ListCount(&gMemPool.pool[i])));
            sumLength += varLength;
        }
    }

    int writeBytes = 0;
    if (gKpiFileFd != -1) {        
        FileSeek(gKpiFileFd, F_SEEK_BEGIN);
        FileTruncate(gKpiFileFd, 0);
        FileWrite(gKpiFileFd, kpiData, sumLength, &writeBytes);
    }    

    // write detail data
    if (gDetailKpiFileFd != -1) {
        sumLength = 0;
    
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.imsi);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.mTmsi);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.idResp);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.rrcSetupCompl);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.attachReq);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.tauReq);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.servReq);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.extServReq);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.detachReq);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.lcIdArray[0]);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.lcIdArray[1]);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.lcIdArray[2]);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.lcIdArray[3]);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "%8d; ", gLteKpi.lcIdArray[4]);
        sumLength += varLength;
        varLength = sprintf(kpiData + sumLength, "\n");
        sumLength += varLength;
    
        FileWrite(gDetailKpiFileFd, kpiData, sumLength, &writeBytes);
    }  
}

#endif

// ----------------------------
void KpiInit() 
{
    memset((void*)&gLteKpi, 0, sizeof(LteKpi));

#ifdef OS_LINUX 
    if (gLteConfig.kpiConfig.reportType != KPI_NO_REPORT) {
        if (gLteConfig.kpiConfig.reportType == KPI_REPORT_FILE || 
            gLteConfig.kpiConfig.reportType == KPI_REPORT_DETAIL_FILE) 
        {
            gKpiNotifyCount = gLteConfig.kpiConfig.reportFilePeriod / gLteConfig.pollingInterval;
            if (gKpiNotifyCount < 1) {
                gKpiNotifyCount = 1;
            }
            EventInit(&gKpiEvent);
            gKpiFileFd = FileOpen(gLteConfig.kpiConfig.kpiFileName, FILE_CREATE, FILE_WRITE_ONLY);
            if (gKpiFileFd == -1) {
                LOG_ERROR(ULP_LOGGER_NAME, "Fail to create kpi file\n");
            }

            if (gLteConfig.kpiConfig.reportType == KPI_REPORT_DETAIL_FILE) {
                time_t timep;   
                struct tm *p;   
                char detailFileName[256] = {0};
                int offset = 0;
                time(&timep);   
                p = localtime(&timep);
                memcpy(detailFileName, gLteConfig.kpiConfig.detailFilePath, strlen(gLteConfig.kpiConfig.detailFilePath));
                offset = strlen(gLteConfig.kpiConfig.detailFilePath);
                if (detailFileName[offset - 1] != '/') {
                    detailFileName[offset] = '/';
                    offset++;
                }
                sprintf(detailFileName + offset, "eq5_kpi_%04d-%02d-%02d_%02d-%02d-%02d.txt",
                    (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
                LOG_DBG(ULP_LOGGER_NAME, "Create kpi detail file: %s\n", detailFileName);
                gDetailKpiFileFd = FileOpen(detailFileName, FILE_CREATE, FILE_WRITE_ONLY);
                if (gDetailKpiFileFd == -1) {
                    LOG_ERROR(ULP_LOGGER_NAME, "Fail to create kpi detail file: %s\n", detailFileName);
                } else {
                    char kpiName[] = "IMSI; M-TMSI; IdResp; RrcSetupCompl; AttachReq; TAUReq; ServReq; ExtServReq; DetachReq; LcId0; LcId1; LcId2; LcId3; LcId4;\n";
                    int writeBytes;
                    FileWrite(gDetailKpiFileFd, kpiName, sizeof(kpiName), &writeBytes);
                }
            }
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "TODO\n");
        }

        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.priority = KPI_TASK_PRIORITY;
        threadParams.policy = RT_SCHED_RR;
        threadParams.stackSize = 0;
        ThreadCreate((void*)LteKpiEntryFunc, &threadHandle, &threadParams);

        LOG_DBG(ULP_LOGGER_NAME, "Create Kpi task, reportFilePeriod = %d\n", gLteConfig.kpiConfig.reportFilePeriod);
    }
#endif
}

// ----------------------------
void ShowKpi()
{
    KpiRefresh();
    LOG_DBG(ULP_LOGGER_NAME, "semLock = %d, mem = %d, activeRlcCtx = %d, macInd = %d\n", 
        gLteKpi.semLock, gLteKpi.mem, gLteKpi.activeRlcCtx, gLteKpi.macInd);
    LOG_DBG(ULP_LOGGER_NAME, "lcId0 = %d, M-Tmsi = %d, imsi = %d, rlcInd = %d, pdcpInd = %d\n",     
        gLteKpi.lcIdArray[0], gLteKpi.mTmsi, gLteKpi.imsi, gLteKpi.rlcInd, gLteKpi.pdcpInd);
    LOG_DBG(ULP_LOGGER_NAME, "rrcSetupCompl = %d, idResp = %d, attachReq = %d,rlcInd = %d, pdcpInd = %d\n",     
        gLteKpi.rrcSetupCompl, gLteKpi.idResp, gLteKpi.attachReq, gLteKpi.rlcInd, gLteKpi.pdcpInd);
    LOG_DBG(ULP_LOGGER_NAME, "lcId1 = %d, lcId2 = %d, lcId3 = %d, lcId4 = %d, lcId5 = %d\n",     
        gLteKpi.lcIdArray[1], gLteKpi.lcIdArray[2], gLteKpi.lcIdArray[3], gLteKpi.lcIdArray[4], gLteKpi.lcIdArray[5]);
}

// ----------------------------
void KpiRefresh()
{
    gLteKpi.mem = MemGetNumMemBlock() - MemGetAvailableMemBlock();
    if (gLteKpi.mem < 0) {
        MemDumpStatus();
    }
    gLteKpi.activeRlcCtx = RlcGetUeContextCount();
    gLteKpi.activeRrcCtx = RrcGetUeContextCount();
}

// ----------------------------
void KpiCountSem(unsigned char createFlag)
{
#ifdef LTE_KPI
    // Could be problem if multi threads create sem
    if (createFlag) {
        gLteKpi.semLock++;
    } else {
        if (gLteKpi.semLock) {
            gLteKpi.semLock--;
        }
    }
#endif
}

