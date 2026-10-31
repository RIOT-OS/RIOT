/***************************************************************************//**
* \file cy_cryptolite_aes_ccm.h
* \version 2.80
*
* \brief
*  This file provides common constants and parameters
*  for the Cryptolite AES CCM driver.
*
********************************************************************************
* Copyright 2023 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_AES_CCM_H)
#define CY_CRYPTOLITE_AES_CCM_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"

#if defined(CY_CRYPTOLITE_CFG_AES_C)
#if (CRYPTOLITE_AES_PRESENT == 1) 
#if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CCM)

#include "cy_cryptolite_aes.h"
#include "cy_cryptolite_vu.h"

/**
* \addtogroup group_cryptolite_lld_aes_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Init
****************************************************************************//**
*
* Performs an AES CCM Init operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesCcm_buffer The buffers should be a SAHB mapped addresses.
* The pointer to the memory buffers storage.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Init(CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_aes_ccm_buffers_t * aesCcm_buffer, cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_SetKey
****************************************************************************//**
*
* Sets AES CCM Key for the operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param key
* The pointer to the CCM key.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_SetKey(CRYPTOLITE_Type *base,
                                            uint8_t const *key, cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Set_Length
****************************************************************************//**
*
* Sets the length for Additional authentication data, plain text and Tag for AES CCM operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aadSize
* The Size of the Additional Authentication Data.
*
* \param textSize
* The Size of the Text.
*
* \param tagLength
* The Size of the Tag.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Set_Length(CRYPTOLITE_Type *base,
                                            uint32_t aadSize,  uint32_t textSize, uint32_t tagLength,
                                            cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Start
****************************************************************************//**
*
* Function to set IV for the AES CCM operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param dirMode
* Can be \ref CY_CRYPTOLITE_ENCRYPT or \ref CY_CRYPTOLITE_DECRYPT
* (\ref cy_en_cryptolite_dir_mode_t)
*
* \param ivSize
* The size of the IV.
*
* \param iv
* The pointer to the IV.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Start(CRYPTOLITE_Type *base,
                                                        cy_en_cryptolite_dir_mode_t dirMode,    
                                                        uint32_t ivSize, uint8_t const * iv,
                                                        cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Update_Aad
****************************************************************************//**
*
* Performs an AES CCM update AAD Multistage  operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aadSize
* The size of the AAD.
*
* \param aad
* The pointer to a AAD.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Update_Aad(CRYPTOLITE_Type *base,
                                                            uint32_t aadSize,
                                                            uint8_t const *aad,
                                                            cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Update
****************************************************************************//**
*
* Performs an AES CCM Update Multistage update operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination block.
*
* \param src
* The pointer to a source block.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Update(CRYPTOLITE_Type *base, 
                                                        uint32_t srcSize,
                                                        uint8_t *dst,
                                                        uint8_t const *src,
                                                        cy_stc_cryptolite_aes_ccm_state_t *aesCcmState)  ;


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Finish
****************************************************************************//**
*
* Performs an AES CCM finish operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param tag
* The pointer to the CCM Tag.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Finish(CRYPTOLITE_Type *base, uint8_t *tag, cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Encrypt_Tag
****************************************************************************//**
*
* Performs an AES CCM Encrypt operation.
* \note Cy_Cryptolite_Aes_Ccm_Init() and Cy_Cryptolite_Aes_Ccm_SetKey() should be called before calling this function
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param ivSize
* The size of the IV.
*
* \param iv
* The pointer to the IV.
*
* \param aadSize
* The size of the AAD.
*
* \param aad
* The pointer to a AAD.
*
* \param srcSize
* The size of the source block.
*
* \param cipherTxt
* The pointer to a cipher text block.
*
* \param plainTxt
* The pointer to a plain text block.
*
* \param tagSize
* The size of the Tag.
*
* \param tag
* The pointer to the tags.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Encrypt_Tag(CRYPTOLITE_Type *base,
                                                            uint32_t ivSize, uint8_t const * iv,
                                                            uint32_t aadSize, uint8_t const *aad,
                                                            uint32_t srcSize, uint8_t *cipherTxt, uint8_t const *plainTxt,
                                                            uint32_t tagSize, uint8_t *tag,
                                                            cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Decrypt
****************************************************************************//**
*
* Performs an AES CCM Decrypt operation.
* \note Cy_Cryptolite_Aes_Ccm_Init() and Cy_Cryptolite_Aes_Ccm_SetKey() should be called before calling this function
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param ivSize
* The size of the IV.
*
* \param iv
* The pointer to the IV.
*
* \param aadSize
* The size of the AAD.
*
* \param aad
* The pointer to a AAD.
*
* \param srcSize
* The size of the source block.
*
* \param plainTxt
* The pointer to a plain text block.
*
* \param cipherTxt
* The pointer to a cipher text block.
*
* \param tagSize
* The size of the Tag.
*
* \param tag
* The pointer to the tags.
*
* \param isValid
* The pointer Store the authentication status.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Decrypt(CRYPTOLITE_Type *base,
                                                            uint32_t ivSize, uint8_t const * iv,
                                                            uint32_t aadSize, uint8_t const *aad,
                                                            uint32_t srcSize, uint8_t *plainTxt, uint8_t const *cipherTxt,
                                                            uint32_t tagSize, uint8_t *tag, cy_en_cryptolite_ccm_auth_result_t *isValid,
                                                            cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ccm_Free
****************************************************************************//**
*
* Clears AES CCM operation context.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ccm_Free(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_ccm_state_t *aesCcmState);



#endif /*CY_CRYPTOLITE_CFG_CIPHER_MODE_CCM*/
#endif /* #if (CY_CRYPTOLITE_CFG_AES_C)*/
#endif /* #if CRYPTOLITE_AES_PRESENT*/
/** \} group_cryptolite_lld_aes_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_AES_CCM_H) */

/* [] END OF FILE */
