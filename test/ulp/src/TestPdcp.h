/*
 * TestPdcp.h
 *
 *  Created on: May 04, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_PDCP_H
#define TEST_PDCP_H

#include "TestSuite.h"


class TestPdcp : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestPdcp::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestPdcp::TearDown()" << std::endl;
    }    
};


 #endif
 