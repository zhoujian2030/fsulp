/*
 * TestDpEngine.h
 *
 *  Created on: July 04, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_DP_ENGINE_H
#define TEST_DP_ENGINE_H

#include "TestSuite.h"


class TestDpEngine : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestDpEngine::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestDpEngine::TearDown()" << std::endl;
    }    
};


 #endif
 