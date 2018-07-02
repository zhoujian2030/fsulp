/*
 * DpEngine.cpp
 *
 *  Created on: June 30, 2018
 *      Author: j.zhou
 */

#include "DpEngine.h"
#include "dpeCommon.h"
#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
DpEngine::DpEngine(DpEngineConfig* pDpeConfig) 
: Thread("DP Engine"), m_pConfig(pDpeConfig)
{
    
}

// -------------------------------
DpEngine::~DpEngine() 
{

}

// -------------------------------
unsigned long DpEngine::run()
{
    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "starting...\n");

    while (true) {
        //
        Thread::sleep(1000);
    }

    return 0;
}
