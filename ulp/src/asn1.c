/*
 * asn1.cpp
 *
 *  Created on: Sep 20, 2017
 *      Author: j.zhou
 */

#include <stddef.h>
#include "asn1.h"
#include "asn1Lib.h"
#include <stdio.h>

int Asn1ParseDcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType) {
    int err = ASN1_ERROR;

    if(msgBuf != NULL && length != 0 && ext != NULL && msgType != NULL)
    {
        // Extension indicator
        *ext = (*msgBuf) >> 7;
        *msgType = ((*msgBuf) >> 3) & 0x0f;

        err = ASN1_SUCCES;
    }

    return err;
}

int Asn1ParseUlCcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType) {
	int err = ASN1_ERROR;
    if(msgBuf != NULL && length != 0 && ext != NULL && msgType != NULL)
    {
        // Extension indicator
        *ext = (*msgBuf) >> 7;
        *msgType = ((*msgBuf) >> 3) & 0x0f;

        err = ASN1_SUCCES;
    }

    return err;
}

int Asn1ParseDlCcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType) {
	int err = ASN1_ERROR;
    if(msgBuf != NULL && length != 0 && ext != NULL && msgType != NULL)
    {
        // Extension indicator
        *ext = (*msgBuf) >> 7;
        *msgBuf = ((*msgBuf) >> 5) & 0x03;

        err = ASN1_SUCCES;
    }

    return err;
}

int Asn1ParseUlDTMsg(unsigned char *msgBuf, unsigned int length, unsigned char *msgType, LIBLTE_MME_ID_RESPONSE_MSG_STRUCT* pIdResp) {
	LIBLTE_BIT_MSG_STRUCT msgBitBuf;
	msgBitBuf.msg = msgBitBuf.buff;

	unsigned int bitLen = length << 3;
	LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT ulDTMsg;
	LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

	convert_bytes_to_bits_vector(msgBitBuf.msg, msgBuf, bitLen);
	// remove 5 bits RRC msg type
	msgBitBuf.msg += 5;
	msgBitBuf.N_bits = bitLen - 5;

	err = liblte_rrc_unpack_ul_information_transfer_msg(&msgBitBuf, &ulDTMsg);
	if (err == LIBLTE_SUCCESS && ulDTMsg.dedicated_info_type == LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS) {
		uint8 pd;
		err = liblte_mme_parse_msg_header(&ulDTMsg.dedicated_info, &pd, msgType);
#ifdef PARSE_IMSI
		if (*msgType == NAS_MSG_TYPE_IDENTITY_RESPONSE) {
			liblte_mme_unpack_identity_response_msg(&ulDTMsg.dedicated_info, pIdResp);
			// if (LIBLTE_MME_MOBILE_ID_TYPE_IMSI == idResp.mobile_id.type_of_id) {
			// 	memcpy(imsi, idResp.mobile_id.imsi, 15);
			// }
		}
#endif
	}

	return err;
}

int Asn1ParseUlInfoTransMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT* pUlInfoTrans) {
	LIBLTE_BIT_MSG_STRUCT msgBitBuf;
	msgBitBuf.msg = msgBitBuf.buff;
	unsigned int bitLen = length << 3;

	convert_bytes_to_bits_vector(msgBitBuf.msg, msgBuf, bitLen);
	// remove 5 bits RRC msg type
	msgBitBuf.msg += 5;
	msgBitBuf.N_bits = bitLen - 5;

	return liblte_rrc_unpack_ul_information_transfer_msg(&msgBitBuf, pUlInfoTrans);
}

int Asn1ParseRrcSetupComplMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT* pRrcSetupCompl)
{
    LIBLTE_BIT_MSG_STRUCT msgBitBuf;
	msgBitBuf.msg = msgBitBuf.buff;
	unsigned int bitLen = length << 3;

    convert_bytes_to_bits_vector(msgBitBuf.msg, msgBuf, bitLen);
	// remove 5 bits RRC msg type
	msgBitBuf.msg += 5;
	msgBitBuf.N_bits = bitLen - 5;

    return liblte_rrc_unpack_rrc_connection_setup_complete_msg(&msgBitBuf, pRrcSetupCompl);
    // printf("mmegi = %d, mmec = %d, mcc = %d, mnc = %d\n", rrcConnSetupComplMsg.registered_mme.mmegi, rrcConnSetupComplMsg.registered_mme.mmec, 
    //     rrcConnSetupComplMsg.registered_mme.plmn_id.mcc, rrcConnSetupComplMsg.registered_mme.plmn_id.mnc);
}

