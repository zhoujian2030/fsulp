/*
 * TestSocket.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_SOCKET_H
#define TEST_SOCKET_H

#include "TestSuite.h"


class TestSocket : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestSocket::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestSocket::TearDown()" << std::endl;
    }    
};

#endif

