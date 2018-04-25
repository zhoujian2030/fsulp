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
    }
    
    virtual void TearDown() {
        std::cout << "TestUlp::TearDown()" << std::endl;
    }    
};


 #endif