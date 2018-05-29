/*
 * lteConfig.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#ifndef LTE_CONFIG_H
#define LTE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "logger.h"

typedef struct {       
    int  pollingInterval;       // in micro second
    LoggerConfig logConfig;     // TRACE, DEBUG, INFO, WARNING, ERROR
} LteConfig;


extern LteConfig gLteConfig;

int IsStringEqual(char* src, char* dst);
void ParseConfig(char* configFileName);
void ShowConfig();

#ifdef __cplusplus
}
#endif

#endif 
