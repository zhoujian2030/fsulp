/*
 * TestMempool.h
 *
 *  Created on: Apr 25, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_MEMPOOL
#define TEST_MEMPOOL

#include "TestSuite.h"


class TestMempool : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestMempool::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestMempool::TearDown()" << std::endl;
    }    
};


 #endif