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

#define IP_MAC_DATA_IND(param) ({1;})
#define IP_RLC_DATA_IND(param1, param2, param3, param4) ({1;})
#define IP_PDCP_SRB_DATA_IND(param1, param2, param3, param4) ({1;})
#define IP_RRC_DATA_IND(param1) ({1;})
#define IP_RRC_DECODE_RESULT(param1, param2, param3, param4) ({1;})

#else 

int IP_Call_Mac_Data_Ind(void* pData);
int IP_Call_Rlc_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size);
int IP_Call_Pdcp_Srb_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size);
int IP_Call_Rrc_Data_Ind(void* pData);
int IP_Rrc_Decode_Result(unsigned short rnti, unsigned char rrcMsgType, unsigned char nasMsgType, void* pData);

#define IP_MAC_DATA_IND(param) ({\
        int ret = 0;\
        if(param != 0) {\
            ret = IP_Call_Mac_Data_Ind((void*)param);\
        }\
        ret;\
    })

#define IP_RLC_DATA_IND(param1, param2, param3, param4) ({\
        int ret = 0;\
        if(param3 != 0) {\
            ret = IP_Call_Rlc_Data_Ind(param1, param2, param3, param4);\
        }\
        ret;\
    })

#define IP_PDCP_SRB_DATA_IND(param1, param2, param3, param4) ({\
        int ret = 0;\
        if(param3 != 0) {\
            ret = IP_Call_Pdcp_Srb_Data_Ind(param1, param2, param3, param4);\
        }\
        ret;\
    })

#define IP_RRC_DATA_IND(param1) ({\
        int ret = 0;\
        if(param1 != 0) {\
            ret = IP_Call_Rrc_Data_Ind(param1);\
        }\
        ret;\
    })

#define IP_RRC_DECODE_RESULT(param1, param2, param3, param4) ({\
        int ret = IP_Rrc_Decode_Result(param1, param2, param3, param4);\
        ret;\
    })

#endif

#ifdef __cplusplus
}
#endif

#endif
