/*
 * lteCommon.c
 *
 *  Created on: May 17, 2018
 *      Author: J.ZH
 */

#include "lteCommon.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gSystemTime, ".ulpdata");
#pragma DATA_SECTION(gSystemSfnSf, ".ulpdata");
#endif
SystemTime gSystemTime = {1970,01,01,0,0,0,0};
SystemSfnSf gSystemSfnSf = {0, 0};

// --------------------------
inline void UpdateSystemTime()
{
	volatile UInt32* pRadtSymcnReg = (UInt32*)0x01f48050;
	gSystemSfnSf.sfn = (UInt16)((*pRadtSymcnReg) & 0x000003FF);//0~1024

	pRadtSymcnReg = (UInt32*)0x01f4804c;
	gSystemSfnSf.sf = ((UInt8)(((*pRadtSymcnReg) >> 19) & 0x000000FF)) >> 1;

	gSystemTime.millisecond++;

	if(gSystemTime.millisecond >= 1000){
		gSystemTime.millisecond = 0;
		gSystemTime.second++;
	}

	if(gSystemTime.second >= 60){
		gSystemTime.second = 0;
		gSystemTime.minute++;
	}

	if(gSystemTime.minute >= 60){
		gSystemTime.minute = 0;
		gSystemTime.hour++;
	}

	if(gSystemTime.hour >= 24){
		gSystemTime.hour = 0;
		gSystemTime.day++;
	}

	if(gSystemTime.day >= 30){
		gSystemTime.day = 0;
		gSystemTime.month++;
	}
}

// --------------------------
inline void UpdateSfnSf(UInt16 sfn, UInt8 sf)
{
	gSystemSfnSf.sfn = sfn;
	gSystemSfnSf.sf = sf;
}

// --------------------------
inline void GetSfnAndSf(UInt16* pSfn, UInt8* pSf)
{
	*pSfn = gSystemSfnSf.sfn;
	*pSf = gSystemSfnSf.sf;
}

