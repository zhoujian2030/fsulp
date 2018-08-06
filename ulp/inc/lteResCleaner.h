/*
 * lteResCleaner.h
 *
 *  Created on: May 07, 2018
 *      Author: j.zh
 */

#ifndef LTE_RES_CLEANER_H
#define LTE_RES_CLEANER_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int gMaxIdleCount;

void InitResCleaner(unsigned char startResCleanerFlag);
void NotifyResCleaner();

#ifdef __cplusplus
}
#endif

#endif
