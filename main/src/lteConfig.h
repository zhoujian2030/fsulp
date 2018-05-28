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

typedef struct {
    int loglevel;           // TRACE, DEBUG, INFO, WARNING, ERROR
    int pollingInterval;    // in micro second
} LteConfig;


extern LteConfig gLteConfig;

int IsStringEqual(char* src, char* dst);
void ConfigUpdate(char* configFileName);
void ConfigShow();

#ifdef __cplusplus
}
#endif

#endif 