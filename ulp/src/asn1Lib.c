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
    int32             nBits = msg->N_bits;

    if(msg              != NULL &&
       ul_info_transfer != NULL)
    {
        // Extension choice
        uint8 ext = liblte_bits_2_value(&msg_ptr, 1);
        nBits -= 1;

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);
        nBits -= 2;

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);;
        nBits -= 1;

        // Dedicated info type choice
        ul_info_transfer->dedicated_info_type = (LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 2);
        nBits -= 2;

        if(LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS == ul_info_transfer->dedicated_info_type) {
        	liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr, &ul_info_transfer->dedicated_info, &nBits);
        } else {
            //printf("Invalid dedicated_info_type = %d\n", ul_info_transfer->dedicated_info_type);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        if (nBits >= 0) {
            err = LIBLTE_SUCCESS;
        }
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
                                                          LIBLTE_SIMPLE_BYTE_MSG_STRUCT  *ded_info_nas,
                                                          int32 *nBits_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr       != NULL &&
       ded_info_nas != NULL &&
       nBits_ptr    != NULL)
    {
        if(0 == liblte_bits_2_value(ie_ptr, 1)) {
            ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 7);
            *nBits_ptr = *nBits_ptr - 7;
        } else {
            if(0 == liblte_bits_2_value(ie_ptr, 1)) {
                ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 14);
                *nBits_ptr = *nBits_ptr - 14;
            } else {
                // FIXME: Unlikely to have more than 16K of octets
                ded_info_nas->N_bytes = 0;
            }
            *nBits_ptr = *nBits_ptr - 1;
        }
        *nBits_ptr = *nBits_ptr - 1;

        //printf("N_bytes = %d, *nBits_ptr = %d\n", ded_info_nas->N_bytes, *nBits_ptr);
        if ((*nBits_ptr < 0) || (ded_info_nas->N_bytes == 0) || (ded_info_nas->N_bytes > LIBLTE_MAX_MSG_SIZE_BYTES) ||
            ((ded_info_nas->N_bytes << 3) > (*nBits_ptr + 7))) {
            // printf("N_bytes = %d, *nBits_ptr = %d\n", ded_info_nas->N_bytes, *nBits_ptr);
            ded_info_nas->N_bytes = 0;
            *nBits_ptr = -1;
            return err;
        }

        for(i=0; i<ded_info_nas->N_bytes; i++) {
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
        err = liblte_mme_unpack_mobile_id_ie(&msg_ptr, &id_resp->mobile_id);
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

LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_transaction_identifier_ie(uint8 **ie_ptr,
                                                                  uint8  *rrc_transaction_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr             != NULL &&
       rrc_transaction_id != NULL)
    {
        *rrc_transaction_id = liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_plmn_identity_ie(uint8                           **ie_ptr,
                                                     LIBLTE_RRC_PLMN_IDENTITY_STRUCT  *plmn_id,
                                                     int32* nBits_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             mcc_opt;
    uint8             mnc_size;

    if(ie_ptr  != NULL &&
       plmn_id != NULL)
    {
        mcc_opt = liblte_bits_2_value(ie_ptr, 1);
        *nBits_ptr = *nBits_ptr - 1;

        if(TRUE == mcc_opt)
        {
            plmn_id->mcc  = 0xF000;
            plmn_id->mcc |= (liblte_bits_2_value(ie_ptr, 4) << 8);
            plmn_id->mcc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mcc |= liblte_bits_2_value(ie_ptr, 4);
            *nBits_ptr = *nBits_ptr - 12;

        }else{
            plmn_id->mcc = LIBLTE_RRC_MCC_NOT_PRESENT;
        }

        mnc_size     = (liblte_bits_2_value(ie_ptr, 1) + 2);
        *nBits_ptr = *nBits_ptr - 1;
        if(2 == mnc_size)
        {
            plmn_id->mnc  = 0xFF00;
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mnc |= liblte_bits_2_value(ie_ptr, 4);
            *nBits_ptr = *nBits_ptr - 8;
        }else{
            plmn_id->mnc  = 0xF000;
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 8);
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mnc |= liblte_bits_2_value(ie_ptr, 4);
            *nBits_ptr = *nBits_ptr - 12;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_mmec_ie(uint8 **ie_ptr,
                                            uint8  *mmec)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       mmec   != NULL)
    {
        *mmec = liblte_bits_2_value(ie_ptr, 8);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

// ----------------------------------
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_complete_msg(
   LIBLTE_BIT_MSG_STRUCT *msg,
   LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete)
{
   LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
   uint8             *msg_ptr = msg->msg;
   int32             nBits = msg->N_bits;

   if(msg                != NULL &&
      con_setup_complete != NULL)
   {
       // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr, &con_setup_complete->rrc_transaction_id);
        nBits -= 2;

        // Extension choice
        uint8 ext = liblte_bits_2_value(&msg_ptr, 1);
        nBits -= 1;

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);
        nBits -= 2;

        // Optional indicators
        con_setup_complete->registered_mme_present = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);
        nBits -= 2;

        // Selected PLMN identity
        con_setup_complete->selected_plmn_id = liblte_bits_2_value(&msg_ptr, 3) + 1;
        nBits -= 3;

        // Registered MME
        if(con_setup_complete->registered_mme_present)
        {
            // Optional indicator
            con_setup_complete->registered_mme.plmn_id_present = liblte_bits_2_value(&msg_ptr, 1);
            nBits -= 1;

            // PLMN identity
            if(con_setup_complete->registered_mme.plmn_id_present)
            {
                liblte_rrc_unpack_plmn_identity_ie(&msg_ptr, &con_setup_complete->registered_mme.plmn_id, &nBits);
            }

            // MMEGI
            con_setup_complete->registered_mme.mmegi = liblte_bits_2_value(&msg_ptr, 16);
            nBits -= 16;

            // MMEC
            liblte_rrc_unpack_mmec_ie(&msg_ptr, &con_setup_complete->registered_mme.mmec);
            nBits -= 8;
        }

        // Dedicated info NAS
        liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr, &con_setup_complete->dedicated_info_nas, &nBits);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        if (nBits >= 0) {
            err = LIBLTE_SUCCESS;
        }
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_attach_type_ie(uint8 **ie_ptr,
                                                       uint8   bit_offset,
                                                       uint8  *attach_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       attach_type != NULL)
    {
        *attach_type = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_key_set_id_ie(uint8                            **ie_ptr,
                                                      uint8                              bit_offset,
                                                      LIBLTE_MME_NAS_KEY_SET_ID_STRUCT  *nas_ksi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       nas_ksi != NULL)
    {
        nas_ksi->tsc_flag = (LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM)((**ie_ptr >> (bit_offset + 3)) & 0x01);
        nas_ksi->nas_ksi  = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT  *ue_network_cap)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            length;

    if(ie_ptr         != NULL &&
       ue_network_cap != NULL)
    {
        length                  = **ie_ptr;
        *ie_ptr                += 1;
        ue_network_cap->eea[0]  = (**ie_ptr >> 7) & 0x01;
        ue_network_cap->eea[1]  = (**ie_ptr >> 6) & 0x01;
        ue_network_cap->eea[2]  = (**ie_ptr >> 5) & 0x01;
        ue_network_cap->eea[3]  = (**ie_ptr >> 4) & 0x01;
        ue_network_cap->eea[4]  = (**ie_ptr >> 3) & 0x01;
        ue_network_cap->eea[5]  = (**ie_ptr >> 2) & 0x01;
        ue_network_cap->eea[6]  = (**ie_ptr >> 1) & 0x01;
        ue_network_cap->eea[7]  = **ie_ptr & 0x01;
        *ie_ptr                += 1;
        ue_network_cap->eia[0]  = (**ie_ptr >> 7) & 0x01;
        ue_network_cap->eia[1]  = (**ie_ptr >> 6) & 0x01;
        ue_network_cap->eia[2]  = (**ie_ptr >> 5) & 0x01;
        ue_network_cap->eia[3]  = (**ie_ptr >> 4) & 0x01;
        ue_network_cap->eia[4]  = (**ie_ptr >> 3) & 0x01;
        ue_network_cap->eia[5]  = (**ie_ptr >> 2) & 0x01;
        ue_network_cap->eia[6]  = (**ie_ptr >> 1) & 0x01;
        ue_network_cap->eia[7]  = **ie_ptr & 0x01;
        *ie_ptr                += 1;
        if(length > 2)
        {
            ue_network_cap->uea[0]       = (**ie_ptr >> 7) & 0x01;
            ue_network_cap->uea[1]       = (**ie_ptr >> 6) & 0x01;
            ue_network_cap->uea[2]       = (**ie_ptr >> 5) & 0x01;
            ue_network_cap->uea[3]       = (**ie_ptr >> 4) & 0x01;
            ue_network_cap->uea[4]       = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->uea[5]       = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->uea[6]       = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->uea[7]       = **ie_ptr & 0x01;
            ue_network_cap->uea_present  = TRUE;
            *ie_ptr                     += 1;
        }else{
            ue_network_cap->uea_present = FALSE;
        }
        if(length > 3)
        {
            ue_network_cap->ucs2          = (**ie_ptr >> 7) & 0x01;
            ue_network_cap->ucs2_present  = TRUE;
            ue_network_cap->uia[1]        = (**ie_ptr >> 6) & 0x01;
            ue_network_cap->uia[2]        = (**ie_ptr >> 5) & 0x01;
            ue_network_cap->uia[3]        = (**ie_ptr >> 4) & 0x01;
            ue_network_cap->uia[4]        = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->uia[5]        = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->uia[6]        = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->uia[7]        = **ie_ptr & 0x01;
            ue_network_cap->uia_present   = TRUE;
            *ie_ptr                      += 1;
        }else{
            ue_network_cap->ucs2_present = FALSE;
            ue_network_cap->uia_present  = FALSE;
        }
        if(length > 4)
        {
            ue_network_cap->lpp                = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->lpp_present        = TRUE;
            ue_network_cap->lcs                = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->lcs_present        = TRUE;
            ue_network_cap->onexsrvcc          = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->onexsrvcc_present  = TRUE;
            ue_network_cap->nf                 = **ie_ptr >> 1;
            ue_network_cap->nf_present         = TRUE;
            *ie_ptr                           += 1;
        }else{
            ue_network_cap->lpp_present       = FALSE;
            ue_network_cap->lcs_present       = FALSE;
            ue_network_cap->onexsrvcc_present = FALSE;
            ue_network_cap->nf_present        = FALSE;
        }
        if(length > 5)
        {
            *ie_ptr += length-5;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_message_container_ie(uint8                  **ie_ptr,
                                                             LIBLTE_SIMPLE_BYTE_MSG_STRUCT  *esm_msg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr  != NULL &&
       esm_msg != NULL)
    {
        esm_msg->N_bytes  = (*ie_ptr)[0] << 8;
        esm_msg->N_bytes |= (*ie_ptr)[1];

        if (esm_msg->N_bytes > LIBLTE_MAX_MSG_SIZE_BYTES) {
            // TODO
            esm_msg->N_bytes = 0;
            return err;
        }

        for(i=0; i<esm_msg->N_bytes; i++)
        {
            esm_msg->msg[i]  = (*ie_ptr)[2+i];
        }
        *ie_ptr += esm_msg->N_bytes + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_p_tmsi_signature_ie(uint8  **ie_ptr,
                                                        uint32  *p_tmsi_signature)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr           != NULL &&
       p_tmsi_signature != NULL)
    {
        *p_tmsi_signature  = (*ie_ptr)[0] << 24;
        *p_tmsi_signature |= (*ie_ptr)[1] << 16;
        *p_tmsi_signature |= (*ie_ptr)[2] << 8;
        *p_tmsi_signature |= (*ie_ptr)[3];
        *ie_ptr           += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_TRACKING_AREA_ID_STRUCT  *tai)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       tai    != NULL)
    {
        tai->mcc  = ((*ie_ptr)[0] & 0x0F)*100;
        tai->mcc += (((*ie_ptr)[0] >> 4) & 0x0F)*10;
        tai->mcc += (*ie_ptr)[1] & 0x0F;
        if((((*ie_ptr)[1] >> 4) & 0x0F) == 0x0F)
        {
            tai->mnc  = ((*ie_ptr)[2] & 0x0F)*10;
            tai->mnc += ((*ie_ptr)[2] >> 4) & 0x0F;
        }else{
            tai->mnc  = ((*ie_ptr)[1] >> 4) & 0x0F;
            tai->mnc += ((*ie_ptr)[2] & 0x0F)*100;
            tai->mnc += (((*ie_ptr)[2] >> 4) & 0x0F)*10;
        }
        tai->tac  = (*ie_ptr)[3] << 8;
        tai->tac |= (*ie_ptr)[4];
        *ie_ptr  += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_drx_parameter_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_DRX_PARAMETER_STRUCT  *drx_param)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       drx_param != NULL)
    {
        drx_param->split_pg_cycle_code            = (*ie_ptr)[0];
        drx_param->drx_cycle_len_coeff_and_value  = ((*ie_ptr)[1] >> 4) & 0x0F;
        drx_param->split_on_ccch                  = ((*ie_ptr)[1] >> 3) & 0x01;
        drx_param->non_drx_timer                  = (LIBLTE_MME_NON_DRX_TIMER_ENUM)((*ie_ptr)[1] & 0x07);
        *ie_ptr                                  += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_ms_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT  *ms_network_cap)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       ms_network_cap != NULL)
    {
        ms_network_cap->gea[1]         = ((*ie_ptr)[1] >> 7) & 0x01;
        ms_network_cap->sm_cap_ded     = ((*ie_ptr)[1] >> 6) & 0x01;
        ms_network_cap->sm_cap_gprs    = ((*ie_ptr)[1] >> 5) & 0x01;
        ms_network_cap->ucs2           = ((*ie_ptr)[1] >> 4) & 0x01;
        ms_network_cap->ss_screening   = (LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM)(((*ie_ptr)[1] >> 2) & 0x03);
        ms_network_cap->solsa          = ((*ie_ptr)[1] >> 1) & 0x01;
        ms_network_cap->revision       = (*ie_ptr)[1] & 0x01;
        ms_network_cap->pfc            = ((*ie_ptr)[2] >> 7) & 0x01;
        ms_network_cap->gea[2]         = ((*ie_ptr)[2] >> 6) & 0x01;
        ms_network_cap->gea[3]         = ((*ie_ptr)[2] >> 5) & 0x01;
        ms_network_cap->gea[4]         = ((*ie_ptr)[2] >> 4) & 0x01;
        ms_network_cap->gea[5]         = ((*ie_ptr)[2] >> 3) & 0x01;
        ms_network_cap->gea[6]         = ((*ie_ptr)[2] >> 2) & 0x01;
        ms_network_cap->gea[7]         = ((*ie_ptr)[2] >> 1) & 0x01;
        ms_network_cap->lcsva          = (*ie_ptr)[2] & 0x01;
        ms_network_cap->ho_g2u_via_iu  = ((*ie_ptr)[3] >> 7) & 0x01;
        ms_network_cap->ho_g2e_via_s1  = ((*ie_ptr)[3] >> 6) & 0x01;
        ms_network_cap->emm_comb       = ((*ie_ptr)[3] >> 5) & 0x01;
        ms_network_cap->isr            = ((*ie_ptr)[3] >> 4) & 0x01;
        ms_network_cap->srvcc          = ((*ie_ptr)[3] >> 3) & 0x01;
        ms_network_cap->epc            = ((*ie_ptr)[3] >> 2) & 0x01;
        ms_network_cap->nf             = ((*ie_ptr)[3] >> 1) & 0x01;
        *ie_ptr                       += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_location_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_LOCATION_AREA_ID_STRUCT  *lai)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       lai    != NULL)
    {
        lai->mcc  = ((*ie_ptr)[0] & 0x0F)*100;
        lai->mcc += (((*ie_ptr)[0] >> 4) & 0x0F)*10;
        lai->mcc += (*ie_ptr)[1] & 0x0F;
        if((((*ie_ptr)[1] >> 4) & 0x0F) == 0x0F)
        {
            lai->mnc  = ((*ie_ptr)[2] & 0x0F)*10;
            lai->mnc += ((*ie_ptr)[2] >> 4) & 0x0F;
        }else{
            lai->mnc  = ((*ie_ptr)[1] >> 4) & 0x0F;
            lai->mnc += ((*ie_ptr)[2] & 0x0F)*100;
            lai->mnc += (((*ie_ptr)[2] >> 4) & 0x0F)*10;
        }
        lai->lac  = (*ie_ptr)[3] << 8;
        lai->lac |= (*ie_ptr)[4];
        *ie_ptr  += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_tmsi_status_ie(uint8                       **ie_ptr,
                                                   uint8                         bit_offset,
                                                   LIBLTE_MME_TMSI_STATUS_ENUM  *tmsi_status)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       tmsi_status != NULL)
    {
        *tmsi_status = (LIBLTE_MME_TMSI_STATUS_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_2_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT  *ms_cm2)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ms_cm2 != NULL)
    {
        ms_cm2->rev_lev        = (LIBLTE_MME_REVISION_LEVEL_ENUM)(((*ie_ptr)[1] >> 5) & 0x03);
        ms_cm2->es_ind         = ((*ie_ptr)[1] >> 4) & 0x01;
        ms_cm2->a5_1           = ((*ie_ptr)[1] >> 3) & 0x01;
        ms_cm2->rf_power_cap   = (LIBLTE_MME_RF_POWER_CAPABILITY_ENUM)((*ie_ptr)[1] & 0x07);
        ms_cm2->ps_cap         = ((*ie_ptr)[2] >> 6) & 0x01;
        ms_cm2->ss_screen_ind  = (LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM)(((*ie_ptr)[2] >> 4) & 0x03);
        ms_cm2->sm_cap         = ((*ie_ptr)[2] >> 3) & 0x01;
        ms_cm2->vbs            = ((*ie_ptr)[2] >> 2) & 0x01;
        ms_cm2->vgcs           = ((*ie_ptr)[2] >> 1) & 0x01;
        ms_cm2->fc             = (*ie_ptr)[2] & 0x01;
        ms_cm2->cm3            = ((*ie_ptr)[3] >> 7) & 0x01;
        ms_cm2->lcsva_cap      = ((*ie_ptr)[3] >> 5) & 0x01;
        ms_cm2->ucs2           = ((*ie_ptr)[3] >> 4) & 0x01;
        ms_cm2->solsa          = ((*ie_ptr)[3] >> 3) & 0x01;
        ms_cm2->cmsp           = ((*ie_ptr)[3] >> 2) & 0x01;
        ms_cm2->a5_3           = ((*ie_ptr)[3] >> 1) & 0x01;
        ms_cm2->a5_2           = (*ie_ptr)[3] & 0x01;
        *ie_ptr               += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_3_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT  *ms_cm3)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ms_cm3 != NULL)
    {
        // FIXME

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_supported_codec_list_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT  *supported_codec_list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr               != NULL &&
       supported_codec_list != NULL)
    {
        supported_codec_list->N_supported_codecs = ((*ie_ptr)[0]/4);
        for(i=0; i<supported_codec_list->N_supported_codecs; i++)
        {
            supported_codec_list->supported_codec[i].sys_id        = (*ie_ptr)[1+i*4+0];
            supported_codec_list->supported_codec[i].codec_bitmap  = (*ie_ptr)[1+i*4+2] << 8;
            supported_codec_list->supported_codec[i].codec_bitmap |= (*ie_ptr)[1+i*4+3];
        }
        *ie_ptr += (supported_codec_list->N_supported_codecs*4) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_update_type_ie(uint8                                  **ie_ptr,
                                                              uint8                                    bit_offset,
                                                              LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM  *aut)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       aut    != NULL)
    {
        *aut = (LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_voice_domain_pref_and_ue_usage_setting_ie(uint8                                                    **ie_ptr,
                                                                              LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT  *voice_domain_pref_and_ue_usage_setting)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                                 != NULL &&
       voice_domain_pref_and_ue_usage_setting != NULL)
    {
        *ie_ptr                                                   += 1;
        voice_domain_pref_and_ue_usage_setting->ue_usage_setting   = (LIBLTE_MME_UE_USAGE_SETTING_ENUM)((**ie_ptr >> 2) & 0x01);
        voice_domain_pref_and_ue_usage_setting->voice_domain_pref  = (LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM)(**ie_ptr & 0x03);
        *ie_ptr                                                   += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_device_properties_ie(uint8                             **ie_ptr,
                                                         uint8                               bit_offset,
                                                         LIBLTE_MME_DEVICE_PROPERTIES_ENUM  *device_props)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       device_props != NULL)
    {
        *device_props = (LIBLTE_MME_DEVICE_PROPERTIES_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_type_ie(uint8                     **ie_ptr,
                                                 uint8                       bit_offset,
                                                 LIBLTE_MME_GUTI_TYPE_ENUM  *guti_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       guti_type != NULL)
    {
        *guti_type = (LIBLTE_MME_GUTI_TYPE_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_mobile_id_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_EPS_MOBILE_ID_STRUCT  *eps_mobile_id)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id;
    uint32             length;
    uint32             i;

    if(ie_ptr        != NULL &&
       eps_mobile_id != NULL)
    {
        length   = **ie_ptr;
        *ie_ptr += 1;

        eps_mobile_id->type_of_id = **ie_ptr & 0x07;

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == eps_mobile_id->type_of_id ) {
            *ie_ptr                 += 1;
            eps_mobile_id->guti.mcc  = (**ie_ptr & 0x0F)*100;
            eps_mobile_id->guti.mcc += ((**ie_ptr >> 4) & 0x0F)*10;
            *ie_ptr                 += 1;
            eps_mobile_id->guti.mcc += **ie_ptr & 0x0F;
            if(((**ie_ptr >> 4) & 0x0F) == 0x0F)
            {
                *ie_ptr                 += 1;
                eps_mobile_id->guti.mnc  = (**ie_ptr & 0x0F)*10;
                eps_mobile_id->guti.mnc += (**ie_ptr >> 4) & 0x0F;
                *ie_ptr                 += 1;
            }else{
                eps_mobile_id->guti.mnc  = (**ie_ptr >> 4) & 0x0F;
                *ie_ptr                 += 1;
                eps_mobile_id->guti.mnc += (**ie_ptr & 0x0F)*100;
                eps_mobile_id->guti.mnc += ((**ie_ptr >> 4) & 0x0F)*10;
                *ie_ptr                 += 1;
            }
            eps_mobile_id->guti.mme_group_id  = **ie_ptr << 8;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.mme_group_id |= **ie_ptr;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.mme_code      = **ie_ptr;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi        = **ie_ptr << 24;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr << 16;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr << 8;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr;
            *ie_ptr                          += 1;
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_TMSI == eps_mobile_id->type_of_id) {
            *ie_ptr  += 1;
            eps_mobile_id->m_tmsi = **ie_ptr << 24;
            *ie_ptr  += 1;
            eps_mobile_id->m_tmsi |= **ie_ptr << 16;
            *ie_ptr  += 1;
            eps_mobile_id->m_tmsi |= **ie_ptr << 8;
            *ie_ptr  += 1;
            eps_mobile_id->m_tmsi |= **ie_ptr;
            *ie_ptr  += 1;
        } else {
            if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == eps_mobile_id->type_of_id)
            {
                id = eps_mobile_id->imsi;
            } else {
                id = eps_mobile_id->imei;
            }

            id[0]    = **ie_ptr >> 4;
            *ie_ptr += 1;
            for(i=0; i<7; i++)
            {
                id[i*2+1]  = **ie_ptr & 0x0F;
                id[i*2+2]  = **ie_ptr >> 4;
                *ie_ptr   += 1;
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_request_msg(
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg,
    LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       attach_req != NULL)
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

        // EPS Attach Type & NAS Key Set Identifier
        liblte_mme_unpack_eps_attach_type_ie(&msg_ptr, 0, &attach_req->eps_attach_type);
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &attach_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &attach_req->eps_mobile_id);

        // UE Network Capability
        liblte_mme_unpack_ue_network_capability_ie(&msg_ptr, &attach_req->ue_network_cap);

        // ESM Message Container
        if (LIBLTE_SUCCESS != liblte_mme_unpack_esm_message_container_ie(&msg_ptr, &attach_req->esm_msg)) {
            return err;
        }

        // Old P-TMSI Signature
        if(LIBLTE_MME_P_TMSI_SIGNATURE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_p_tmsi_signature_ie(&msg_ptr, &attach_req->old_p_tmsi_signature);
            attach_req->old_p_tmsi_signature_present = TRUE;
        }else{
            attach_req->old_p_tmsi_signature_present = FALSE;
        }

        // Additional GUTI
        if(LIBLTE_MME_ADDITIONAL_GUTI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &attach_req->additional_guti);
            attach_req->additional_guti_present = TRUE;
        }else{
            attach_req->additional_guti_present = FALSE;
        }

        // Last Visited Registered TAI
        if(LIBLTE_MME_LAST_VISITED_REGISTERED_TAI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_tracking_area_id_ie(&msg_ptr, &attach_req->last_visited_registered_tai);
            attach_req->last_visited_registered_tai_present = TRUE;
        }else{
            attach_req->last_visited_registered_tai_present = FALSE;
        }

        // DRX Parameter
        if(LIBLTE_MME_DRX_PARAMETER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_drx_parameter_ie(&msg_ptr, &attach_req->drx_param);
            attach_req->drx_param_present = TRUE;
        }else{
            attach_req->drx_param_present = FALSE;
        }

        // MS Network Capability
        if(LIBLTE_MME_MS_NETWORK_CAPABILITY_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_ms_network_capability_ie(&msg_ptr, &attach_req->ms_network_cap);
            attach_req->ms_network_cap_present = TRUE;
        }else{
            attach_req->ms_network_cap_present = FALSE;
        }

        // Old Location Area ID
        if(LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_location_area_id_ie(&msg_ptr, &attach_req->old_lai);
            attach_req->old_lai_present = TRUE;
        }else{
            attach_req->old_lai_present = FALSE;
        }

        // TMSI Status
        if((LIBLTE_MME_TMSI_STATUS_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_tmsi_status_ie(&msg_ptr, 0, &attach_req->tmsi_status);
            msg_ptr++;
            attach_req->tmsi_status_present = TRUE;
        }else{
            attach_req->tmsi_status_present = FALSE;
        }

        // Mobile Station Classmark 2
        if(LIBLTE_MME_MS_CLASSMARK_2_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_station_classmark_2_ie(&msg_ptr, &attach_req->ms_cm2);
            attach_req->ms_cm2_present = TRUE;
        }else{
            attach_req->ms_cm2_present = FALSE;
        }

        // Mobile Station Classmark 3
        if(LIBLTE_MME_MS_CLASSMARK_3_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_station_classmark_3_ie(&msg_ptr, &attach_req->ms_cm3);
            attach_req->ms_cm3_present = TRUE;
        }else{
            attach_req->ms_cm3_present = FALSE;
        }

        // Supported Codecs
        if(LIBLTE_MME_SUPPORTED_CODEC_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_supported_codec_list_ie(&msg_ptr, &attach_req->supported_codecs);
            attach_req->supported_codecs_present = TRUE;
        }else{
            attach_req->supported_codecs_present = FALSE;
        }

        // Additional Update Type
        if((LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_additional_update_type_ie(&msg_ptr, 0, &attach_req->additional_update_type);
            msg_ptr++;
            attach_req->additional_update_type_present = TRUE;
        }else{
            attach_req->additional_update_type_present = FALSE;
        }

        // Voice Domain Preference and UE's Usage Setting
        if(LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_voice_domain_pref_and_ue_usage_setting_ie(&msg_ptr, &attach_req->voice_domain_pref_and_ue_usage_setting);
            attach_req->voice_domain_pref_and_ue_usage_setting_present = TRUE;
        }else{
            attach_req->voice_domain_pref_and_ue_usage_setting_present = FALSE;
        }

        // Device Properties
        if((LIBLTE_MME_ATTACH_REQUEST_DEVICE_PROPERTIES_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &attach_req->device_properties);
            msg_ptr++;
            attach_req->device_properties_present = TRUE;
        }else{
            attach_req->device_properties_present = FALSE;
        }

        // Old GUTI Type
        if((LIBLTE_MME_GUTI_TYPE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_guti_type_ie(&msg_ptr, 0, &attach_req->old_guti_type);
            msg_ptr++;
            attach_req->old_guti_type_present = TRUE;
        }else{
            attach_req->old_guti_type_present = FALSE;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_type_ie(uint8                         **ie_ptr,
                                                   uint8                           bit_offset,
                                                   LIBLTE_MME_DETACH_TYPE_STRUCT  *detach_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       detach_type != NULL)
    {
        detach_type->switch_off     = ((*ie_ptr)[0] >> (3 + bit_offset)) & 0x01;
        detach_type->type_of_detach = ((*ie_ptr)[0] >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_request_msg(
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg,
    LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT *detach_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       detach_req != NULL)
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

        // Detach Type & NAS Key Set Identifier
        liblte_mme_unpack_detach_type_ie(&msg_ptr, 0, &detach_req->detach_type);
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &detach_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &detach_req->eps_mobile_id);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

// -------------------------------------------
LIBLTE_ERROR_ENUM liblte_mme_unpack_ext_service_request_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT *ext_service_req)
{
    // refer to 3gpp 24.301 8.2.15
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg != NULL && ext_service_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type) {
            msg_ptr++;
        } else {
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // service type
        ext_service_req->service_type = *msg_ptr & 0x0f;
        //NAS Key Set Identifier
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &ext_service_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &ext_service_req->eps_mobile_id);

        // printf("[%s], service_type = %d, tsc_flag = %d, nas_ksi = %d\n", __func__, ext_service_req->service_type,
        //     ext_service_req->nas_ksi.tsc_flag, ext_service_req->nas_ksi.nas_ksi);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

// -------------------------------------------
LIBLTE_ERROR_ENUM liblte_mme_unpack_tau_request_msg(LIBLTE_SIMPLE_BYTE_MSG_STRUCT *msg, LIBLTE_MME_TAU_REQ_STRUCT *tau_req)
{
    // refer to 3gpp 24.301 8.2.29
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg != NULL && tau_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type) {
            msg_ptr++;
        } else {
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // eps update type
        tau_req->eps_update_type.value = *msg_ptr & 0x07;
        tau_req->eps_update_type.activeFlag = (*msg_ptr & 0x08) >> 3;
        //NAS Key Set Identifier
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &tau_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &tau_req->eps_mobile_id);

        // printf("[%s], eps_update_type.value = %d, eps_update_type.activeFlag = %d, tsc_flag = %d, nas_ksi = %d\n", __func__, tau_req->eps_update_type.value,
        //     tau_req->eps_update_type.activeFlag, tau_req->nas_ksi.tsc_flag, tau_req->nas_ksi.nas_ksi);

        err = LIBLTE_SUCCESS;
    }

    return(err);   
}

// -------------------------------------------
LIBLTE_ERROR_ENUM liblte_rrc_unpack_s_tmsi_ie(uint8 **ie_ptr, LIBLTE_RRC_S_TMSI_STRUCT  *s_tmsi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL && s_tmsi != NULL)
    {
        liblte_rrc_unpack_mmec_ie(ie_ptr, &s_tmsi->mmec);
        s_tmsi->m_tmsi = liblte_bits_2_value(ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

// -------------------------------------------
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_request_msg(LIBLTE_BIT_MSG_STRUCT *msg, LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *con_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg != NULL && con_req != NULL)
    {
        // Extension Choice
        uint8 ext = liblte_bits_2_value(&msg_ptr, 1);
        
        // UE Identity Type
        con_req->ue_id_type = (LIBLTE_RRC_CON_REQ_UE_ID_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

        // UE Identity
        if(LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI == con_req->ue_id_type) {
            liblte_rrc_unpack_s_tmsi_ie(&msg_ptr, (LIBLTE_RRC_S_TMSI_STRUCT *)&con_req->ue_id);
        } else {
            con_req->ue_id.random = (uint64)liblte_bits_2_value(&msg_ptr, 8) << 32;
            con_req->ue_id.random |= liblte_bits_2_value(&msg_ptr, 32);
        }

        // Establishment Cause
        con_req->cause = (LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM)liblte_bits_2_value(&msg_ptr, 3);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);    
}
