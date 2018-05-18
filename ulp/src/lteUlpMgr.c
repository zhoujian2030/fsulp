/*
 * lteUlpMgr.cpp
 *
 *  Created on: May 02, 2018
 *      Author: j.zh
 */

#include "lteCommon.h"
#include "lteUlpMgr.h"
#include "lteLogger.h"
#include "mempool.h"
#include "lteRlc.h"
#include "lteResCleaner.h"
#include "lteKpi.h"
#include "lteMac.h"
#include "lteRrc.h"


// -------------------------
void InitUlpLayer(unsigned char standloneMacFlag, unsigned char startResCleanerFlag)
{
    KpiInit();
    InitLogger();
    InitMemPool();
    InitMacLayer(standloneMacFlag);
    InitRlcLayer();
    InitRrcLayer();
    InitResCleaner(startResCleanerFlag);
}

// -------------------------
void UlpOneMilliSecondIsr()
{
	UpdateSystemTime();

	NotifyMacHandler();
	NotifyResCleaner();
	NotifyLogHandler();
}
