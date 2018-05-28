/*
 * TestSuite.h
 *
 *  Created on: June 03, 2016
 *      Author: j.zhou
 */
 
#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "gtest/gtest.h"
#include <iostream>

class TestSuite : public ::testing::Test {
protected:
    virtual void SetUp() {
        std::cout << "TestSuite::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestSuite::TearDown()" << std::endl;
    }
};

#endif
