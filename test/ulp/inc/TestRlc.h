/*
 * TestRlc.h
 *
 *  Created on: Apr 28, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_RLC_H
#define TEST_RLC_H

#include "TestSuite.h"


class TestRlc : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestRlc::SetUp()" << std::endl;
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
    }
    
    virtual void TearDown() {
        std::cout << "TestRlc::TearDown()" << std::endl;
    }    
};


 #endif
 