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
#include "lteKpi.h"

// -------------------------
void InitUlpLayer()
{
    InitMemPool();
    InitRlcLayer();
}

