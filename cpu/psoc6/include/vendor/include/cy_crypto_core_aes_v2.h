/***************************************************************************//**
* \file cy_crypto_core_aes_v2.h
* \version 2.150
*
* \brief
*  This file provides constant and parameters for the API for the AES method
*  in the Crypto driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2024), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#if !defined (CY_CRYPTO_CORE_AES_V2_H)
#define CY_CRYPTO_CORE_AES_V2_H

#include "cy_crypto_common.h"

#if defined(CY_IP_MXCRYPTO) && defined(CY_CRYPTO_CFG_HW_V2_ENABLE)

#if defined(__cplusplus)
extern "C" {
#endif

#if (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_AES_C)


#if defined(CY_CRYPTO_CFG_CCM_C)
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Init(CRYPTO_Type *base,
                                            cy_stc_crypto_aes_ccm_buffers_t * aesCcmBuffer, cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_SetKey(CRYPTO_Type *base,
                                            uint8_t const *key, cy_en_crypto_aes_key_length_t keyLength,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Set_Length(CRYPTO_Type *base,
                                            uint32_t aadSize,  uint32_t textSize, uint32_t tagLength,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Start(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                             uint32_t ivSize, uint8_t const * iv,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Update_Aad(CRYPTO_Type *base,
                                            uint32_t aadSize,
                                            uint8_t const *aad,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Finish(CRYPTO_Type *base, uint8_t *tag, cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Encrypt_Tag(CRYPTO_Type *base,
                                            uint32_t ivSize, uint8_t const * iv,
                                            uint32_t aadSize, uint8_t const *aad,
                                            uint32_t srcSize, uint8_t *cipherTxt, uint8_t const *plainTxt,
                                            uint32_t tagSize, uint8_t *tag,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Decrypt(CRYPTO_Type *base,
                                            uint32_t ivSize, uint8_t const * iv,
                                            uint32_t aadSize, uint8_t const *aad,
                                            uint32_t srcSize, uint8_t *plainTxt, uint8_t const *cipherTxt,
                                            uint32_t tagSize, uint8_t const *tag, cy_en_crypto_aesccm_tag_verify_result_t *isValid,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ccm_Free(CRYPTO_Type *base, cy_stc_crypto_aes_ccm_state_t *aesCcmState);


#endif /* CY_CRYPTO_CFG_CCM_C*/

#if (CPUSS_CRYPTO_GCM == 1) && defined(CY_CRYPTO_CFG_GCM_C)

/** AES GCM TAG verification status */
typedef enum
{
    CY_CRYPTO_TAG_VALID     = 0x05555555u,
    CY_CRYPTO_TAG_INVALID   = 0x0AAAAAAAu,
} cy_en_crypto_aesgcm_tag_verify_result_t;

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Init(CRYPTO_Type *base,
                                                    cy_stc_crypto_aes_gcm_buffers_t *aesGCMBuffers,
                                                    cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_SetKey(CRYPTO_Type *base, uint8_t const *aesKey,
                                                      cy_en_crypto_aes_key_length_t keyLength,
                                                      cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Start(CRYPTO_Type *base, cy_en_crypto_dir_mode_t mode,
                                                     uint8_t const *iv, uint32_t ivSize,
                                                     cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_AAD_Update(CRYPTO_Type *base, uint8_t *aad, uint32_t aadSize,
                                                          cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Update(CRYPTO_Type *base, const uint8_t *input,
                                                       uint32_t inputSize,  uint8_t *output,
                                                       cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Finish(CRYPTO_Type *base,  uint8_t *p_tag,   uint32_t tagSize, cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Free(CRYPTO_Type *base,  cy_stc_crypto_aes_gcm_state_t* aesGCMctx);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Encrypt_Tag(CRYPTO_Type *base, uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength,
                                                            uint8_t const *iv, uint32_t ivSize, uint8_t *aad,   uint32_t aadSize,
                                                            const uint8_t *input,   uint32_t inputSize,  uint8_t *output, uint8_t *tag, uint32_t tagSize);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_GCM_Decrypt_Tag(CRYPTO_Type *base, uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength,
                                                            uint8_t const *iv, uint32_t ivSize, uint8_t *aad,   uint32_t aadSize,
                                                            const uint8_t *input,   uint32_t inputSize, uint8_t *tag, uint32_t tagSize, uint8_t *output, cy_en_crypto_aesgcm_tag_verify_result_t * isVerified);

#endif /* (CPUSS_CRYPTO_GCM == 1) && defined(CY_CRYPTO_CFG_GCM_C)*/

void Cy_Crypto_Core_V2_Aes_LoadEncKey(CRYPTO_Type *base,
                                      cy_stc_crypto_aes_state_t const *aesState);
void Cy_Crypto_Core_V2_Aes_LoadDecKey(CRYPTO_Type *base,
                                      cy_stc_crypto_aes_state_t const *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Free(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Init(CRYPTO_Type *base,
                                                 uint8_t const *key,
                                                 cy_en_crypto_aes_key_length_t keyLength,
                                                 cy_stc_crypto_aes_state_t *aesState,
                                                 cy_stc_crypto_aes_buffers_t *aesBuffers);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ecb(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ecb_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ecb_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ecb_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState);

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC)
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cbc(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);


cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cbc_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState);
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cbc_Set_IV(CRYPTO_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_crypto_aes_state_t *aesState);
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cbc_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState);
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cbc_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState);

#if defined(CY_CRYPTO_CFG_CBC_MAC_C)
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_CbcMac_Setup(CRYPTO_Type *base,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_CbcMac_Update(CRYPTO_Type *base,
                                                        uint32_t srcSize,
                                                        uint8_t const *src,
                                                        cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_CbcMac_Finish(CRYPTO_Type *base, uint8_t *mac,
                                                         cy_stc_crypto_aes_state_t *aesState);
#endif /* defined(CY_CRYPTO_CFG_CBC_MAC_C) */


#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB)
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cfb(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);


cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cfb_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cfb_Set_IV(CRYPTO_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cfb_Update(CRYPTO_Type *base,
                                             uint32_t srcSize,
                                             uint8_t *dst,
                                             uint8_t const *src,
                                             cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Cfb_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState);

#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR)
cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ctr(CRYPTO_Type *base,
                                                uint32_t srcSize,
                                                uint32_t *srcOffset,
                                                uint8_t *ivPtr,
                                                uint8_t *streamBlock,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);


cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ctr_Setup(CRYPTO_Type *base,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ctr_Set_IV(CRYPTO_Type *base,
                                            const uint8_t *iv,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ctr_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Aes_Ctr_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState);

#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR) */

#endif /* (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_AES_C) */

#if defined(__cplusplus)
}
#endif

#endif /* defined(CY_IP_MXCRYPTO) && defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */

#endif /* #if !defined (CY_CRYPTO_CORE_AES_V2_H) */


/* [] END OF FILE */
