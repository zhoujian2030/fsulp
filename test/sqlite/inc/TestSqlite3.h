/*
 * TestSqlite3.h
 *
 *  Created on: June 22, 2018
 *      Author: j.zh
 */
 
#ifndef TEST_SQLITE3_H
#define TEST_SQLITE3_H

#include "TestSuite.h"


class TestSqlite3 : public TestSuite {
protected:
    virtual void SetUp() {
        std::cout << "TestSqlite3::SetUp()" << std::endl;
    }
    
    virtual void TearDown() {
        std::cout << "TestSqlite3::TearDown()" << std::endl;
    }    
};


 #endif
 