/*
 * TestResCleaner.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_RES_CLEANER_H
#define TEST_RES_CLEANER_H

#include "TestSuite.h"


class TestResCleaner : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestResCleaner::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestResCleaner::TearDown()" << std::endl;
    }    
};


 #endif
 