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
    }
    
    virtual void TearDown() {
        std::cout << "TestMac::TearDown()" << std::endl;
    }    
};


 #endif
 