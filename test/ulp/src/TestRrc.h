/*
 * TestRrc.h
 *
 *  Created on: May 05, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_RRC_H
#define TEST_RRC_H

#include "TestSuite.h"


class TestRrc : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestRrc::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestRrc::TearDown()" << std::endl;
    }    
};


 #endif
 