/*
 * TestDci0.h
 *
 *  Created on: Aug 3, 2018
 *      Author: j.zh
 */

#ifndef TEST_DCI0_H
#define TEST_DCI0_H

#include "TestSuite.h"


class TestDci0 : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestDci0::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestDci0::TearDown()" << std::endl;
    }    
};


 #endif