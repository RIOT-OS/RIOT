/***************************************************************************//**
* \file cy_cryptolite_hmac.h
* \version 2.80
* \brief
*  This file provides common constants and parameters
*  for the Cryptolite hmac sha256 driver.
*
********************************************************************************
* Copyright 2022 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_HMAC_H)
#define CY_CRYPTOLITE_HMAC_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#include "cy_cryptolite_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CRYPTOLITE_SHA_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_HMAC_C) && defined(CY_CRYPTOLITE_CFG_SHA2_256_ENABLED)

#include "cy_cryptolite_sha256.h"

/** \cond INTERNAL */
/** \endcond */

/**
* \addtogroup group_cryptolite_data_structures
* \{
*/

/** \cond INTERNAL */
#define CY_CRYPTOLITE_HMAC_IPAD               (0x36u)
#define CY_CRYPTOLITE_HMAC_0PAD               (0x5Cu)
#define CY_CRYPTOLITE_HMAC_MAX_PAD_SIZE       (CY_CRYPTOLITE_SHA256_BLOCK_SIZE)
#define CY_CRYPTOLITE_HMAC_MAX_M0_KEY_SIZE    (CY_CRYPTOLITE_SHA256_BLOCK_SIZE)
/** \endcond */

/** The structure for storing the SHA256 context.
* All fields for the context structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in the function calls. Firmware must
* ensure that the defined instance of this structure remains in scope
* while the drive is in use.
*/
/*Check : ALIGN to be enforced with separate buf type*/
typedef struct
{
    /** \cond INTERNAL */
    CY_ALIGN(4) uint8_t ipad[CY_CRYPTOLITE_HMAC_MAX_PAD_SIZE];
    CY_ALIGN(4) uint8_t opad[CY_CRYPTOLITE_HMAC_MAX_PAD_SIZE];
    CY_ALIGN(4) uint8_t m0Key[CY_CRYPTOLITE_SHA256_BLOCK_SIZE];
    uint16_t blocksize;
    uint16_t digestsize;
    /** sha256 context */
    cy_stc_cryptolite_context_sha256_t ctx_sha256;    /** \endcond */
} cy_stc_cryptolite_context_hmac_sha256_t;


/** \} group_cryptolite_data_structures */

/**
* \addtogroup group_cryptolite_lld_mac_functions
* \{
*/
/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Init
****************************************************************************//**
*
* The function to initialize the HMAC operation.
*
* \param base
* The pointer to the Cryptolite instance.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_context_hmac_sha256_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_init_start_update_finish_free
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Init(CRYPTOLITE_Type *base,
                                        cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Start
****************************************************************************//**
*
* Initializes the initial Hash vector.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param key
* The pointer to the HMAC key.
*
* \param keyLength
* The length of the HMAC key.
*
*  key
* The pointer to the HMAC key.
*
*  keyLength
* The length of the HMAC key.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_context_hmac_sha256_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_init_start_update_finish_free
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Start(CRYPTOLITE_Type *base, uint8_t const *key, uint32_t keyLength,
                                        cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Update
****************************************************************************//**
*
* Performs the HMAC update on message.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param message
* The address pointer to the message whose Hash is being computed.
*
* \param messageSize
* The size of the message whose Hash is being computed.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_context_hmac_sha256_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_init_start_update_finish_free
*
* \note There is no alignment or size restriction for message buffer, However providing
* a four byte aligned buffer with size in multiple of \ref CY_CRYPTOLITE_SHA256_BLOCK_SIZE,
* will result in best execution time.
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Update(CRYPTOLITE_Type *base,
                                                    uint8_t const *message,
                                                    uint32_t  messageSize,
                                                    cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Finish
****************************************************************************//**
*
* Completes the HMAC SHA256 calculation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param hmac
* The address pointer to the calculated Hmac.
*
* \param cfContext
* The pointer to the  cy_stc_cryptolite_context_hmac_sha_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_init_start_update_finish_free
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Finish(CRYPTOLITE_Type *base,
                               uint8_t *hmac,
                               cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Free
****************************************************************************//**
*
* Clears the used memory and context data.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_context_hmac_sha256_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_init_start_update_finish_free
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Free(CRYPTOLITE_Type *base,
                                                    cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Hmac_Sha256_Run
****************************************************************************//**
*
* This function performs the HMAC SHA256 function.
* Provide the required parameters and the pointer
* to the context structure when making this function call.
* It is independent of the previous Crypto state because it already contains
* preparation, calculation, and finalization steps.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param key
* The pointer to the HMAC key.
*
* \param keyLength
* The length of the HMAC key.
*
* \param message
* The address pointer to a message whose hash value is being computed.
*
* \param messageSize
* The size of a message in bytes.
*
* \param hmac
* The address pointer to the hmac.
*
* \param cfContext
* The pointer to the \ref cy_stc_cryptolite_context_hmac_sha256_t structure that stores all
* internal variables for Cryptolite driver.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Hmac_Sha256_Run
*
* \note There is no alignment or size restriction for message buffer, However providing
* a four byte aligned buffer with size in multiple of \ref CY_CRYPTOLITE_SHA256_BLOCK_SIZE,
* will result in best execution time.
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Hmac_Sha256_Run(CRYPTOLITE_Type *base,
                                        uint8_t const *key,
                                        uint32_t keyLength,
                                        uint8_t const *message,
                                        uint32_t  messageSize,
                                        uint8_t *hmac,
                                        cy_stc_cryptolite_context_hmac_sha256_t *cfContext);

#endif /* #if defined(CY_CRYPTO_CFG_HMAC_C)*/
#endif /* #if CRYPTOLITE_SHA_PRESENT */

/** \} group_cryptolite_lld_mac_functions */
#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_HMAC_H) */
/** \} group_cryptolite */

/* [] END OF FILE */
