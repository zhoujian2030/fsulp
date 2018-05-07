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

#define MAC_IDLE_COUNT  10000

void InitResCleaner();
void NotifyResCleaner();

#ifdef __cplusplus
}
#endif

#endif
