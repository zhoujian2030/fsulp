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
#include "UlpTestCommon.h"
#include "lteKpi.h"
#include "asn1.h"
#include "list.h"

using namespace std;

extern unsigned int gLogLevel;
extern List gRrcUeContextList;
// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_IdResp) {
    gLogLevel = 0;
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
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize);

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
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl) {
    gLogLevel = 0;
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

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize);

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
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsi, 0xd0cc7151);
    RrcDeleteUeContext(pRrcUeCtx);
    KpiRefresh();
    ASSERT_EQ(RrcGetUeContextCount(), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl_TAU_Req) {
    gLogLevel = 0;
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

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}
