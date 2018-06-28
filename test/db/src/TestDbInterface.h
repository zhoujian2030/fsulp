/*
 * TestDbInterface.h
 *
 *  Created on: June 28, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_DB_INTERFACE_H
#define TEST_DB_INTERFACE_H

#include "TestSuite.h"


class TestDbInterface : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestDbInterface::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestDbInterface::TearDown()" << std::endl;
    }    
};


 #endif
 