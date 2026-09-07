/***************************************************************************//**
* \file cy_crypto_core_aes.h
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


#if !defined (CY_CRYPTO_CORE_AES_H)
#define CY_CRYPTO_CORE_AES_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTO)

#include "cy_crypto_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_AES_C)

#include "cy_crypto_core_aes_v1.h"
#include "cy_crypto_core_aes_v2.h"

#include "cy_crypto_core_hw.h"

typedef cy_en_crypto_status_t (*cy_crypto_aes_init_func_t)(CRYPTO_Type *base,
                                                 uint8_t const *key,
                                                 cy_en_crypto_aes_key_length_t keyLength,
                                                 cy_stc_crypto_aes_state_t *aesState,
                                                 cy_stc_crypto_aes_buffers_t *aesBuffers);

typedef cy_en_crypto_status_t (*cy_crypto_aes_ecb_func_t)(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);
#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC)
typedef cy_en_crypto_status_t (*cy_crypto_aes_cbc_func_t)(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);
#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB)
typedef cy_en_crypto_status_t (*cy_crypto_aes_cfb_func_t)(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);
#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR)
typedef cy_en_crypto_status_t (*cy_crypto_aes_ctr_func_t)(CRYPTO_Type *base,
                                                uint32_t srcSize,
                                                uint32_t *srcOffset,
                                                uint8_t *ivPtr,
                                                uint8_t *streamBlock,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState);
#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR) */

/**
* \addtogroup group_crypto_lld_symmetric_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Init
****************************************************************************//**
*
* Initializes AES mode of operation and prepares an inverse key.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters key must align and end in 32 byte boundary.
*
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param key
* The pointer to the encryption/decryption key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_myCryptoCoreAesEcbUse
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Init(CRYPTO_Type *base,
                                                 uint8_t const *key,
                                                 cy_en_crypto_aes_key_length_t keyLength,
                                                 cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE) || defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
    cy_stc_crypto_aes_buffers_t *aesBuffers = (cy_stc_crypto_aes_buffers_t *)((void *)Cy_Crypto_Core_GetVuMemoryAddress(base));

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Init(base, key, keyLength, aesState, aesBuffers);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Init(base, key, keyLength, aesState, aesBuffers);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }
    #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) || defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */

    return tmpResult;
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_InitContext
****************************************************************************//**
*
* Initializes AES mode of operation and prepares an inverse key within a user
* specified buffer.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param key
* The pointer to the encryption/decryption key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \param aesBuffers
* The pointer to the AES buffer provided by the user. The user must not modify
* anything in this buffer during operation.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_InitContext(CRYPTO_Type *base,
                                                 uint8_t const *key,
                                                 cy_en_crypto_aes_key_length_t keyLength,
                                                 cy_stc_crypto_aes_state_t *aesState,
                                                 cy_stc_crypto_aes_buffers_t *aesBuffers)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Init(base, key, keyLength, aesState, aesBuffers);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Init(base, key, keyLength, aesState, aesBuffers);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Free
****************************************************************************//**
*
* Clears AES operation context.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_myCryptoCoreAesEcbUse
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Free(CRYPTO_Type *base,
                                                 cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Free(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Free(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ecb
****************************************************************************//**
*
* Performs the AES operation on a single block.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src & dst must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t).
*
* \param dst
* The pointer to the destination cipher block.
*
* \param src
* The pointer to the source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_myCryptoCoreAesEcbUse
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ecb(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Ecb(base, dirMode, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ecb(base, dirMode, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ecb_Setup
****************************************************************************//**
*
* Performs an AES ECB init operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t).
*
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ecb_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState)


{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)dirMode;
        (void)aesState;

        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ecb_Setup(base, dirMode, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ecb_Update
****************************************************************************//**
*
* Performs an AES ECB Multistage update operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src & dst must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ecb_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)dst;
        (void)src;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ecb_Update(base, srcSize, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ecb_Finish
****************************************************************************//**
*
* Performs an AES ECB finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ecb_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ecb_Finish(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC)
/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cbc
****************************************************************************//**
*
* Performs the AES-CBC operation defined in the dirMode parameter on a plain text
* defined in the src parameter.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t)
*
* \param srcSize
* The size of the source plain text.
*
* \param ivPtr
* The pointer to the initial vector.
*
* \param dst
* The pointer to the destination cipher text.
*
* \param src
* The pointer to the source plain text.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cbc(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Cbc(base, dirMode, srcSize, ivPtr, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cbc(base, dirMode, srcSize, ivPtr, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cbc_Setup
****************************************************************************//**
*
* Performs an AES CBC setup operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t).
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cbc_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)dirMode;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cbc_Setup(base, dirMode, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cbc_Set_IV
****************************************************************************//**
*
* Function to set AES CBC IV.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters iv must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param iv
* The pointer to the IV.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cbc_Set_IV(CRYPTO_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)iv;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cbc_Set_IV(base, iv, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cbc_Update
****************************************************************************//**
*
* Performs an AES CBC Multistage update operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src & dst must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cbc_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)dst;
        (void)src;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cbc_Update(base, srcSize, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cbc_Finish
****************************************************************************//**
*
* Performs an AES CBC finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cbc_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cbc_Finish(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_CbcMac_Setup
****************************************************************************//**
*
* Performs an AES CBC MAC setup operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_CbcMac_Setup(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_CbcMac_Setup(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_CbcMac_Update
****************************************************************************//**
*
* Performs an AES CBC MAC Multistage update operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_CbcMac_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)src;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_CbcMac_Update(base, srcSize, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_CbcMac_Finish
****************************************************************************//**
*
* Performs an AES CBC finish operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters mac must align and end in 32 byte boundary.
*
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param mac
* The pointer to the cbc-mac.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_CbcMac_Finish(CRYPTO_Type *base, uint8_t *mac, cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)mac;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_CbcMac_Finish(base, mac, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}

#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CBC) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB)
/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cfb
****************************************************************************//**
*
* Performs the AES-CFB operation defined in the dirMode parameter on a plain text
* defined in the SRC parameter.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t)
*
* \param srcSize
* The size of the source plain text.
*
* \param ivPtr
* The pointer to the initial vector.
*
* \param dst
* The pointer to the destination cipher text.
*
* \param src
* The pointer to the source plain text.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cfb(CRYPTO_Type *base,
                                                cy_en_crypto_dir_mode_t dirMode,
                                                uint32_t srcSize,
                                                uint8_t *ivPtr,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Cfb(base, dirMode, srcSize, ivPtr, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cfb(base, dirMode, srcSize, ivPtr, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cfb_Setup
****************************************************************************//**
*
* Performs an AES CFB setup operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t).
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cfb_Setup(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,
                                            cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)dirMode;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cfb_Setup(base, dirMode, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cfb_Set_IV
****************************************************************************//**
*
* Sets IV for AES CFB mode.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters iv must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param iv
* The pointer to iv.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cfb_Set_IV(CRYPTO_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)iv;
        (void)aesState;        
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cfb_Set_IV(base, iv, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cfb_Update
****************************************************************************//**
*
* Performs an AES CFB Multistage update operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src & dst must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cfb_Update(CRYPTO_Type *base,
                                             uint32_t srcSize,
                                             uint8_t *dst,
                                             uint8_t const *src,
                                             cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)dst;
        (void)src;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cfb_Update(base, srcSize, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Cfb_Finish
****************************************************************************//**
*
* Performs an AES CFB finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Cfb_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Cfb_Finish(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}

#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CFB) */

#if defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR)
/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ctr
****************************************************************************//**
*
* Performs the AES-CTR operation on a plain text defined in the src parameter.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters iv must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source plain text.
*
* \param srcOffset
* The size of an offset within the current block stream for resuming within the
* current cipher stream.
*
* \param ivPtr
* The 128-bit initial vector that contains a 64-bit nonce and 64-bit counter.
*
* \param streamBlock
* The saved stream-block for resuming. Is over-written by the function.
*
* \param dst
* The pointer to the destination cipher text.
*
* \param src
* The pointer to the source plain text. Must be 4-Byte aligned.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ctr(CRYPTO_Type *base,
                                                uint32_t srcSize,
                                                uint32_t *srcOffset,
                                                uint8_t *ivPtr,
                                                uint8_t *streamBlock,
                                                uint8_t *dst,
                                                uint8_t const *src,
                                                cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = Cy_Crypto_Core_V1_Aes_Ctr(base, srcSize, srcOffset, ivPtr, streamBlock, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ctr(base, srcSize, srcOffset, ivPtr, streamBlock, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ctr_Setup
****************************************************************************//**
*
* Performs an AES CTR Multistage update operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ctr_Setup(CRYPTO_Type *base,
                                            cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ctr_Setup(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}  

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ctr_Set_IV
****************************************************************************//**
*
* Sets IV for the AES CTR operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param iv
* The pointer to iv.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ctr_Set_IV(CRYPTO_Type *base,
                                            const uint8_t *iv,
                                            cy_stc_crypto_aes_state_t *aesState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)iv;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ctr_Set_IV(base, iv, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}  



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ctr_Update
****************************************************************************//**
*
* Performs an AES CTR Multistage update operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters src & dst must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ctr_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)dst;
        (void)src;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ctr_Update(base, srcSize, dst, src, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}  




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ctr_Finish
****************************************************************************//**
*
* Performs an AES CTR Finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ctr_Finish(CRYPTO_Type *base, cy_stc_crypto_aes_state_t *aesState)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ctr_Finish(base, aesState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}  

#endif /* defined(CY_CRYPTO_CFG_CIPHER_MODE_CTR) */

#if (CPUSS_CRYPTO_GCM == 1) && defined(CY_CRYPTO_CFG_GCM_C)

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Init
****************************************************************************//**
*
* The function to initialize AES GCM operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesGCMBuffers
* The pointer to the AES GCM buffer provided by the user. The user must not modify anything in this structure.
*
* \param aesGCMctx
* The pointer to the AES GCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Init(CRYPTO_Type *base,cy_stc_crypto_aes_gcm_buffers_t *aesGCMBuffers,
                                                      cy_stc_crypto_aes_gcm_state_t* aesGCMctx)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Init(base, aesGCMBuffers, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_SetKey
****************************************************************************//**
*
* The function to set AES GCM Key.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters aeskey must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesKey
* The pointer to the AES key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param aesGCMctx
* The pointer to the AES GCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_SetKey(CRYPTO_Type *base, uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength,
                                                      cy_stc_crypto_aes_gcm_state_t* aesGCMctx)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_SetKey(base, aesKey, keyLength, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Start
****************************************************************************//**
*
* The function to start AES GCM operation.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters iv must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param mode
* \ref cy_en_crypto_dir_mode_t 
*
* \param iv
* The pointer to the Initialization vector.
*
* \param ivSize
* The length of the iv.
*
* \param aesGCMctx
* The pointer to the AES GCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Start(CRYPTO_Type *base, cy_en_crypto_dir_mode_t mode,
                                                      uint8_t const *iv, uint32_t ivSize,
                                                      cy_stc_crypto_aes_gcm_state_t* aesGCMctx)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Start(base, mode, iv, ivSize, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_AAD_Update
****************************************************************************//**
*
* The function to update the Additional Authentication Data.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters aad must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aad
* The pointer to the Additional Authentication Data.
*
* \param aadSize
*  The length of the Additional Authentication Data
*
* \param aesGCMctx
* The pointer to the AES GCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_AAD_Update(CRYPTO_Type *base, uint8_t *aad,
                                                                        uint32_t aadSize,
                                                                        cy_stc_crypto_aes_gcm_state_t* aesGCMctx)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_AAD_Update(base, aad, aadSize, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Update
****************************************************************************//**
*
* The function to update the data
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters input & output must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param input
* The pointer to the input data to be encrypted/decrypted.
*
* \param inputSize
*  The length of the input data.
*
* \param output
* The pointer to the encrypted/decrypted output data.
*
* \param aesGCMctx
* The pointer to the AES GCm state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Update(CRYPTO_Type *base,  const uint8_t *input,
                                                       uint32_t inputSize,  uint8_t *output,
                                                       cy_stc_crypto_aes_gcm_state_t* aesGCMctx)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Update(base, input, inputSize, output, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Finish
****************************************************************************//**
*
* The function to finish the AES GCM operation and to calculate the tag.
* For CAT1C & CAT1D(CM55) devices when D-Cache is enabled parameters p_tag must align and end in 32 byte boundary.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param p_tag
* The pointer to the buffer for storing tag.
*
* \param tagSize
*  The length of the p_tag.
*
* \param aesGCMctx
* The pointer to the AES aesGCMctx structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Finish(CRYPTO_Type *base,  uint8_t *p_tag, 
                                                              uint32_t tagSize, cy_stc_crypto_aes_gcm_state_t* aesGCMctx)


{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Finish(base, p_tag, tagSize, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Free
****************************************************************************//**
*
* The function to finish the encryption process and calculate tag.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesGCMctx
* The pointer to the AES aesGCMctx structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_init_update_finish_free
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Free(CRYPTO_Type *base,  cy_stc_crypto_aes_gcm_state_t* aesGCMctx)


{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Free(base, aesGCMctx);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Encrypt_Tag
****************************************************************************//**
*
* Performs the AES GCM encryption operation on the input data, iv & aad data, generates the encrypted data and TAG.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesKey
* The pointer to the AES key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param iv
* The pointer to the Initialization vector.
*
* \param ivSize
* The length of the iv.
*
* \param aad
* The pointer to the Additional Authentication Data.
*
* \param aadSize
*  The length of the additional Authentication Data
*
* \param input
* The pointer to the input data to be encrypted/decrypted.
*
* \param inputSize
*  The length of the input data.
*
* \param output
* The pointer to the encrypted/decrypted output data.
*
* \param tag
* The pointer to the tag.
*
* \param tagSize
*  The length of the p_tag.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_Encrypt_Tag
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Encrypt_Tag(CRYPTO_Type *base, uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength,
                                                            uint8_t const *iv, uint32_t ivSize, uint8_t *aad,   uint32_t aadSize, 
                                                            const uint8_t *input,   uint32_t inputSize,  uint8_t *output, uint8_t *tag, uint32_t tagSize)

{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Encrypt_Tag(base, aesKey, keyLength,
                                                          iv, ivSize, aad, aadSize, 
                                                          input, inputSize, output, tag, tagSize);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_GCM_Decrypt_Tag
****************************************************************************//**
*
* Performs the AES GCM decryption operation on the input data and verifies the TAG.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesKey
* The pointer to the AES key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param iv
* The pointer to the Initialization vector.
*
* \param ivSize
* The length of the iv.
*
* \param aad
* The pointer to the Additional Authentication Data.
*
* \param aadSize
*  The length of the additional Authentication Data
*
* \param input
* The pointer to the input data to be encrypted/decrypted.
*
* \param inputSize
*  The length of the input data.
*
* \param tag
* The pointer to the tag.
*
* \param tagSize
*  The length of the p_tag.
*
* \param output
* The pointer to the encrypted/decrypted output data.
*
* \param isVerified
* The status of the AES GCM verification.
*
* \return
* \ref cy_en_crypto_status_t
*
* \funcusage
* \snippet crypto/snippet/main.c snippet_Cy_Crypto_Core_Aes_GCM_Decrypt_Tag
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_GCM_Decrypt_Tag(CRYPTO_Type *base, uint8_t const *aesKey, cy_en_crypto_aes_key_length_t keyLength,
                                                            uint8_t const *iv, uint32_t ivSize, uint8_t *aad,   uint32_t aadSize, 
                                                            const uint8_t *input,   uint32_t inputSize, uint8_t *tag, uint32_t tagSize, uint8_t *output,
                                                            cy_en_crypto_aesgcm_tag_verify_result_t * isVerified)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_GCM_Decrypt_Tag(base, aesKey, keyLength,
                                                            iv, ivSize, aad, aadSize, 
                                                            input, inputSize, tag, tagSize, output, isVerified);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}
#endif /* (CPUSS_CRYPTO_GCM == 1) && defined(CY_CRYPTO_CFG_GCM_C)*/


#if defined(CY_CRYPTO_CFG_CCM_C)

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Init
****************************************************************************//**
*
* Performs an AES CCM Init operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesCcmBuffer The buffers should be a SAHB mapped addresses.
* The pointer to the memory buffers storage.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Init(CRYPTO_Type *base,
                                            cy_stc_crypto_aes_ccm_buffers_t * aesCcmBuffer, cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesCcmBuffer;
        (void)aesCcmState;
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Init(base, aesCcmBuffer, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}   


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_SetKey
****************************************************************************//**
*
* Sets AES CCM Key for the operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param key
* The pointer to the CCM key.
*
* \param keyLength
* \ref cy_en_crypto_aes_key_length_t
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_SetKey(CRYPTO_Type *base,
                                            uint8_t const *key, cy_en_crypto_aes_key_length_t keyLength,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)key;
        (void)keyLength;
        (void)aesCcmState;        
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_SetKey(base, key, keyLength, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}                                            



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Set_Length
****************************************************************************//**
*
* Sets the length for Additional authentication data, plain text and Tag for AES CCM operation.
*
* \param base
* The pointer to the CRYPTO instance.
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Set_Length(CRYPTO_Type *base,
                                            uint32_t aadSize,  uint32_t textSize, uint32_t tagLength,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aadSize;
        (void)textSize;
        (void)tagLength;
        (void)aesCcmState;          
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Set_Length(base, aadSize, textSize, tagLength, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}   



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Start
****************************************************************************//**
*
* Function to set IV for the AES CCM operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTO_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_crypto_dir_mode_t)
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/

__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Start(CRYPTO_Type *base,
                                            cy_en_crypto_dir_mode_t dirMode,    
                                             uint32_t ivSize, uint8_t const * iv,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{

    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)dirMode;
        (void)ivSize;
        (void)iv;
        (void)aesCcmState;          
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Start(base, dirMode, ivSize, iv, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}   


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Update_Aad
****************************************************************************//**
*
* Performs an AES CCM update AAD Multistage  operation.
*
* \param base
* The pointer to the CRYPTO instance.
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Update_Aad(CRYPTO_Type *base,
                                            uint32_t aadSize,
                                            uint8_t const *aad,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aadSize;
        (void)aad;
        (void)aesCcmState;           
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Update_Aad(base, aadSize, aad, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Update
***************************************************************************//**
*
* Performs an AES CCM Update Multistage update operation.
*
* \param base
* The pointer to the CRYPTO instance.
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Update(CRYPTO_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)srcSize;
        (void)dst;
        (void)src;
        (void)aesCcmState;              
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Update(base, srcSize, dst, src, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}   

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Finish
****************************************************************************//**
*
* Performs an AES CCM finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param tag
* The pointer to the CCM Tag.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Finish(CRYPTO_Type *base, uint8_t *tag, cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)tag;
        (void)aesCcmState;          
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Finish(base, tag, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}




/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Encrypt_Tag
****************************************************************************//**
*
* Performs an AES CCM Encrypt operation.
* \note Cy_Crypto_Core_Aes_Ccm_Init() and Cy_Crypto_Core_Aes_Ccm_SetKey() should be called before calling this function
*
* \param base
* The pointer to the CRYPTO instance.
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Encrypt_Tag(CRYPTO_Type *base,
                                            uint32_t ivSize, uint8_t const * iv,
                                            uint32_t aadSize, uint8_t const *aad,
                                            uint32_t srcSize, uint8_t *cipherTxt, uint8_t const *plainTxt,
                                            uint32_t tagSize, uint8_t *tag,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{

    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)ivSize;
        (void)iv;
        (void)aadSize;
        (void)aad;
        (void)srcSize;
        (void)cipherTxt;
        (void)plainTxt;
        (void)tagSize;
        (void)tag;
        (void)aesCcmState;          
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Encrypt_Tag(base, ivSize, iv, aadSize, aad, srcSize, cipherTxt, plainTxt, tagSize, tag, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}   


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Decrypt
****************************************************************************//**
*
* Performs an AES CCM Decrypt operation.
* \note Cy_Crypto_Core_Aes_Ccm_Init() and Cy_Crypto_Core_Aes_Ccm_SetKey() should be called before calling this function
*
* \param base
* The pointer to the CRYPTO instance.
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
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Decrypt(CRYPTO_Type *base,
                                            uint32_t ivSize, uint8_t const * iv,
                                            uint32_t aadSize, uint8_t const *aad,
                                            uint32_t srcSize, uint8_t *plainTxt, uint8_t const *cipherTxt,
                                            uint32_t tagSize, uint8_t const *tag, cy_en_crypto_aesccm_tag_verify_result_t *isValid,
                                            cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)ivSize;
        (void)iv;
        (void)aadSize;
        (void)aad;
        (void)srcSize;
        (void)cipherTxt;
        (void)plainTxt;
        (void)tagSize;
        (void)tag;
        (void)isValid;
        (void)aesCcmState;          
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Decrypt(base, ivSize, iv, aadSize, aad, srcSize, plainTxt, cipherTxt, tagSize, tag, isValid, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}   


/*******************************************************************************
* Function Name: Cy_Crypto_Core_Aes_Ccm_Free
****************************************************************************//**
*
* Clears AES CCM operation context.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesCcmState
* The pointer to the AES CCM state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Aes_Ccm_Free(CRYPTO_Type *base, cy_stc_crypto_aes_ccm_state_t *aesCcmState)
{

    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        (void)base;
        (void)aesCcmState;         
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Aes_Ccm_Free(base, aesCcmState);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}

#endif /*CY_CRYPTO_CFG_CCM_C*/

/** \} group_crypto_lld_symmetric_functions */

#endif /* (CPUSS_CRYPTO_AES == 1) && defined(CY_CRYPTO_CFG_AES_C) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTO */

#endif /* #if !defined (CY_CRYPTO_CORE_AES_H) */


/* [] END OF FILE */
