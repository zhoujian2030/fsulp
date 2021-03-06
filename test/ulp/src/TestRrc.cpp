/*
 * TestRrc.cpp
 *
 *  Created on: May 05, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestRrc.h"
#include "lteRrc.h"
#include "mempool.h"
#include "lteIntegrationPoint.h"
#include "lteRrcPdcpInterface.h"
#include "lteRrcMacInterface.h"
#include "UlpTestCommon.h"
#include "lteKpi.h"
#include "asn1.h"
#include "list.h"
#include "lteLogger.h"
#include "lteCommon.h"

using namespace std;

extern LteKpi gLteKpi;

extern List gRrcUeContextList;
extern List gReadyRrcUeContextList;
// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_IdResp) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;
    unsigned char expectImsiStr[] = "460041143702947";
    // for (unsigned i=0; i<15; i++) {
    //     expectImsiStr[i] -= 0x30;
    // }

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;
    ulRptInfoList.count = 2;
    ulRptInfoList.ulRptInfo[0].rbNum = 1;
    ulRptInfoList.ulRptInfo[1].rbNum = 4;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.msgType, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr, 15) == 0);
#ifdef PHY_DEBUG
    ASSERT_TRUE(gLteKpi.idRespRbNum[0] == 1);
    ASSERT_TRUE(gLteKpi.idRespRbNum[3] == 1);
#endif
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_Detach) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    // ULInformationTransfer
    //     Type: UL_DCCH
    //     Direction: Uplink
    //     Computer Timestamp: 17:05:45.860
    //     UE Timestamp: 26202026 (ms)
    //     Radio Technology: LTE
    //     UL-DCCH-Message
    //         message
    //         c1
    //             ulInformationTransfer
    //             criticalExtensions
    //                 c1
    //                 ulInformationTransfer-r8
    //                     dedicatedInfoType
    //                     dedicatedInfoNAS: 0x271CC08127080745090BF664F0000362CCF922F43B
    //     Detach Request
    //         Security header type: (2) Integrity protected and ciphered 
    //         protocol_discriminator: EPS Mobility Management
    //         Message authentication code: 0x1cc08127 
    //         Sequence number: 8 
    //         Security header type: (0) Plain NAS message, not security protected 
    //         protocol_discriminator: (7) EPS mobility management messages 
    //         NAS EPS Mobility Management Message Type: (0x45) Detach request 
    //         Type of security context flag (TSC): (0) Native security context 
    //         NAS key set identifier: (0) 
    //         Switch off: (1) Switch off 
    //         Detach Type: (1) EPS detach 
    //         EPS mobile identity - GUTI or IMSI
    //         Length: 11 octets
    //         odd/even indic: 0
    //         Type of identity: (6) GUTI 
    //         Mobile Country Code (MCC): (460) China (People's Republic of) 
    //         Mobile Network Code (MNC): (00) China Mobile  
    //         MME Group ID: 866
    //         MME Code: 204
    //         M-TMSI: 0xf922f43b
    unsigned char rrcMsg[] = {
        0x48, 0x02, 0xa4, 0xe3, 0x98, 0x10, 0x24, 0xe1, 0x00, 0xe8,
        0xa1, 0x21, 0x7e, 0xcc, 0x9e, 0x00, 0x00, 0x6c, 0x59, 0x9f,
        0x24, 0x5e, 0x87, 0x60
    };

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_DETACH_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.msgType, DETACH_REQUEST);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsi, 0xf922f43b);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 102;
    unsigned short lcId = 1;
    // c1: rrcConnectionSetupComplete-r8 (0)
	// rrcConnectionSetupComplete-r8
	// 	selectedPLMN-Identity: 1
	// 	registeredMME
	// 		mmegi: 8001 [bit length 16, 1000 0000  0000 0001 decimal value 32769]
	// 		mmec: 00 [bit length 8, 0000 0000 decimal value 0]
	// 	dedicatedInfoNAS: 173945e5340b0741020bf6030227800100d0cc715104e0e0...
	// 		Non-Access-Stratum (NAS)PDU
	// 			0001 .... = Security header type: Integrity protected (1)
	// 			.... 0111 = Protocol discriminator: EPS mobility management messages (0x7)
	// 			Message authentication code: 0x3945e534
	// 			Sequence number: 11
	// 			0000 .... = Security header type: Plain NAS message, not security protected (0)
	// 			.... 0111 = Protocol discriminator: EPS mobility management messages (0x7)
	// 			NAS EPS Mobility Management Message Type: Attach request (0x41)
	// 			0... .... = Type of security context flag (TSC): Native security context (for KSIasme)
	// 			.000 .... = NAS key set identifier:  (0)
	// 			.... 0... = Spare bit(s): 0x00
	// 			.... .010 = EPS attach type: Combined EPS/IMSI attach (2)
	// 			EPS mobile identity
	// 				Length: 11
	// 				.... 0... = Odd/even indication: Even number of identity digits
	// 				.... .110 = Type of identity: GUTI (6)
	// 				Mobile Country Code (MCC): Canada (302)
	// 				Mobile Network Code (MNC): Rogers Wireless (720)
	// 				MME Group ID: 32769
	// 				MME Code: 0
	// 				M-TMSI: 0xd0cc7151
    unsigned char rrcMsg[] = {
        0x20, 0x20, 0x80, 0x01, 0x00, 0x59, 0x17, 0x39, 0x45, 
        0xE5, 0x34, 0x0B, 0x07, 0x41, 0x02, 0x0B, 0xF6, 0x03, 0x02,
        0x27, 0x80, 0x01, 0x00, 0xD0, 0xCC, 0x71, 0x51, 0x04, 0xE0,
        0xE0, 0xC0, 0x40, 0x00, 0x21, 0x02, 0x03, 0xD0, 0x11, 0xD1, 
        0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00, 0x10, 
        0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;
    ulRptInfoList.count = 2;
    ulRptInfoList.ulRptInfo[0].rbNum = 2;
    ulRptInfoList.ulRptInfo[1].rbNum = 6;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_ATTACH_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.msgType, ATTACH_REQUEST);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsi, 0xd0cc7151);
#ifdef PHY_DEBUG
    ASSERT_EQ(1, (int)gLteKpi.rrcSetupComplRbNum[1]);
    ASSERT_EQ(1, (int)gLteKpi.rrcSetupComplRbNum[5]);
#endif
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl_ExtServReq) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();
    Asn1Init();

    unsigned short rnti = 103;
    unsigned short lcId = 1;

    // DLT: 147, Payload: lte-rrc.ul.dcch (LTE Radio Resource Control (RRC) protocol)
    // UL-DCCH-Message
    //     message: c1 (0)
    //         c1: rrcConnectionSetupComplete (4)
    //             rrcConnectionSetupComplete
    //                 rrc-TransactionIdentifier: 1
    //                 criticalExtensions: c1 (0)
    //                     c1: rrcConnectionSetupComplete-r8 (0)
    //                         rrcConnectionSetupComplete-r8
    //                             selectedPLMN-Identity: 1
    //                             dedicatedInfoNAS: 174c63e3c209074c0105f4ea69f853b157022000
    //                                 Non-Access-Stratum (NAS)PDU
    //                                     0001 .... = Security header type: Integrity protected (1)
    //                                     .... 0111 = Protocol discriminator: EPS mobility management messages (0x7)
    //                                     Message authentication code: 0x4c63e3c2
    //                                     Sequence number: 9
    //                                     0000 .... = Security header type: Plain NAS message, not security protected (0)
    //                                     .... 0111 = Protocol discriminator: EPS mobility management messages (0x7)
    //                                     NAS EPS Mobility Management Message Type: Extended service request (0x4c)
    //                                     0... .... = Type of security context flag (TSC): Native security context (for KSIasme)
    //                                     .000 .... = NAS key set identifier:  (0)
    //                                     .... 0001 = Service type: Mobile terminating CS fallback or 1xCS fallback (1)
    //                                     Mobile identity - M-TMSI - TMSI/P-TMSI (0xea69f853)
    //                                         Length: 5
    //                                         1111 .... = Unused: 0xf
    //                                         .... 0... = Odd/even indication: Even number of identity digits
    //                                         .... .100 = Mobile Identity Type: TMSI/P-TMSI/M-TMSI (4)
    //                                         TMSI/P-TMSI: 0xea69f853
    //                                     CSFB response
    //                                         1011 .... = Element ID: 0xb-
    //                                         .... 0... = Spare bit(s): 0x00
    //                                         .... ..01 = CSFB response: CS fallback accepted by the UE (1)
    //                                     EPS bearer context status
    //                                         Element ID: 0x57
    //                                         Length: 2
    //                                         0... .... = EBI(7): BEARER CONTEXT-INACTIVE
    //                                         .0.. .... = EBI(6): BEARER CONTEXT-INACTIVE
    //                                         ..1. .... = EBI(5): BEARER CONTEXT-ACTIVE
    //                                         ...0 .... = EBI(4) spare: False
    //                                         .... 0... = EBI(3) spare: False
    //                                         .... .0.. = EBI(2) spare: False
    //                                         .... ..0. = EBI(1) spare: False
    //                                         .... ...0 = EBI(0) spare: False
    //                                         0... .... = EBI(15): BEARER CONTEXT-INACTIVE
    //                                         .0.. .... = EBI(14): BEARER CONTEXT-INACTIVE
    //                                         ..0. .... = EBI(13): BEARER CONTEXT-INACTIVE
    //                                         ...0 .... = EBI(12): BEARER CONTEXT-INACTIVE
    //                                         .... 0... = EBI(11): BEARER CONTEXT-INACTIVE
    //                                         .... .0.. = EBI(10): BEARER CONTEXT-INACTIVE
    //                                         .... ..0. = EBI(9): BEARER CONTEXT-INACTIVE
    //                                         .... ...0 = EBI(8): BEARER CONTEXT-INACTIVE
    //                             nonCriticalExtension
    //                                 nonCriticalExtension
    //                                     nonCriticalExtension
    //                                         nonCriticalExtension
    //                                             mobilityState-r12: normal (0)
    //                                             mobilityHistoryAvail-r12: true (0)
    unsigned char rrcMsg[] = {
        0x22, 0x10, 0x28, 0x2e, 0x98, 0xc7, 0xc7, 0x84, 0x12, 0x0e, 
        0x98, 0x02, 0x0b, 0xe9, 0xd4, 0xd3, 0xf0, 0xa7, 0x62, 0xae, 
        0x04, 0x40, 0x00, 0x85, 0xc0, 0x00, 0x00, 0x00, 0x00
    };

    RrcUeDataInd_test* pRrcUeDataInd;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_EXTENDED_SERVICE_REQUEST);    
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.mTmsiPresent, 1); 
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.mTmsi, 0xea69f853);
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.msgType, EXT_SERVICE_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl_TAU_Req) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 103;
    unsigned short lcId = 1;
    // RRCConnectionSetupComplete
        // Type: UL_DCCH
        // Direction: Uplink
        // Computer Timestamp: 21:24:42.044
        // UE Timestamp: 11301161 (ms)
        // Radio Technology: LTE
        // UL-DCCH-Message
        //     message
        //     c1
        //         rrcConnectionSetupComplete
        //         rrc-TransactionIdentifier: 1
        //         criticalExtensions
        //             c1
        //             rrcConnectionSetupComplete-r8
        //                 selectedPLMN-Identity: 1
        //                 registeredMME
        //                 mmegi: 0x0376
        //                 mmec: 0xAC
        //                 dedicatedInfoNAS: 0x170E816666030748110BF664F0000376ACC81B62595804E0E0C0C05264F000267D5C20005702200031039560341364F000267D11035758965D0104
        // Tracking Area Update Request
        //     Security header type: (1) Integrity protected 
        //     protocol_discriminator: EPS Mobility Management
        //     Message authentication code: 0xe816666 
        //     Sequence number: 3 
        //     Security header type: (0) Plain NAS message, not security protected 
        //     protocol_discriminator: (7) EPS mobility management messages 
        //     NAS EPS Mobility Management Message Type: (0x48) Tracking area update request 
        //     Type of security context flag (TSC): (0) Native security context 
        //     NAS key set identifier: (1)   ASME
        //     Active flag:  No bearer establishment requested (0)
        //     EPS update type value: (1) Combined TA/LA updating 
        //     EPS mobile identity - Old GUTI
        //     Length: 11 octets
        //     odd/even indic: 0
        //     Type of identity: (6) GUTI 
        //     Mobile Country Code (MCC): (460) China (People's Republic of) 
        //     Mobile Network Code (MNC): (00) China Mobile  
        //     MME Group ID: 886
        //     MME Code: 172
        //     M-TMSI: 0xc81b6259
        //     UE network capability
        //     Element ID: 88 
        //     Length: 4 octets
        //     EEA0: (1) Support
        //     128-EEA1: (1) Support
        //     128-EEA2: (1) Support
        //     EEA3: (0) Not support
        //     EEA4: (0) Not support
        //     EEA5: (0) Not support
        //     EEA6: (0) Not support
        //     EEA7: (0) Not support
        //     EIA0: (1) Support
        //     128-EIA1: (1) Support
        //     128-EIA2: (1) Support
        //     EIA3: (0) Not support
        //     EIA4: (0) Not support
        //     EIA5: (0) Not support
        //     EIA6: (0) Not support
        //     EIA7: (0) Not support
        //     UEA0: (1) Support
        //     UEA1: (1) Support
        //     UEA2: (0) Not support
        //     UEA3: (0) Not support
        //     UEA4: (0) Not support
        //     UEA5: (0) Not support
        //     UEA6: (0) Not support
        //     UEA7: (0) Not support
        //     UCS2 support (UCS2): (1) Support
        //     UMTS integrity algorithm UIA1: (1) Support
        //     UMTS integrity algorithm UIA2: (0) Not support
        //     UMTS integrity algorithm UIA3: (0) Not support
        //     UMTS integrity algorithm UIA4: (0) Not support
        //     UMTS integrity algorithm UIA5: (0) Not support
        //     UMTS integrity algorithm UIA6: (0) Not support
        //     UMTS integrity algorithm UIA7: (0) Not support
        //     Tracking area identity - Last visited registered TAI
        //     Element ID: 82 
        //     Mobile Country Code (MCC): (460) China (People's Republic of) 
        //     Mobile Network Code (MNC): (00) China Mobile  
        //     Tracking area code(TAC): 0x267d
        //     DRX Parameter
        //     Element ID: 92 
        //     DRX Parameter
        //         Split PG Cycle Code: (32) 32
        //         Split on CCCH: (0) Split pg cycle on CCCH is supported by the mobile station
        //         Non-DRX timer: (0) no non-DRX mode after transfer state
        //         CN Specific DRX cycle length coefficient: (0) CN Specific DRX cycle length coefficient / value not specified by the MS
        //     EPS bearer context status
        //     Element ID: 87 
        //     Length: 2 octets
        //     EBI(7): (0) BEARER CONTEXT INACTIVE 
        //     EBI(6): (0) BEARER CONTEXT INACTIVE 
        //     EBI(5): (1) BEARER CONTEXT ACTIVE 
        //     EBI(4): (0) BEARER CONTEXT INACTIVE 
        //     EBI(3): (0) BEARER CONTEXT INACTIVE 
        //     EBI(2): (0) BEARER CONTEXT INACTIVE 
        //     EBI(1): (0) BEARER CONTEXT INACTIVE 
        //     EBI(0): (0) BEARER CONTEXT INACTIVE 
        //     EBI(15): (0) BEARER CONTEXT INACTIVE 
        //     EBI(14): (0) BEARER CONTEXT INACTIVE 
        //     EBI(13): (0) BEARER CONTEXT INACTIVE 
        //     EBI(12): (0) BEARER CONTEXT INACTIVE 
        //     EBI(11): (0) BEARER CONTEXT INACTIVE 
        //     EBI(10): (0) BEARER CONTEXT INACTIVE 
        //     EBI(9): (0) BEARER CONTEXT INACTIVE 
        //     EBI(8): (0) BEARER CONTEXT INACTIVE 
        //     MS Network Capability
        //     Element ID: 49 
        //     Length: 3 octets
        //     GEA1: (1) encryption algorithm available
        //     SM capabilities via dedicated channels: (0) Mobile station does not support mobile terminated point to point SMS via dedicated signalling channels
        //     SM capabilities via GPRS channels: (0) Mobile station does not support mobile terminated point to point SMS via GPRS packet data channels
        //     UCS2 support: (1) the ME has no preference between the use of the default alphabet and the use of UCS2
        //     SS Screening Indicator: (1) capability of handling of ellipsis notation and phase 2 error handling
        //     SoLSA Capability: (0) The ME does not support SoLSA
        //     Revision level indicator: (1) used by a mobile station supporting R99 or later versions of the protocol
        //     PFC feature mode: (0) Mobile station does not support BSS packet flow procedures
        //     GEA2: (1) encryption algorithm available
        //     GEA3: (1) encryption algorithm available
        //     GEA4: (0) encryption algorithm not available
        //     GEA5: (0) encryption algorithm not available
        //     GEA6: (0) encryption algorithm not available
        //     GEA7: (0) encryption algorithm not available
        //     LCS VA capability:: (0) LCS value added location request notification capability not supported
        //     PS inter-RAT HO to UTRAN Iu mode capability: (0) PS inter-RAT HO to UTRAN Iu mode not supported
        //     PS inter-RAT HO to E-UTRAN S1 mode capability: (0) PS inter-RAT HO to E-UTRAN S1 mode not supported
        //     CSFB Capability: (1) Mobile station support CS fallback
        //     ISR support: (1) The mobile station support ISR
        //     SRVCC to GERAN/UTRAN capability: (0) SRVCC from UTRAN HSPA or E-UTRAN to GERAN/UTRAN not supported
        //     EPC Capability: (1) EPC supported
        //     Location area identification - Old location area identification
        //     Element ID: 19 
        //     Location Area Identification (LAI)
        //         Mobile Country Code (MCC): (460) China (People's Republic of) 
        //         Mobile Network Code (MNC): (00) China Mobile  
        //         Location Area Code (LAC): 9853
        //     Mobile station classmark 2
        //     Element ID: 17 
        //     Length: 3 octets
        //     Length of mobile station classmark 2 contents: 3
        //     RF power capability: (7) Undefined value
        //     Revision level: (2) Used by mobile stations supporting R99 or later versions of the protocol
        //     ES IND: (1)  Controlled Early Classmark Sending option is implemented in the MS
        //     Encryption algorithm A5/1: (0) Available
        //     Encryption algorithm A5/2: (0) Not available
        //     Encryption algorithm A5/3: (1) Available
        //     Frequency Capability: (0) The MS does not support the E-GSM or R-GSM band
        //     PS capability (Pseudo-synchronization capability): (1) Present
        //     SS Screening Indicator: (1) Capability of handling of ellipsis notation and phase 2 error handling 
        //     SM capability: (1) Mobile station supports mobile terminated point to point SMS
        //     CM3: (1) The MS supports options that are indicated in classmark 3 IE
        //     VBS notification reception: (0) No VBS capability or no notifications wanted
        //     VGCS notification reception: (0) No VGCS capability or no notifications wanted
        //     LCS VA capability: (0) LCS value added location request notification capability not supported.
        //     UCS2: (1) the ME has no preference between the use of the default alphabet and the use of UCS2.
        //     SoLSA: (0) The ME does not support SoLSA.
        //     CM Service Prompt: (1)"Network initiated MO CM connection request" supported for at least one CM protocol.
        //     Voice domain preference and UE's usage setting
        //     Element ID: 93 
        //     Length: 1 octets
        //     Length of Voice domain preference and UE's usage setting contents: 1
        //     Voice domain preference for E-UTRAN: (0) CS Voice only
        //     UE's usage setting: (1) Data centric

    unsigned char rrcMsg[] = {
        0x22, 0x20, 0x03, 0x76, 0xac, 0x3b, 0x17, 0x0e, 0x81, 0x66, 0x66, 0x03, 0x07, 0x48, 0x11, 0x0b, 
        0xf6, 0x64, 0xf0, 0x00, 0x03, 0x76, 0xac, 0xc8, 0x1b, 0x62, 0x59, 0x58, 0x04, 0xe0, 0xe0, 0xc0, 
        0xc0, 0x52, 0x64, 0xf0, 0x00, 0x26, 0x7d, 0x5c, 0x20, 0x00, 0x57, 0x02, 0x20, 0x00, 0x31, 0x03, 
        0x95, 0x60, 0x34, 0x13, 0x64, 0xf0, 0x00, 0x26, 0x7d, 0x11, 0x03, 0x57, 0x58, 0x96, 0x5d, 0x01, 
        0x04
    };

    RrcUeDataInd_test* pRrcUeDataInd;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();

    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST);    
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.mTmsiPresent, 1); 
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.mTmsi, 0xc81b6259);
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.msgType, TAU_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl_ServReq) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();
    Asn1Init();

    unsigned short rnti = 103;
    unsigned short lcId = 1;

    // RRCConnectionSetupComplete
    // Type: UL_DCCH
    // Direction: Uplink
    // Computer Timestamp: 22:04:21.161
    // UE Timestamp: 470481 (ms)
    // Radio Technology: LTE
    // UL-DCCH-Message
    //     message
    //     c1
    //         rrcConnectionSetupComplete
    //         rrc-TransactionIdentifier: 1
    //         criticalExtensions
    //             c1
    //             rrcConnectionSetupComplete-r8
    //                 selectedPLMN-Identity: 1
    //                 dedicatedInfoNAS: 0xC7869657
    // Service Request
    //     Security header type: (12) Security header for the SERVICE REQUEST message  
    //     protocol_discriminator: EPS Mobility Management
    //     key set identifier: 4 
    //     Sequence number (short): 6 
    //     Message authentication code  (short): 0x9657 
    unsigned char rrcMsg[] = {
        0x22, 0x00, 0x09, 0x8f, 0x0d, 0x2c, 0xae
    };

    RrcUeDataInd_test* pRrcUeDataInd;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
#ifdef DPE
#ifdef TARGET_LOCATION
    ASSERT_EQ((int)gLteKpi.mem, 1);
#endif
#else
    ASSERT_EQ((int)gLteKpi.mem, 0);
#endif
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_SECURITY_HDR_TYPE_SERVICE_REQUEST);    
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.mTmsiPresent, 0); 
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.imsiPresent, 0);
    ASSERT_EQ(pRrcUeDataInd->ueContext.ueIdentity.msgType, SERVICE_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_Update_Diff_Imsi) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg1[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char rrcMsg2[] = {
        0x48, 0x02, 0x22, 0xe6, 0x8e, 0xcb, 0xc9, 0x62, 0x80, 
        0xea, 0xc1, 0x09, 0x20, 0xc4, 0x04, 0x0e, 0xca, 0x6c, 
        0x25, 0x00, 0x00, 0x00, 0x00, 0x00 
    };
    

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;
    unsigned char expectImsiStr1[] = "460041143702947";
    unsigned char expectImsiStr2[] = "460020267351682";

    // id resp 1
    unsigned short dataSize = sizeof(rrcMsg1);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg1, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr1, 15) == 0);
    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 0);

    // id resp 2
    dataSize = sizeof(rrcMsg2);
    pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg2, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 2); // 1 rrc ctx, 1 pPdcpDataInd

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 2);  // 1 rrc ctx in gRrcUeContextList, 1 in gReadyRrcUeContextList
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr2, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 1);

    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 1);
    pRrcUeCtx = (RrcUeContext*)ListPopNode(&gReadyRrcUeContextList);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    cout << "old imsi = " << pRrcUeCtx->ueIdentity.imsi << endl;
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr1, 15) == 0); 
    MemFree(pRrcUeCtx);   
    
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_Update_Same_Imsi) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg1[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char rrcMsg2[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;
    unsigned char expectImsiStr1[] = "460041143702947";
    unsigned char expectImsiStr2[] = "460041143702947";

    // id resp 1
    unsigned short dataSize = sizeof(rrcMsg1);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg1, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr1, 15) == 0);
    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 0);

    // id resp 2
    dataSize = sizeof(rrcMsg2);
    pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg2, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 2); // 1 rrc ctx, 1 pPdcpDataInd

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  // 1 rrc ctx in gRrcUeContextList
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr2, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);

    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_Update_Diff_Imsi_Prev_Deleting) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg1[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char rrcMsg2[] = {
        0x48, 0x02, 0x22, 0xe6, 0x8e, 0xcb, 0xc9, 0x62, 0x80, 
        0xea, 0xc1, 0x09, 0x20, 0xc4, 0x04, 0x0e, 0xca, 0x6c, 
        0x25, 0x00, 0x00, 0x00, 0x00, 0x00 
    };
    

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;
    unsigned char expectImsiStr1[] = "460041143702947";
    unsigned char expectImsiStr2[] = "460020267351682";

    // id resp 1
    unsigned short dataSize = sizeof(rrcMsg1);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg1, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);  
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr1, 15) == 0);
    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 0);
    pRrcUeCtx->deleteFlag = 1;

    // id resp 2
    dataSize = sizeof(rrcMsg2);
    pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg2, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 2); // 1 rrc ctx, 1 pPdcpDataInd

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 2);  // 1 rrc ctx in gRrcUeContextList, 1 in gReadyRrcUeContextList
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 2);
    RrcDeleteUeContext(pRrcUeCtx);  // delete prev ue ctx
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr2, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);

    ASSERT_EQ((int)ListCount(&gReadyRrcUeContextList), 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_Invalid_Rrc_Msg) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg1[] = {
        0x26, 0xfe, 0x33, 0xb0, 0xbc, 0xbf, 0xee, 0x03       // outofmemory
        // 0x21, 0x0c, 0x4c, 0xaf, 0xee, 0xba                   // nas len too large
        // 0xa5, 0x4d, 0x83, 0x8e, 0x68, 0xd1, 0xb2, 0x1e, 0x60, 0x43, 0x25, 0x0f  // nas len 0
    };
    unsigned char rrcMsg2[] = {
        0xca, 0x52, 0xb7, 0xe9, 0x01, 0xd2, 0x0a, 0x3d, 0x7a, 0x8f, 0x43, 0xc7, 0x41, 0xaa, 0x5b
    };

    // rrc msg 1
    unsigned short dataSize = sizeof(rrcMsg1);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg1, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 0);  
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));

    // rrc msg 2
    dataSize = sizeof(rrcMsg2);
    pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg2, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 pPdcpDataInd

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 0); 
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_SMS) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg[] = {
        0x48, 0x05, 0x84, 0xe1, 0x2f, 0xb9, 0x16, 0x40, 0x60, 0xec, 
        0x64, 0x61, 0x20, 0x24, 0x00, 0x00, 0x20, 0x01, 0x12, 0x2d, 
        0x06, 0x21, 0x04, 0x00, 0x00, 0xbe, 0x02, 0x66, 0x3c, 0xe1, 
        0xb2, 0x2d, 0x06, 0x25, 0x33, 0x0e, 0xa0, 0xfe, 0xa0, 0x01, 
        0x1e, 0xa0, 0x8b, 0x37, 0x2e, 0xd0, 0x80
    };

    RrcUeDataInd_test* pRrcUeDataInd;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 0);  
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_UPLINK_NAS_TRANSPORT);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}


// -------------------------------
TEST_F(TestRrc, Interface_MacUeCcchDataInd_Rrc_Request) {
    LteLoggerSetLogLevel(0);
    gCallRrcDataInd = 0;
    KpiInit();
    InitRrcLayer();
    InitMemPool();

    unsigned short rnti = 102;
    // RRCConnectionRequest
    // Type: UL_CCCH
    // Direction: Uplink
    // Computer Timestamp: 11:45:15.897
    // UE Timestamp: 54482 (ms)
    // Radio Technology: LTE
    //   UL-CCCH-Message
    //     message
    //       c1
    //         rrcConnectionRequest
    //           criticalExtensions
    //             rrcConnectionRequest-r8
    //               ue-Identity
    //                 s-TMSI
    //                   mmec: 0xD2
    //                   m-TMSI: 0xD0C7EE09
    //               establishmentCause: (4) mo-Data
    //               spare: 0x0
    unsigned char rrcMsg[] = {
        0x4d, 0x2d, 0x0c, 0x7e, 0xe0, 0x98
    };

    RrcUeDataInd_test* pRrcUeDataInd;
    RrcUeContext* pRrcUeCtx;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pCcchDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pCcchDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    MacUeCcchDataInd(rnti, pCcchDataInd, dataSize, 0);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, 0xff);
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));

    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(rnti);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsi, 0xD0C7EE09);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}
