/*
 * TestUlp.h
 *
 *  Created on: Apr 25, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_ULP
#define TEST_ULP

#include "TestSuite.h"


class TestUlp : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestUlp::SetUp()" << std::endl;
        memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));
        memset((void*)&gMacUeCcchDataInd, 0, sizeof(MacCcchDataInd_Test_Array));
        memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
        memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    }
    
    virtual void TearDown() {
        std::cout << "TestUlp::TearDown()" << std::endl;
    }    
};


 #endif
 