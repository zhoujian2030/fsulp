/*
 * TestRlc.h
 *
 *  Created on: Apr 28, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_RLC
#define TEST_RLC

#include "TestSuite.h"


class TestRlc : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestRlc::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestRlc::TearDown()" << std::endl;
    }    
};


 #endif
 