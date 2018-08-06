/*
 * TestJson.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_JSON_H
#define TEST_JSON_H

#include "TestSuite.h"


class TestJson : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestJson::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestJson::TearDown()" << std::endl;
    }    
};

#endif

