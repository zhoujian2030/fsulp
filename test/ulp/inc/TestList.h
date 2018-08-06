/*
 * TestList.h
 *
 *  Created on: Apr 26, 2018
 *      Author: j.zh
 */

#ifndef TEST_LIST_H
#define TEST_LIST_H

#include "TestSuite.h"


class TestList : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestList::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestList::TearDown()" << std::endl;
    }    
};


 #endif