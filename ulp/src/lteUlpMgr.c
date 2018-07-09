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
#include "lteUlpWorker.h"
#include "target.h"

// -------------------------
void InitUlpLayer(unsigned char startUlpWorkerFlag, unsigned char startResCleanerFlag)
{
    InitLteLogger();
    KpiInit();
    InitMemPool();
    Asn1Init();
    InitMacLayer();
    InitRlcLayer();
    InitRrcLayer();
    TgtInit();
    InitUlpWorker(startUlpWorkerFlag);
    InitResCleaner(startResCleanerFlag);
}

// -------------------------
void UlpOneMilliSecondIsr()
{
#ifdef TI_DSP
	UpdateSystemTime();
#endif

	NotifyUlpWorker();
	NotifyResCleaner();

#ifdef OS_LINUX
    NotifyKpi();
#endif

#ifdef TI_DSP
	NotifyLogHandler();
#endif
}
