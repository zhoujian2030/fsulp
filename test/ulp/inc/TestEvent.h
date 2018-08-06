/*
 * TestEvent.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_EVENT_H
#define TEST_EVENT_H

#include "TestSuite.h"


class TestEvent : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestEvent::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestEvent::TearDown()" << std::endl;
    }    
};


 #endif
 