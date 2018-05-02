/*
 * lteKpi.c
 *
 *  Created on: May 02, 2018
 *      Author: J.ZH
 */

#include "lteKpi.h"
#include "lteLogger.h"
#include "mempool.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gLteKpi, ".ulpata");
#endif
LteKpi gLteKpi = {0};

#define LTE_KPI

// ----------------------------
void ShowKpi()
{
    gLteKpi.mem = MemGetNumMemBlock() - MemGetAvailableMemBlock();
    LOG_DBG(ULP_LOGGER_NAME, "semLock = %d, mem = %d, activeRlcCtx = %d, macInd = %d\n", 
        gLteKpi.semLock, gLteKpi.mem, gLteKpi.activeRlcCtx, gLteKpi.macInd);
    LOG_DBG(ULP_LOGGER_NAME, "lcId0 = %d, idResp = %d, imsi = %d, rlcInd = %d, pdcpInd = %d\n",     
        gLteKpi.lcIdArray[0], gLteKpi.idResp, gLteKpi.imsi, gLteKpi.rlcInd, gLteKpi.pdcpInd);
    LOG_DBG(ULP_LOGGER_NAME, "lcId1 = %d, lcId2 = %d, lcId3 = %d, lcId4 = %d, lcId5 = %d\n",     
        gLteKpi.lcIdArray[1], gLteKpi.lcIdArray[2], gLteKpi.lcIdArray[3], gLteKpi.lcIdArray[4], gLteKpi.lcIdArray[5]);
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

// ----------------------------
void KpiCountRlcUeCtx(unsigned char createFlag)
{
    if (createFlag) {
        gLteKpi.activeRlcCtx++;
    } else {
        if (gLteKpi.activeRlcCtx) {
            gLteKpi.activeRlcCtx--;
        }
    }    
}
