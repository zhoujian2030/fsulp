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
#include "asn1.h"


// -------------------------
void InitUlpLayer(unsigned char standloneMacFlag, unsigned char startResCleanerFlag)
{
    KpiInit();
    InitLteLogger();
    InitMemPool();
    Asn1Init();
    InitMacLayer(standloneMacFlag);
    InitRlcLayer();
    InitRrcLayer();
    InitResCleaner(startResCleanerFlag);
}

// -------------------------
void UlpOneMilliSecondIsr()
{
#ifdef TI_DSP
	UpdateSystemTime();
#endif

	NotifyMacHandler();
	NotifyResCleaner();

#ifdef TI_DSP
	NotifyLogHandler();
#endif
}
