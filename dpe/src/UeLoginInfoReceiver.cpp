/*
 * UeLoginInfoReceiver.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include "UeLoginInfoReceiver.h"
#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
UeLoginInfoReceiver::UeLoginInfoReceiver(DpEngineConfig* pDbeConfig) 
: m_pConfig(pDbeConfig)
{
    DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
}

// -------------------------------
UeLoginInfoReceiver::~UeLoginInfoReceiver() 
{

}
