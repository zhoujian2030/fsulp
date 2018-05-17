/*
 * TestLogger.cpp
 *
 *  Created on: May 15, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestLogger.h"
#include "lteLogger.h"

using namespace std;

// -------------------------------
TEST_F(TestLogger, WriteLog) {
    InitLogger();
    WriteLog(1, LOG_LEVEL_DBG, "%s", "1234567890");
    WriteLog(1, LOG_LEVEL_DBG, "%s", "xxxx");
    WriteLog(1, LOG_LEVEL_DBG, "%s", "aabbeeddee");

    WriteBuffer("123467sdfsdfsdfsdfsdfsdfsfsdfdsfd", 10);
}