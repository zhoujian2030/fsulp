/*
 * lteIntegrationPoint.h
 *
 *  Created on: Apr 28, 2018
 *      Author: J.ZH
 */

#ifndef LTE_INTEGRATION_POINT_H
#define LTE_INTEGRATION_POINT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNIT_TEST

#define IP_MAC_DATA_IND(param)
#define IP_RLC_DATA_IND(param1, param2, param3, param4)
#define IP_PDCP_SRB_DATA_IND(param1, param2, param3, param4)
#define IP_RRC_DECODE_RESULT(param1, param2, param3, param4)

#else 

void IP_Call_Mac_Data_Ind(void* pData);
void IP_Call_Rlc_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size);
void IP_Call_Pdcp_Srb_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size);
void IP_Rrc_Decode_Result(unsigned short rnti, unsigned char rrcMsgType, unsigned char nasMsgType, void* pData);

#define IP_MAC_DATA_IND(param) {\
        if(param != 0) {\
            IP_Call_Mac_Data_Ind((void*)param);\
        }\
    }

#define IP_RLC_DATA_IND(param1, param2, param3, param4) {\
        if(param3 != 0) {\
            IP_Call_Rlc_Data_Ind(param1, param2, param3, param4);\
        }\
    }

#define IP_PDCP_SRB_DATA_IND(param1, param2, param3, param4) {\
        if(param3 != 0) {\
            IP_Call_Pdcp_Srb_Data_Ind(param1, param2, param3, param4);\
        }\
    }

#define IP_RRC_DECODE_RESULT(param1, param2, param3, param4) {\
        IP_Rrc_Decode_Result(param1, param2, param3, param4);\
    }

#endif

#ifdef __cplusplus
}
#endif

#endif
