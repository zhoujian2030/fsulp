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

#define LIBLTE_RRC_MCC_NOT_PRESENT 0xFFFF
typedef struct{
    uint16 mcc;
    uint16 mnc;
}LIBLTE_RRC_PLMN_IDENTITY_STRUCT;
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id;
    uint16                          mmegi;
    uint8                           mmec;
    uint8                           plmn_id_present;
}LIBLTE_RRC_REGISTERED_MME_STRUCT;
typedef struct{
    LIBLTE_RRC_REGISTERED_MME_STRUCT registered_mme;
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT    dedicated_info_nas;
    uint8                            rrc_transaction_id;
    uint8                            selected_plmn_id;
    uint8                            registered_mme_present;
}LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT;

// for attach req
typedef enum{
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE = 0,
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_MAPPED,
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_N_ITEMS,
}LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM;

typedef struct{
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM tsc_flag;
    uint8                                         nas_ksi;
}LIBLTE_MME_NAS_KEY_SET_ID_STRUCT;

typedef struct{
    uint32 m_tmsi;
    uint16 mcc;
    uint16 mnc;
    uint16 mme_group_id;
    uint8  mme_code;
}LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT;

typedef struct{
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
    uint8                                type_of_id;
    uint8                                imsi[15];
    uint8                                imei[15];
}LIBLTE_MME_EPS_MOBILE_ID_STRUCT;

typedef struct{
    uint8 eea[8];
    uint8 eia[8];
    uint8 uea[8];
    uint8 uea_present;
    uint8 ucs2;
    uint8 ucs2_present;
    uint8 uia[8];
    uint8 uia_present;
    uint8 lpp;
    uint8 lpp_present;
    uint8 lcs;
    uint8 lcs_present;
    uint8 onexsrvcc;
    uint8 onexsrvcc_present;
    uint8 nf;
    uint8 nf_present;
}LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT;

typedef struct{
    uint16 mcc;
    uint16 mnc;
    uint16 tac;
}LIBLTE_MME_TRACKING_AREA_ID_STRUCT;

typedef enum{
    LIBLTE_MME_NON_DRX_TIMER_NO_NON_DRX_MODE = 0,
    LIBLTE_MME_NON_DRX_TIMER_MAX_1S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_2S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_4S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_8S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_16S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_32S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_64S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_N_ITEMS,
}LIBLTE_MME_NON_DRX_TIMER_ENUM;

typedef struct{
    LIBLTE_MME_NON_DRX_TIMER_ENUM non_drx_timer;
    uint8                         split_pg_cycle_code;
    uint8                         drx_cycle_len_coeff_and_value;
    uint8                         split_on_ccch;
}LIBLTE_MME_DRX_PARAMETER_STRUCT;

typedef enum{
    LIBLTE_MME_SS_SCREENING_INDICATOR_PHASE_1 = 0,
    LIBLTE_MME_SS_SCREENING_INDICATOR_PHASE_2,
    LIBLTE_MME_SS_SCREENING_INDICATOR_RESERVED_1,
    LIBLTE_MME_SS_SCREENING_INDICATOR_RESERVED_2,
    LIBLTE_MME_SS_SCREENING_INDICATOR_N_ITEMS,
}LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM;

typedef struct{
    LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM ss_screening;
    uint8                                   gea[8];
    uint8                                   sm_cap_ded;
    uint8                                   sm_cap_gprs;
    uint8                                   ucs2;
    uint8                                   solsa;
    uint8                                   revision;
    uint8                                   pfc;
    uint8                                   lcsva;
    uint8                                   ho_g2u_via_iu;
    uint8                                   ho_g2e_via_s1;
    uint8                                   emm_comb;
    uint8                                   isr;
    uint8                                   srvcc;
    uint8                                   epc;
    uint8                                   nf;
}LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT;

typedef struct{
    uint16 mcc;
    uint16 mnc;
    uint16 lac;
}LIBLTE_MME_LOCATION_AREA_ID_STRUCT;

typedef enum{
    LIBLTE_MME_REVISION_LEVEL_GSM_PHASE_1 = 0,
    LIBLTE_MME_REVISION_LEVEL_GSM_PHASE_2,
    LIBLTE_MME_REVISION_LEVEL_R99,
    LIBLTE_MME_REVISION_LEVEL_RESERVED,
    LIBLTE_MME_REVISION_LEVEL_N_ITEMS,
}LIBLTE_MME_REVISION_LEVEL_ENUM;

typedef enum{
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_1 = 0,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_2,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_3,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_4,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_5,
    LIBLTE_MME_RF_POWER_CAPABILITY_N_ITEMS,
}LIBLTE_MME_RF_POWER_CAPABILITY_ENUM;

typedef enum{
    LIBLTE_MME_SS_SCREEN_INDICATOR_0 = 0,
    LIBLTE_MME_SS_SCREEN_INDICATOR_1,
    LIBLTE_MME_SS_SCREEN_INDICATOR_2,
    LIBLTE_MME_SS_SCREEN_INDICATOR_3,
    LIBLTE_MME_SS_SCREEN_INDICATOR_N_ITEMS,
}LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM;

typedef struct{
    LIBLTE_MME_REVISION_LEVEL_ENUM      rev_lev;
    LIBLTE_MME_RF_POWER_CAPABILITY_ENUM rf_power_cap;
    LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM ss_screen_ind;
    uint8                                es_ind;
    uint8                                a5_1;
    uint8                                ps_cap;
    uint8                                sm_cap;
    uint8                                vbs;
    uint8                                vgcs;
    uint8                                fc;
    uint8                                cm3;
    uint8                                lcsva_cap;
    uint8                                ucs2;
    uint8                                solsa;
    uint8                                cmsp;
    uint8                                a5_3;
    uint8                                a5_2;
}LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT;

typedef struct{
    // FIXME
}LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT;

typedef struct{
    uint8  sys_id;
    uint16 codec_bitmap;
}LIBLTE_MME_SUPPORTED_CODEC_STRUCT;

#define LIBLTE_MAX_MSG_SIZE_BITS  102048
#define LIBLTE_MME_MAX_N_SUPPORTED_CODECS (LIBLTE_MAX_MSG_SIZE_BITS/16)
typedef struct{
    LIBLTE_MME_SUPPORTED_CODEC_STRUCT supported_codec[LIBLTE_MME_MAX_N_SUPPORTED_CODECS];
    uint32                            N_supported_codecs;
}LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT;

typedef enum{
    LIBLTE_MME_UE_USAGE_SETTING_VOICE_CENTRIC = 0,
    LIBLTE_MME_UE_USAGE_SETTING_DATA_CENTRIC,
    LIBLTE_MME_UE_USAGE_SETTING_N_ITEMS,
}LIBLTE_MME_UE_USAGE_SETTING_ENUM;

typedef enum{
    LIBLTE_MME_VOICE_DOMAIN_PREF_CS_ONLY = 0,
    LIBLTE_MME_VOICE_DOMAIN_PREF_PS_ONLY,
    LIBLTE_MME_VOICE_DOMAIN_PREF_CS_PREFFERED,
    LIBLTE_MME_VOICE_DOMAIN_PREF_PS_PREFFERED,
    LIBLTE_MME_VOICE_DOMAIN_PREF_N_ITEMS,
}LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM;

typedef struct{
    LIBLTE_MME_UE_USAGE_SETTING_ENUM  ue_usage_setting;
    LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM voice_domain_pref;
}LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT;

typedef enum{
    LIBLTE_MME_TMSI_STATUS_NO_VALID_TMSI = 0,
    LIBLTE_MME_TMSI_STATUS_VALID_TMSI,
    LIBLTE_MME_TMSI_STATUS_N_ITEMS,
}LIBLTE_MME_TMSI_STATUS_ENUM;

typedef enum{
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_NO_ADDITIONAL_INFO = 0,
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_SMS_ONLY,
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_N_ITEMS,
}LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM;

typedef enum{
    LIBLTE_MME_DEVICE_PROPERTIES_NOT_CONFIGURED_FOR_LOW_PRIORITY = 0,
    LIBLTE_MME_DEVICE_PROPERTIES_CONFIGURED_FOR_LOW_PRIORITY,
    LIBLTE_MME_DEVICE_PROPERTIES_N_ITEMS,
}LIBLTE_MME_DEVICE_PROPERTIES_ENUM;

typedef enum{
    LIBLTE_MME_GUTI_TYPE_NATIVE = 0,
    LIBLTE_MME_GUTI_TYPE_MAPPED,
    LIBLTE_MME_GUTI_TYPE_N_ITEMS,
}LIBLTE_MME_GUTI_TYPE_ENUM;

#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI 0x1
#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI 0x6
#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMEI 0x3

#define LIBLTE_MME_P_TMSI_SIGNATURE_IEI                       0x19
#define LIBLTE_MME_ADDITIONAL_GUTI_IEI                        0x50
#define LIBLTE_MME_LAST_VISITED_REGISTERED_TAI_IEI            0x52
#define LIBLTE_MME_DRX_PARAMETER_IEI                          0x5C
#define LIBLTE_MME_MS_NETWORK_CAPABILITY_IEI                  0x31
#define LIBLTE_MME_TMSI_STATUS_IEI                            0x9
#define LIBLTE_MME_MS_CLASSMARK_2_IEI                         0x11
#define LIBLTE_MME_MS_CLASSMARK_3_IEI                         0x20
#define LIBLTE_MME_SUPPORTED_CODEC_LIST_IEI                   0x40
#define LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_IEI                 0xF
#define LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_IEI 0x5D
#define LIBLTE_MME_ATTACH_REQUEST_DEVICE_PROPERTIES_IEI       0xD
#define LIBLTE_MME_GUTI_TYPE_IEI                              0xE

#define LIBLTE_MME_GUTI_IEI                         0x50
#define LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI 0x13
#define LIBLTE_MME_MS_IDENTITY_IEI                  0x23
#define LIBLTE_MME_EMM_CAUSE_IEI                    0x53
#define LIBLTE_MME_T3402_VALUE_IEI                  0x17
#define LIBLTE_MME_T3423_VALUE_IEI                  0x59
#define LIBLTE_MME_EQUIVALENT_PLMNS_IEI             0x4A
#define LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI        0x34
#define LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI  0x64
#define LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI     0xF
#define LIBLTE_MME_T3412_EXTENDED_VALUE_IEI         0x5E

typedef struct{
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT                         nas_ksi;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          eps_mobile_id;
    LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT                  ue_network_cap;
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT                            esm_msg;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          additional_guti;
    LIBLTE_MME_TRACKING_AREA_ID_STRUCT                       last_visited_registered_tai;
    LIBLTE_MME_DRX_PARAMETER_STRUCT                          drx_param;
    LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT                  ms_network_cap;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT                       old_lai;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT             ms_cm2;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT             ms_cm3;
    LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT                   supported_codecs;
    LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT voice_domain_pref_and_ue_usage_setting;
    LIBLTE_MME_TMSI_STATUS_ENUM                              tmsi_status;
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM                   additional_update_type;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM                        device_properties;
    LIBLTE_MME_GUTI_TYPE_ENUM                                old_guti_type;
    uint32                                                   old_p_tmsi_signature;
    uint8                                                    eps_attach_type;
    uint8                                                    old_p_tmsi_signature_present;
    uint8                                                    additional_guti_present;
    uint8                                                    last_visited_registered_tai_present;
    uint8                                                    drx_param_present;
    uint8                                                    ms_network_cap_present;
    uint8                                                    old_lai_present;
    uint8                                                    tmsi_status_present;
    uint8                                                    ms_cm2_present;
    uint8                                                    ms_cm3_present;
    uint8                                                    supported_codecs_present;
    uint8                                                    additional_update_type_present;
    uint8                                                    voice_domain_pref_and_ue_usage_setting_present;
    uint8                                                    device_properties_present;
    uint8                                                    old_guti_type_present;
}LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT;

extern LIBLTE_ERROR_ENUM liblte_mme_parse_msg_header(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, uint8 *pd, uint8 *msg_type);
extern LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT *msg, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer);

extern LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_complete_msg(LIBLTE_BIT_MSG_STRUCT *msg, LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete);

extern void convert_int_to_bits_vector(uint8 **bits, uint32 value, uint32 num_bits);
extern void convert_bytes_to_bits_vector(uint8 *bit_vector, uint8 *byte_buf, uint32 num_bits);

extern uint32 liblte_bits_2_value(uint8  **bits, uint32   N_bits);
extern LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_nas_ie(uint8 **ie_ptr, LIBLTE_SIMPLE_BYTE_MSG_STRUCT  *ded_info_nas);

extern void liblte_rrc_warning_not_handled(uint8 opt, const char *func_name);
extern void liblte_rrc_consume_noncrit_extension(uint8 ext, const char *func_name, uint8 **ie_ptr);

extern LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_response_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp);
extern LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_id_ie(uint8 **ie_ptr, LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id);
extern LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_request_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req);
#endif /* ASN1LIB_H_ */
