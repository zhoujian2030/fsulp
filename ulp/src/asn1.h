/*
 * asn1.h
 *
 *  Created on: Sep 20, 2017
 *      Author: j.zhou
 */

#ifndef ASN1_H_
#define ASN1_H_

#include "asn1Lib.h"

#define PARSE_IMSI

typedef enum {
	ASN1_SUCCES,
	ASN1_ERROR
} ASN1_ERROR_ENUM;

// DL RRC message type
#define RRC_DL_DCCH_MSG_TYPE_CSFB_PARAMS_RESP_CDMA2000		0
#define RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER				1
#define RRC_DL_DCCH_MSG_TYPE_HANDOVER_FROM_EUTRA_PREP_REQ	2
#define RRC_DL_DCCH_MSG_TYPE_MOBILITY_FROM_EUTRA_COMMAND	3
#define RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG				4
#define RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE				5
#define RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND			6
#define RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY			7
#define RRC_DL_DCCH_MSG_TYPE_COUNTER_CHECK					8
#define RRC_DL_DCCH_MSG_TYPE_UE_INFO_REQ					9
#define RRC_DL_DCCH_MSG_TYPE_LOGGED_MEASUREMENTS_CONFIG		10
#define RRC_DL_DCCH_MSG_TYPE_RN_RECONFIG					11
#define RRC_DL_DCCH_MSG_TYPE_N_ITEMS						12

// UL RRC message type
#define RRC_UL_DCCH_MSG_TYPE_CSFB_PARAMS_REQ_CDMA2000	0
#define RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT			1
#define RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE	2
#define RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE		3
#define RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE		4
#define RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE		5
#define RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_FAILURE		6
#define RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO			7
#define RRC_UL_DCCH_MSG_TYPE_UL_HANDOVER_PREP_TRANSFER	8
#define RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER			9
#define RRC_UL_DCCH_MSG_TYPE_COUNTER_CHECK_RESP			10
#define RRC_UL_DCCH_MSG_TYPE_UE_INFO_RESP				11
#define RRC_UL_DCCH_MSG_TYPE_PROXIMITY_IND				12
#define RRC_UL_DCCH_MSG_TYPE_RN_RECONFIG_COMPLETE		13
#define RRC_UL_DCCH_MSG_TYPE_SPARE2						14
#define RRC_UL_DCCH_MSG_TYPE_SPARE1						15
#define RRC_UL_DCCH_MSG_TYPE_N_ITEMS					16

// MME NAS message type
#define NAS_PD_EPS_SESSION_MANAGEMENT                                              0x2
#define NAS_PD_EPS_MOBILITY_MANAGEMENT                                             0x7
#define NAS_SECURITY_HDR_TYPE_PLAIN_NAS                                            0x0
#define NAS_SECURITY_HDR_TYPE_INTEGRITY                                            0x1
#define NAS_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED                               0x2
#define NAS_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT              0x3
#define NAS_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT 0x4
#define NAS_SECURITY_HDR_TYPE_SERVICE_REQUEST                                      0xC
#define NAS_MSG_TYPE_ATTACH_REQUEST                                                0x41
#define NAS_MSG_TYPE_ATTACH_ACCEPT                                                 0x42
#define NAS_MSG_TYPE_ATTACH_COMPLETE                                               0x43
#define NAS_MSG_TYPE_ATTACH_REJECT                                                 0x44
#define NAS_MSG_TYPE_DETACH_REQUEST                                                0x45
#define NAS_MSG_TYPE_DETACH_ACCEPT                                                 0x46
#define NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST                                  0x48
#define NAS_MSG_TYPE_TRACKING_AREA_UPDATE_ACCEPT                                   0x49
#define NAS_MSG_TYPE_TRACKING_AREA_UPDATE_COMPLETE                                 0x4A
#define NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REJECT                                   0x4B
#define NAS_MSG_TYPE_EXTENDED_SERVICE_REQUEST                                      0x4C
#define NAS_MSG_TYPE_SERVICE_REJECT                                                0x4E
#define NAS_MSG_TYPE_GUTI_REALLOCATION_COMMAND                                     0x50
#define NAS_MSG_TYPE_GUTI_REALLOCATION_COMPLETE                                    0x51
#define NAS_MSG_TYPE_AUTHENTICATION_REQUEST                                        0x52
#define NAS_MSG_TYPE_AUTHENTICATION_RESPONSE                                       0x53
#define NAS_MSG_TYPE_AUTHENTICATION_REJECT                                         0x54
#define NAS_MSG_TYPE_AUTHENTICATION_FAILURE                                        0x5C
#define NAS_MSG_TYPE_IDENTITY_REQUEST                                              0x55
#define NAS_MSG_TYPE_IDENTITY_RESPONSE                                             0x56
#define NAS_MSG_TYPE_SECURITY_MODE_COMMAND                                         0x5D
#define NAS_MSG_TYPE_SECURITY_MODE_COMPLETE                                        0x5E
#define NAS_MSG_TYPE_SECURITY_MODE_REJECT                                          0x5F
#define NAS_MSG_TYPE_EMM_STATUS                                                    0x60
#define NAS_MSG_TYPE_EMM_INFORMATION                                               0x61
#define NAS_MSG_TYPE_DOWNLINK_NAS_TRANSPORT                                        0x62
#define NAS_MSG_TYPE_UPLINK_NAS_TRANSPORT                                          0x63
#define NAS_MSG_TYPE_CS_SERVICE_NOTIFICATION                                       0x64
#define NAS_MSG_TYPE_DOWNLINK_GENERIC_NAS_TRANSPORT                                0x68
#define NAS_MSG_TYPE_UPLINK_GENERIC_NAS_TRANSPORT                                  0x69
#define NAS_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST                   0xC1
#define NAS_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT                    0xC2
#define NAS_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT                    0xC3
#define NAS_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST                 0xC5
#define NAS_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT                  0xC6
#define NAS_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT                  0xC7
#define NAS_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REQUEST                             0xC9
#define NAS_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_ACCEPT                              0xCA
#define NAS_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REJECT                              0xCB
#define NAS_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST                         0xCD
#define NAS_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT                          0xCE
#define NAS_MSG_TYPE_PDN_CONNECTIVITY_REQUEST                                      0xD0
#define NAS_MSG_TYPE_PDN_CONNECTIVITY_REJECT                                       0xD1
#define NAS_MSG_TYPE_PDN_DISCONNECT_REQUEST                                        0xD2
#define NAS_MSG_TYPE_PDN_DISCONNECT_REJECT                                         0xD3
#define NAS_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REQUEST                            0xD4
#define NAS_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REJECT                             0xD5
#define NAS_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REQUEST                          0xD6
#define NAS_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REJECT                           0xD7
#define NAS_MSG_TYPE_ESM_INFORMATION_REQUEST                                       0xD9
#define NAS_MSG_TYPE_ESM_INFORMATION_RESPONSE                                      0xDA
#define NAS_MSG_TYPE_NOTIFICATION                                                  0xDB
#define NAS_MSG_TYPE_ESM_STATUS                                                    0xE8

extern int Asn1ParseDcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType);
extern int Asn1ParseUlCcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType);
extern int Asn1ParseDlCcchMsgHeader(unsigned char *msgBuf, unsigned int length, unsigned char *ext, unsigned char *msgType);
extern int Asn1ParseUlDTMsg(unsigned char *msgBuf, unsigned int length, unsigned char *msgType, LIBLTE_MME_ID_RESPONSE_MSG_STRUCT* pIdResp);
extern int Asn1ParseUlInfoTransMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT* pUlInfoTrans);
extern int Asn1ParseRrcSetupComplMsg(unsigned char *msgBuf, unsigned int length, LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT* pRrcSetupCompl);

#endif /* ASN1_H_ */
