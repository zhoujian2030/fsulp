/*
 * lteUlpMgr.h
 *
 *  Created on: May 02, 2018
 *      Author: j.zh
 */

#ifndef LTE_ULP_MGR_H
#define LTE_ULP_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

void InitUlpLayer(unsigned char standloneMacFlag, unsigned char startResCleanerFlag);
void UlpOneMilliSecondIsr();


#ifdef __cplusplus
}
#endif

#endif 
