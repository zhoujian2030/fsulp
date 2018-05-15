/*
 * lteLogger.c
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#include "lteLogger.h"

#ifndef RUN_ON_STANDALONE_CORE
#ifndef OS_LINUX
#pragma DATA_SECTION(gLogLevel, ".ulpdata");
#endif
unsigned int gLogLevel = 2;
#endif


