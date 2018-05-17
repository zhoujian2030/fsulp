/*
 * messaging.h
 *
 *  Created on: May 16, 2018
 *      Author: J.ZH
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include "lteCommon.h"

#ifdef RUN_ON_STANDALONE_CORE
#include <xdc/std.h>
#include "drivers/multi_core/struct_qmss.h"
#include "drivers/multi_core/global.h"

//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_PHY_DATA            (Uint8)0
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_PHY_CFG             (Uint8)1
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_L3_DATA_OR_CFG      (Uint8)2
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_CMAC_BUF_REP        (Uint8)3
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_CMAC_MAC_CE         (Uint8)4
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_OAM_LOG             (Uint8)5
//#define	QMSS_TX_FREE_HAND_LAYER2D_TO_OAM_DATA_OR_REPLY   (Uint8)6
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_LAYER2D_HARQ_ACK    (Uint8)7
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_LAYER2D_SCH_RESULT  (Uint8)8
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_L3_CFG_RSP          (Uint8)9
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_PHY_CFG             (Uint8)10
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_PHY_DCI             (Uint8)11
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_OAM_LOG             (Uint8)12
//#define QMSS_TX_FREE_HAND_LAYER2C_TO_OAM_CFG_RSP         (Uint8)13
//#define QMSS_TX_FREE_HAND_PHY_TO_LAYER2D_DATAUP          (Uint8)14
//#define QMSS_TX_FREE_HAND_PHY_TO_LAYER2D_ERRIND          (Uint8)15
//#define QMSS_TX_FREE_HAND_PHY_TO_LAYER2C_CQIUP           (Uint8)16
//#define QMSS_TX_FREE_HAND_PHY_TO_LAYER2C_REPLY           (Uint8)17
//#define QMSS_TX_FREE_HAND_PHY_TO_OAM_DATA_OR_REPLY       (Uint8)18
//#define	QMSS_TX_FREE_HAND_MAX                            (Uint8)19
//
//#define	QMSS_TX_HAND_LAYER2D_TO_OTHER_DATA               (Uint8)0
//#define	QMSS_TX_HAND_LAYER2D_TO_OTHER_LOG                (Uint8)1
//#define	QMSS_TX_HAND_LAYER2C_TO_OTHER_DATA               (Uint8)2
//#define	QMSS_TX_HAND_LAYER2C_TO_OTHER_LOG                (Uint8)3
//#define	QMSS_TX_HAND_PHY_TO_OTHER                        (Uint8)4
//#define	QMSS_TX_HAND_MAX                                 (Uint8)5
//
//#define	QMSS_RX_HAND_LAYER2D_FROM_CMAC_SCH_RESULT        (Uint8)0
//#define	QMSS_RX_HAND_LAYER2D_FROM_CMAC_CMAC_HARQ_ACK     (Uint8)1
//#define	QMSS_RX_HAND_LAYER2D_FROM_L1_DATA                (Uint8)2
//#define	QMSS_RX_HAND_LAYER2D_FROM_L1_ERRINDICATION       (Uint8)3
//#define	QMSS_RX_HAND_LAYER2D_FROM_L3_DATA_OR_CONFIG      (Uint8)4
//#define	QMSS_RX_HAND_LAYER2D_FROM_L3_OAM_CONFIG          (Uint8)5
//#define	QMSS_RX_HAND_LAYER2C_FROM_PHY_CFG_RSP            (Uint8)6
//#define	QMSS_RX_HAND_LAYER2C_FROM_PHY_UL_CQI             (Uint8)7
//#define	QMSS_RX_HAND_LAYER2C_FROM_LAYER2D_UL_MAC_CE      (Uint8)8
//#define	QMSS_RX_HAND_LAYER2C_FROM_L2_BUF_REP             (Uint8)9
//#define	QMSS_RX_HAND_LAYER2C_FROM_L3_CFG_REQ             (Uint8)10
//#define	QMSS_RX_HAND_LAYER2C_FROM_OAM_CFG_REQ            (Uint8)11
//#define	QMSS_RX_HAND_MAX                                 (Uint8)12

#define	QMSS_TX_FREE_HAND_ULP_TO_OAM_LOG	qmssShareObj.Qmss_LogTxFreeQueHnd[TxQue_Type_L2_LOG]	// ULP get free memory for sending log data to OAM, QMSS_TX_FREE_HAND_LAYER2D_TO_OAM_LOG

#define	QMSS_TX_HAND_ULP_TO_OAM_LOG			qmssShareObj.Qmss_LogTxQueHnd[TxQue_Type_L2_LOG]	// ULP send log data to OAM, QMSS_TX_HAND_LAYER2D_TO_OTHER_LOG

#define	QMSS_RX_HAND_ULP_FROM_L1_DATA		qmssShareObj.Qmss_RxQueHnd[TxQue_Type_L2][RX_L2_FROM_L1_DATA];	// ULP receive UL data from L1, QMSS_RX_HAND_LAYER2D_FROM_L1_DATA

typedef struct {
	Qmss_QueueHnd qid;
} MessageQueue;

int MessageQSend(MessageQueue* pMsgQueue, char* pBuffer, unsigned int length);
void* MessageQGetFreeTxFd(MessageQueue* pMsgQueue, unsigned char** pBuffer, unsigned int* pBufferLen);
int MessageQSendByFd(MessageQueue* pMsgQueue, void* pFd, unsigned int length);

#endif



#endif /* MESSAGING_H_ */
