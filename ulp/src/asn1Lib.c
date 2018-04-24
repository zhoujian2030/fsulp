/*
 * asn1Lib.c
 *
 *  Created on: Sep 28, 2017
 *      Author: J.ZH
 */

#include <stdio.h>
#include "asn1Lib.h"

/*********************************************************************
    Message Name: Message Header (Plain NAS Message)

    Description: Message header for plain NAS messages.

    Document Reference: 24.301 v10.2.0 Section 9.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_parse_msg_header(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, uint8 *pd, uint8 *msg_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             sec_hdr_type;

    if(msg      != NULL &&
       pd       != NULL &&
       msg_type != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;

        // Protocol Discriminator
        *pd = msg->msg[0] & 0x0F;

        if(LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST == sec_hdr_type)
        {
            *msg_type = LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST;
        }else{
            if(LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT == *pd)
            {
                // Message Type
                *msg_type = msg->msg[2];
            }else{
                if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
                {
                    // Message Type
                    *msg_type = msg->msg[1];
                }else{
                    // Protocol Discriminator
                    *pd = msg->msg[6] & 0x0F;

                    if(LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT == *pd)
                    {
                        *msg_type = msg->msg[8];
                    }else{
                        *msg_type = msg->msg[7];
                    }
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_information_transfer_msg( LIBLTE_BIT_MSG_STRUCT *msg, LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg              != NULL &&
       ul_info_transfer != NULL)
    {
        // Extension choice
        uint8 ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);;

        // Dedicated info type choice
        ul_info_transfer->dedicated_info_type = (LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        if(LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS == ul_info_transfer->dedicated_info_type)
        {
        	liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr,
                                                    &ul_info_transfer->dedicated_info);
        }else{
            printf("[%s], Invalid dedicated_info_type = %d\n", __func__, ul_info_transfer->dedicated_info_type);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

void convert_int_to_bits_vector(uint8 **bits, uint32 value, uint32 num_bits)
{
    uint32 i;

    for(i=0; i<num_bits; i++) {
        (*bits)[i] = (value >> (num_bits-i-1)) & 0x1;
    }
    *bits += num_bits;
}

void convert_bytes_to_bits_vector(uint8 *bit_vector, uint8 *byte_buf, uint32 num_bits)
{
    uint32 i, nbytes;
    nbytes = num_bits >> 3;
    for (i=0; i<nbytes; i++) {
        convert_int_to_bits_vector(&bit_vector, byte_buf[i], 8);
    }

    uint32 temp = nbytes << 3;
    if (num_bits > temp) {
        convert_int_to_bits_vector(&bit_vector, byte_buf[i] >> (8 - (num_bits - temp)), (num_bits - temp));
    }
}

/*********************************************************************
    Name: liblte_bits_2_value

    Description: Converts a bit string to a value
*********************************************************************/
uint32 liblte_bits_2_value(uint8  **bits,
                           uint32   N_bits)
{
    uint32 value = 0;
    uint32 i;

    for(i=0; i<N_bits; i++)
    {
        value |= (*bits)[i] << (N_bits-i-1);
    }
    *bits += N_bits;

    return(value);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_nas_ie(uint8                         **ie_ptr,
                                                          LIBLTE_SIMPLE_BYTE_MSG_STRUCT  *ded_info_nas)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr       != NULL &&
       ded_info_nas != NULL)
    {
        if(0 == liblte_bits_2_value(ie_ptr, 1))
        {
            ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 7);
        }else{
            if(0 == liblte_bits_2_value(ie_ptr, 1))
            {
                ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 14);
            }else{
                // FIXME: Unlikely to have more than 16K of octets
                ded_info_nas->N_bytes = 0;
            }
        }

        for(i=0; i<ded_info_nas->N_bytes; i++)
        {
            ded_info_nas->msg[i] = liblte_bits_2_value(ie_ptr, 8);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

void liblte_rrc_consume_noncrit_extension(uint8 ext, const char *func_name, uint8 **ie_ptr)
{
	// todo empty
	// printf("liblte_rrc_consume_noncrit_extension\n");
}

void liblte_rrc_warning_not_handled(uint8 opt, const char *func_name)
{
  if (opt) {
	//   printf("Unhandled feature in RRC function: %s\n\n", func_name?func_name:"Unknown");
  }
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_response_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg     != NULL &&
       id_resp != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Mobile Identity
        liblte_mme_unpack_mobile_id_ie(&msg_ptr, &id_resp->mobile_id);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_id_ie(uint8 **ie_ptr, LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id;
//    uint32             length;
    uint32             i;
    uint8               odd = FALSE;

    if(ie_ptr    != NULL &&
       mobile_id != NULL)
    {
//        length   = **ie_ptr;
        *ie_ptr += 1;

        mobile_id->type_of_id = **ie_ptr & 0x07;

        if(LIBLTE_MME_MOBILE_ID_TYPE_IMSI == mobile_id->type_of_id)
        {
            id  = mobile_id->imsi;
            odd = TRUE;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEI == mobile_id->type_of_id){
            id  = mobile_id->imei;
            odd = TRUE;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEISV == mobile_id->type_of_id){
            id  = mobile_id->imeisv;
            odd = FALSE;
        }else{
            // FIXME: Not handling these IDs
            return(err);
        }

        id[0]    = **ie_ptr >> 4;
        *ie_ptr += 1;
        for(i=0; i<7; i++)
        {
            id[i*2+1] = (*ie_ptr)[i] & 0x0F;
            id[i*2+2] = (*ie_ptr)[i] >> 4;
        }
        if(odd)
        {
            *ie_ptr += 7;
        }else{
            id[i*2+1]  = (*ie_ptr)[i] & 0xF;
            *ie_ptr   += 8;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
