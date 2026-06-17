/***************************************************************************//**
* \file cy_crypto_core_cmac.h
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


#if !defined (CY_CRYPTO_CORE_CMAC_H)
#define CY_CRYPTO_CORE_CMAC_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTO)

#include "cy_crypto_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_CMAC_C)

#include "cy_crypto_core_cmac_v1.h"
#include "cy_crypto_core_cmac_v2.h"

/** \cond INTERNAL */

typedef cy_en_crypto_status_t (*cy_crypto_cmac_func_t)(CRYPTO_Type *base,
                                          uint8_t  const *src,
                                          uint32_t srcSize,
                                          uint8_t  const *key,
                                          cy_en_crypto_aes_key_length_t keyLength,
                                          uint8_t *dst,
                                          cy_stc_crypto_aes_state_t *aesState);
/** \endcond */

/**
* \addtogroup group_crypto_lld_mac_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac
****************************************************************************//**
*
* Calculates the AES Cipher-based Message Authentication Code (CMAC) on the input
* message with the provided key.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param message
* The pointer to the source plain text. Must be 4-byte aligned.
*
* \param messageSize
* The size of the source plain text in bytes.
*
* \param key
* The pointer to the encryption key. Must be 4-byte aligned.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param cmac
* The pointer to the calculated CMAC.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_myCryptoCoreCmacUse
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac(CRYPTO_Type *base,
                                          uint8_t  const *message,
                                          uint32_t messageSize,
                                          uint8_t  const *key,
                                          cy_en_crypto_aes_key_length_t keyLength,
                                          uint8_t *cmac,
                                          cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Cmac(base, message, messageSize, key, keyLength, cmac, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac(base, message, messageSize, key, keyLength, cmac, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac_Init
****************************************************************************//**
*
* The function for initialization of CMAC operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \param buffer
* The pointer to the cmac buffer.
*
* \return
* \ref cy_en_crypto_status_t
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac_Init(CRYPTO_Type *base, void* cmacState, void  *buffer)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)cmacState;
        (void)buffer;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac_Init(base, (cy_stc_crypto_v2_cmac_state_t *)cmacState, (cy_stc_crypto_v2_cmac_buffers_t *)buffer);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac_Start
****************************************************************************//**
*
* Starts CMAC calculation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \param aesKey
* The pointer to the cmac key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \return
* \ref cy_en_crypto_status_t
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac_Start(CRYPTO_Type *base, void *cmacState,
                                                                uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)cmacState;
        (void)aesKey;
        (void)keyLength;

        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac_Start(base, (cy_stc_crypto_v2_cmac_state_t *)cmacState, aesKey, keyLength);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac_Update
****************************************************************************//**
*
* Performs cmac update for multi stage operation.
*
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameter message must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \param message
* The pointer to the message whose CMAC is being computed.
*
* \param messageSize
* The size of the message whose CMAC is being computed.
*
* \return
* \ref cy_en_crypto_status_t
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac_Update(CRYPTO_Type *base,
                                                                void *cmacState,
                                                                uint8_t const *message,
                                                                uint32_t  messageSize)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)cmacState;
        (void)message;
        (void)messageSize;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac_Update(base, (cy_stc_crypto_v2_cmac_state_t *)cmacState, message, messageSize);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac_Finish
****************************************************************************//**
*
* Completes CMAC calculation.
*
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameter cmac must align and end in 32 byte boundary.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param cmac
* The pointer to the computed CMAC value.
*
* \return
* \ref cy_en_crypto_status_t
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac_Finish(CRYPTO_Type *base, void *cmacState, uint8_t* cmac)


{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)cmacState;
        (void)cmac;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac_Finish(base, (cy_stc_crypto_v2_cmac_state_t *)cmacState, cmac);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Cmac_Free
****************************************************************************//**
*
*
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameter cmac must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \return
* \ref cy_en_crypto_status_t
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Cmac_Free(CRYPTO_Type *base,
                                void *cmacState
                                )

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)cmacState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Cmac_Free(base, (cy_stc_crypto_v2_cmac_state_t *)cmacState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/** \} group_crypto_lld_mac_functions */

#endif /* (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_CMAC_C) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTO */

#endif /* #if !defined (CY_CRYPTO_CORE_CMAC_H) */


/* [] END OF FILE */
