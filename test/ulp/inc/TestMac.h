/*
 * TestMac.h
 *
 *  Created on: May 08, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_MAC_H
#define TEST_MAC_H

#include "TestSuite.h"


class TestMac : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestMac::SetUp()" << std::endl;
        memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));
        memset((void*)&gMacUeCcchDataInd, 0, sizeof(MacCcchDataInd_Test_Array));
    }
    
    virtual void TearDown() {
        std::cout << "TestMac::TearDown()" << std::endl;
    }    
};


 #endif
 