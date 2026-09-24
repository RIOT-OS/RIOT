/***************************************************************************//**
* \file cy_crypto_core_cmac_v2.h
* \version 2.150
*
* \brief
*  This file provides constants and function prototypes
*  for the API for the CMAC method in the Crypto block driver.
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


#if !defined (CY_CRYPTO_CORE_CMAC_V2_H)
#define CY_CRYPTO_CORE_CMAC_V2_H

#include "cy_crypto_common.h"

#if defined(CY_IP_MXCRYPTO) && defined(CY_CRYPTO_CFG_HW_V2_ENABLE)

#if defined(__cplusplus)
extern "C" {
#endif

#if (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_CMAC_C)

/** \cond INTERNAL */

/* The structure to store the AES-CMAC context */
typedef struct
{
    cy_stc_crypto_aes_state_t aesState;
    uint8_t *k;
    uint8_t *temp;
} cy_stc_crypto_v2_cmac_state_t;

/* The structure to define used memory buffers */
typedef struct
{
    uint8_t k[CY_CRYPTO_ALIGN_CACHE_LINE(CY_CRYPTO_AES_BLOCK_SIZE)];
    uint8_t temp[CY_CRYPTO_ALIGN_CACHE_LINE(CY_CRYPTO_AES_BLOCK_SIZE)];
    cy_stc_crypto_aes_buffers_t aesBuffersData;
} cy_stc_crypto_v2_cmac_buffers_t;

/* The function prototypes */
cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac_Init(CRYPTO_Type *base, cy_stc_crypto_v2_cmac_state_t* cmacState, cy_stc_crypto_v2_cmac_buffers_t* buffer);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac_Start(CRYPTO_Type *base, cy_stc_crypto_v2_cmac_state_t *cmacState,
                                                    uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac_Update(CRYPTO_Type *base,
                                cy_stc_crypto_v2_cmac_state_t *cmacState,
                                uint8_t const *message,
                                uint32_t  messageSize);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac_Finish(CRYPTO_Type *base, cy_stc_crypto_v2_cmac_state_t *cmacState, uint8_t* cmac);

cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac_Free(CRYPTO_Type *base,
                                cy_stc_crypto_v2_cmac_state_t *cmacState
                                );

cy_en_crypto_status_t Cy_Crypto_Core_V2_Cmac(CRYPTO_Type *base,
                              uint8_t  const *message,
                              uint32_t messageSize,
                              uint8_t  const *key,
                              cy_en_crypto_aes_key_length_t keyLength,
                              uint8_t *cmac,
                              cy_stc_crypto_aes_state_t *aesState);

/** \endcond */


#endif /* (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_CMAC_C) */

#if defined(__cplusplus)
}
#endif

#endif /* defined(CY_IP_MXCRYPTO) && defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */

#endif /* #if !defined (CY_CRYPTO_CORE_CMAC_V2_H) */


/* [] END OF FILE */
