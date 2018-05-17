/*
 * TestLogger.h
 *
 *  Created on: May 15, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include "TestSuite.h"


class TestLogger : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestLogger::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestLogger::TearDown()" << std::endl;
    }    
};


 #endif
 