/*
 * TestUeInfoReceiver.h
 *
 *  Created on: July 09, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_UE_INFO_RECEIVER_H
#define TEST_UE_INFO_RECEIVER_H

#include "TestSuite.h"


class TestUeInfoReceiver : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestUeInfoReceiver::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestUeInfoReceiver::TearDown()" << std::endl;
    }    
};


 #endif
 