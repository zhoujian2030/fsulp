/*
 * lteLogger.c
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#include "lteLogger.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gLogLevel, ".ulpata");
#endif
unsigned int gLogLevel = 1;


