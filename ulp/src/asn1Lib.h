/*
 * asn1Lib.h
 *
 *  Created on: Sep 28, 2017
 *      Author: J.ZH
 */

#ifndef ASN1LIB_H_
#define ASN1LIB_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t    int8;
typedef uint8_t   uint8;
typedef int16_t   int16;
typedef uint16_t  uint16;
typedef int32_t   int32;
typedef uint32_t  uint32;
typedef int64_t   int64;
typedef uint64_t  uint64;

#define TRUE                1
#define FALSE               0

#define LIBLTE_MAX_MSG_SIZE_BITS  102048
#define LIBLTE_MAX_MSG_SIZE_BYTES 12756

/*********************************************************************
    Message Name: Message Header (Plain NAS Message)

    Description: Message header for plain NAS messages.

    Document Reference: 24.301 v10.2.0 Section 9.1
*********************************************************************/
// Defines
#define LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT                                              0x2
#define LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT                                             0x7
#define LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS                                            0x0
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY                                            0x1
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED                               0x2
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT              0x3
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT 0x4
#define LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST                                      0xC
#define LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST                                                0x41
#define LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT                                                 0x42
#define LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE                                               0x43
#define LIBLTE_MME_MSG_TYPE_ATTACH_REJECT                                                 0x44
#define LIBLTE_MME_MSG_TYPE_DETACH_REQUEST                                                0x45
#define LIBLTE_MME_MSG_TYPE_DETACH_ACCEPT                                                 0x46
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST                                  0x48
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_ACCEPT                                   0x49
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_COMPLETE                                 0x4A
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REJECT                                   0x4B
#define LIBLTE_MME_MSG_TYPE_EXTENDED_SERVICE_REQUEST                                      0x4C
#define LIBLTE_MME_MSG_TYPE_SERVICE_REJECT                                                0x4E
#define LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMMAND                                     0x50
#define LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMPLETE                                    0x51
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST                                        0x52
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE                                       0x53
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT                                         0x54
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE                                        0x5C
#define LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST                                              0x55
#define LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE                                             0x56
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND                                         0x5D
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE                                        0x5E
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_REJECT                                          0x5F
#define LIBLTE_MME_MSG_TYPE_EMM_STATUS                                                    0x60
#define LIBLTE_MME_MSG_TYPE_EMM_INFORMATION                                               0x61
#define LIBLTE_MME_MSG_TYPE_DOWNLINK_NAS_TRANSPORT                                        0x62
#define LIBLTE_MME_MSG_TYPE_UPLINK_NAS_TRANSPORT                                          0x63
#define LIBLTE_MME_MSG_TYPE_CS_SERVICE_NOTIFICATION                                       0x64
#define LIBLTE_MME_MSG_TYPE_DOWNLINK_GENERIC_NAS_TRANSPORT                                0x68
#define LIBLTE_MME_MSG_TYPE_UPLINK_GENERIC_NAS_TRANSPORT                                  0x69
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST                   0xC1
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT                    0xC2
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT                    0xC3
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST                 0xC5
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT                  0xC6
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT                  0xC7
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REQUEST                             0xC9
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_ACCEPT                              0xCA
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REJECT                              0xCB
#define LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST                         0xCD
#define LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT                          0xCE
#define LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REQUEST                                      0xD0
#define LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REJECT                                       0xD1
#define LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REQUEST                                        0xD2
#define LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REJECT                                         0xD3
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REQUEST                            0xD4
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REJECT                             0xD5
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REQUEST                          0xD6
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REJECT                           0xD7
#define LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST                                       0xD9
#define LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE                                      0xDA
#define LIBLTE_MME_MSG_TYPE_NOTIFICATION                                                  0xDB
#define LIBLTE_MME_MSG_TYPE_ESM_STATUS                                                    0xE8

typedef enum{
  LIBLTE_SUCCESS = 0,
  LIBLTE_ERROR_INVALID_INPUTS,
  LIBLTE_ERROR_ENCODE_FAIL,
  LIBLTE_ERROR_DECODE_FAIL,
  LIBLTE_ERROR_INVALID_CRC,
  LIBLTE_ERROR_N_ITEMS
}LIBLTE_ERROR_ENUM;

typedef enum{
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS = 0,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_CDMA2000_1XRTT,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_CDMA2000_HRPD,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_N_ITEMS,
}LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM;

typedef struct{
    uint32  N_bits;
    uint8   buff[LIBLTE_MAX_MSG_SIZE_BITS];
    uint8*  msg;
} LIBLTE_BIT_MSG_STRUCT;

typedef struct{
    uint32 N_bytes;
    uint8  msg[LIBLTE_MAX_MSG_SIZE_BYTES];
} LIBLTE_SIMPLE_BYTE_MSG_STRUCT;

typedef struct{
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT                dedicated_info;
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM dedicated_info_type;
}LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT;

#define LIBLTE_MME_MOBILE_ID_TYPE_IMSI   0x1
#define LIBLTE_MME_MOBILE_ID_TYPE_IMEI   0x2
#define LIBLTE_MME_MOBILE_ID_TYPE_IMEISV 0x3
#define LIBLTE_MME_MOBILE_ID_TYPE_TMSI   0x4
#define LIBLTE_MME_MOBILE_ID_TYPE_TMGI   0x5
typedef struct{
    uint8 type_of_id;
    uint8 imsi[15];
    uint8 imei[15];
    uint8 imeisv[16];
}LIBLTE_MME_MOBILE_ID_STRUCT;

typedef struct{
    LIBLTE_MME_MOBILE_ID_STRUCT mobile_id;
}LIBLTE_MME_ID_RESPONSE_MSG_STRUCT;

extern LIBLTE_ERROR_ENUM liblte_mme_parse_msg_header(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, uint8 *pd, uint8 *msg_type);
extern LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT *msg, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer);

extern void convert_int_to_bits_vector(uint8 **bits, uint32 value, uint32 num_bits);
extern void convert_bytes_to_bits_vector(uint8 *bit_vector, uint8 *byte_buf, uint32 num_bits);

extern uint32 liblte_bits_2_value(uint8  **bits, uint32   N_bits);
extern LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_nas_ie(uint8 **ie_ptr, LIBLTE_SIMPLE_BYTE_MSG_STRUCT  *ded_info_nas);

extern void liblte_rrc_warning_not_handled(uint8 opt, const char *func_name);
extern void liblte_rrc_consume_noncrit_extension(uint8 ext, const char *func_name, uint8 **ie_ptr);

extern LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_response_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp);
extern LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_id_ie(uint8 **ie_ptr, LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id);

#endif /* ASN1LIB_H_ */
