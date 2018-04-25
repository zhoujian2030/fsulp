/*
 * lteMac.h
 *
 *  Created on: Apr 16, 2018
 *      Author: j.zh
 */

#ifndef LTE_MAC_H
#define LTE_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

// void MacUlSchDataInd(unsigned char* pBuffer, unsigned short length);

#define MAX_NUM_CHANNELS      17

#define MAC_UL_CCCH_LCH     0
#define MAC_LCID_1          1
#define MAC_LCID_2          2
#define MAC_LCID_3          3
#define MAC_LCID_4          4
#define MAC_LCID_5          5
#define MAC_LCID_6          6
#define MAC_LCID_7          7
#define MAC_LCID_8          8
#define MAC_LCID_9          9
#define MAC_LCID_10         10
#define MAC_LCID_11         11
#define MAC_LCID_12         12
#define MAC_LCID_13         13
#define MAC_LCID_14         14
#define MAC_LCID_15         15
#define MAC_LCID_16         16
#define MAC_LCID_17         17
#define MAC_LCID_18         18
#define MAC_LCID_19         19
#define MAC_LCID_20         20
#define MAC_LCID_21         21
#define MAC_LCID_22         22
#define MAC_LCID_23         23
#define MAC_LCID_24         24
#define MAC_LCID_25         25
#define MAC_POWER_HEAD_LCH  26
#define MAC_CRNTI_LCH       27
#define MAC_TRUNCATED_BSR   28
#define MAC_S_BSR_LCH       29
#define MAC_L_BSR_LCH       30
#define MAC_L_BSR_LCH       30
#define MAC_PADDING_LCH     31
#define MAC_TA_LCH          29
#define MAC_DRX_LCH         30

#define MAC_UL_PDU_EXTN_FLAG          0x20  // 0010 0000
#define CRNTI_MAC_CE_LENGTH           2
#define LONG_BSR_MAC_CE_LENGTH        3
#define SHORT_BSR_MAC_CE_LENGTH       1
#define POWER_HEADROOM_MAC_CE_LENGTH  1


#define MAC_UL_PDU_RESERVE_BIT_FLAG   0xC0	// 1100 0000
#define MAC_UL_PDU_RESERVE_BIT1_FLAG  0x80	// 1000 0000
#define MAC_UL_PDU_RESERVE_BIT2_FLAG  0x40	// 0100 0000

typedef struct {
    unsigned char lchId; /*Logical channel ID*/
    unsigned int  length; /*Data length for this logical channel id*/
} DemuxDataBase;

#ifdef __cplusplus
}
#endif

#endif
