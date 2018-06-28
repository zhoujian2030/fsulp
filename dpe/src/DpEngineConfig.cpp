/*
 * DpEngineConfig.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
DpEngineConfig::DpEngineConfig() 
: m_mobileIdDbName("/tmp/eq5_mobile_id.db"),
  m_userInfoDbname("/tmp/eq5_user_info.db")
{

}

// -------------------------------
DpEngineConfig::~DpEngineConfig() 
{

}

