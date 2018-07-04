/*
 * TestDpEngine.h
 *
 *  Created on: July 04, 2018
 *      Author: j.zh
 */

#include "TestDpEngine.h"
#include <iostream>
#include <string>
#include "DpEngine.h"
#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
TEST_F(TestDpEngine, API_handleUserRequest_Get_LoginInfo_By_IMSI_And_Date)
{
    LoggerSetlevel(DEBUG);

    DpEngineConfig* pDpConfig = new DpEngineConfig();
    pDpConfig->m_mobileIdDbName = "/tmp/dbtest.db";

    system(("rm -rf " + pDpConfig->m_mobileIdDbName).c_str());
    system("cp /home/zj/nb-ulp/test/dpe/resource/dbtest.db /tmp");

    DpEngine* pDpEngine = new DpEngine(pDpConfig);

    DpRequest dpReq = {
        DP_REQ_GET_UE_LOGIN_INFO_BY_IMSI_AND_DATE,

        {
            {
                "460002441352881",
                0,
                "2018-06-30 00:00:00",
                "2018-06-30 23:59:59"
            }
        }
    };

    pDpEngine->handleUserRequest(&dpReq);

}