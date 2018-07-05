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
    pDpConfig->m_mobileIdDbName = "/tmp/eq5_mobile_id.db";
    pDpConfig->m_userDbname = "/tmp/eq5_user.db";

    system(("rm -rf " + pDpConfig->m_mobileIdDbName).c_str());
    system(("rm -rf " + pDpConfig->m_userDbname).c_str());
    system("cp /home/zj/nb-ulp/test/dpe/resource/eq5_mobile_id.db /tmp");
    system("cp /home/zj/nb-ulp/test/dpe/resource/eq5_user.db /tmp");

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

    char imsiArray[][16] = {
        "460029143962940",
        "460020267351682",
        "460029204530266",
        "460002409761129",
        "460002416299982",
        "460002371697632",
        "460000056590797",
        "460025020192690",
        "460003640399218",
        "460003332679587",
        "460001802574907"
    };
    unsigned int numImsi = sizeof(imsiArray) / sizeof(imsiArray[0]);

    for (unsigned int i=0; i<numImsi; i++) {  
        memcpy(dpReq.u.getLoginInfoReq.imsi, imsiArray[i], 15);
        pDpEngine->handleUserRequest(&dpReq);
    }   

}

// -------------------------------
TEST_F(TestDpEngine, API_handleUserRequest_Get_LoginInfo_By_Date)
{
    LoggerSetlevel(DEBUG);

    DpEngineConfig* pDpConfig = new DpEngineConfig();
    pDpConfig->m_mobileIdDbName = "/tmp/eq5_mobile_id.db";
    pDpConfig->m_userDbname = "/tmp/eq5_user.db";

    system(("rm -rf " + pDpConfig->m_mobileIdDbName).c_str());
    system(("rm -rf " + pDpConfig->m_userDbname).c_str());
    system("cp /home/zj/nb-ulp/test/dpe/resource/eq5_mobile_id.db /tmp");
    system("cp /home/zj/nb-ulp/test/dpe/resource/eq5_user.db /tmp");

    DpEngine* pDpEngine = new DpEngine(pDpConfig);

    DpRequest dpReq = {
        DP_REQ_GET_UE_LOGIN_INFO_BY_DATE,

        {
            {
                "",
                0,
                "2018-06-30 00:00:00",
                "2018-06-30 23:59:59"
            }
        }
    };
    pDpEngine->handleUserRequest(&dpReq);  
}