/*
 * lteUlpMgr.cpp
 *
 *  Created on: May 02, 2018
 *      Author: j.zh
 */

#include "lteUlpMgr.h"
#include "lteLogger.h"
#include "mempool.h"
#include "lteRlc.h"
#include "lteResCleaner.h"
#include "lteKpi.h"
#include "lteMac.h"

// -------------------------
void InitUlpLayer(unsigned char standloneMacFlag, unsigned char startResCleanerFlag)
{
    KpiInit();
    InitMemPool();
    InitMacLayer(standloneMacFlag);
    InitRlcLayer();
    InitResCleaner(startResCleanerFlag);
}

// -------------------------
void UlpOneMilliSecondIsr()
{
	NotifyMacHandler();
	NotifyResCleaner();
}
