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

#ifndef OS_LINUX
#pragma DATA_SECTION(gMsgBitBuffer, ".ulpdata");
#endif
LIBLTE_BIT_MSG_STRUCT gMsgBitBuffer;

// -----------------------------
void Asn1Init()
{
	memset(gMsgBitBuffer.buff, 0, LIBLTE_MAX_MSG_SIZE_BITS);
}

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

int Asn1ParseUlInfoTransMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT* pUlInfoTrans) {
	int err = ASN1_ERROR;

	gMsgBitBuffer.msg = gMsgBitBuffer.buff;
	unsigned int bitLen = length << 3;
    if (bitLen > LIBLTE_MAX_MSG_SIZE_BITS) {
        return err;
    }

	convert_bytes_to_bits_vector(gMsgBitBuffer.msg, msgBuf, bitLen);
	// remove 5 bits RRC msg type
	gMsgBitBuffer.msg += 5;
	gMsgBitBuffer.N_bits = bitLen - 5;

	err = liblte_rrc_unpack_ul_information_transfer_msg(&gMsgBitBuffer, pUlInfoTrans);

	memset(gMsgBitBuffer.buff, 0, bitLen);

	return err;
}

int Asn1ParseRrcSetupComplMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT* pRrcSetupCompl)
{
	int err = ASN1_ERROR;

	gMsgBitBuffer.msg = gMsgBitBuffer.buff;
	unsigned int bitLen = length << 3;
    if (bitLen > LIBLTE_MAX_MSG_SIZE_BITS) {
        return err;
    }

    convert_bytes_to_bits_vector(gMsgBitBuffer.msg, msgBuf, bitLen);
	// remove 5 bits RRC msg type
	gMsgBitBuffer.msg += 5;
	gMsgBitBuffer.N_bits = bitLen - 5;

	err = liblte_rrc_unpack_rrc_connection_setup_complete_msg(&gMsgBitBuffer, pRrcSetupCompl);

	memset(gMsgBitBuffer.buff, 0, bitLen);

	return err;
}

int Asn1ParseRrcConnReqMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_CONNECTION_REQUEST_STRUCT* pRrcConnReq)
{
	int err = ASN1_ERROR;

	gMsgBitBuffer.msg = gMsgBitBuffer.buff;
	unsigned int bitLen = length << 3;
    if (bitLen > LIBLTE_MAX_MSG_SIZE_BITS) {
        return err;
    }

    convert_bytes_to_bits_vector(gMsgBitBuffer.msg, msgBuf, bitLen);
	// remove 2 bits RRC msg type
	gMsgBitBuffer.msg += 2;
	gMsgBitBuffer.N_bits = bitLen - 2;

	err = liblte_rrc_unpack_rrc_connection_request_msg(&gMsgBitBuffer, pRrcConnReq);

	memset(gMsgBitBuffer.buff, 0, bitLen);

	return err;
}

