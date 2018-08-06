/*
 * heartbeatTester.h
 *
 *  Created on: July 2, 2018
 *      Author: j.zhou
 */

#ifndef HEART_BEAT_TESTER_H
#define HEART_BEAT_TESTER_H

#ifdef __cplusplus
extern "C" {
#endif

#define HB_LOGGER_NAME  "HB"

void StartHeartbeatTester();
void NotifyHeartbeatTester();

#ifdef __cplusplus
}
#endif

#endif
