/***************************************************************************//**
* \file cy_crypto_core_hkdf.h
* \version 2.150
*
* \brief
*  This file provides constants and function prototypes
*  for the API for the HKDF method in the Crypto block driver.
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


#if !defined (CY_CRYPTO_CORE_HKDF_H)
#define CY_CRYPTO_CORE_HKDF_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTO)

#include "cy_crypto_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CPUSS_CRYPTO_SHA == 1) && defined(CY_CRYPTO_CFG_HKDF_C)

#include "cy_crypto_core_hkdf_v2.h"

/**
* \addtogroup group_crypto_lld_kdf_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Hkdf_Extract
****************************************************************************//**
*
* Performs HKDF Extract calculation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param mode
* \ref cy_en_crypto_sha_mode_t
*
* \param salt
* The pointer to salt.
*
* \param saltLength
* The size of a salt.
*
* \param ikm
* The pointer to the Input Key material.
*
* \param ikmLength
* The length of the IKM.
*
* \param prk
* The pointer to store the generated prk.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Hkdf_Extract(CRYPTO_Type *base, cy_en_crypto_sha_mode_t mode,
                                          uint8_t  const *salt,
                                          uint32_t saltLength,
                                          uint8_t  const *ikm,
                                          uint32_t ikmLength,
                                          uint8_t *prk)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        (void)base; /* Suppress a compiler warning about unused variables */
        (void)mode; /* Suppress a compiler warning about unused variables */
        (void)salt; /* Suppress a compiler warning about unused variables */
        (void)saltLength; /* Suppress a compiler warning about unused variables */
        (void)ikm; /* Suppress a compiler warning about unused variables */
        (void)ikmLength; /* Suppress a compiler warning about unused variables */
        (void)prk; /* Suppress a compiler warning about unused variables */
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Hkdf_Extract(base, mode, salt, saltLength, ikm, ikmLength, prk);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }


    return tmpResult;
}



/*******************************************************************************
* Function Name: Cy_Crypto_Core_Hkdf_Expand
****************************************************************************//**
*
* Performs HKDF Expand calculation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param mode
* \ref cy_en_crypto_sha_mode_t
*
* \param prk
* The pointer to the pseudo random key.
*
* \param prkLength
* The length of the prk.
*
* \param info
* The pointer to info.
*
* \param infoLength
* The length of the info.
*
* \param okm
* The pointer to Output key material.
*
* \param okmLength
* The length of okm to be generated.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Hkdf_Expand(CRYPTO_Type *base, cy_en_crypto_sha_mode_t mode,
                                          uint8_t  const *prk,
                                          uint32_t prkLength,
                                          uint8_t  const *info,
                                          uint32_t infoLength,
                                          uint8_t *okm,
                                          uint32_t okmLength)
{
    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        (void)base; /* Suppress a compiler warning about unused variables */
        (void)mode; /* Suppress a compiler warning about unused variables */
        (void)prk; /* Suppress a compiler warning about unused variables */
        (void)prkLength; /* Suppress a compiler warning about unused variables */
        (void)info; /* Suppress a compiler warning about unused variables */
        (void)infoLength; /* Suppress a compiler warning about unused variables */
        (void)okm; /* Suppress a compiler warning about unused variables */
        (void)okmLength; /* Suppress a compiler warning about unused variables */
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Hkdf_Expand(base, mode, prk, prkLength, info, infoLength, okm, okmLength);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;
}

/*******************************************************************************
* Function Name: Cy_Crypto_Core_Hkdf
****************************************************************************//**
*
* Performs HKDF calculation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param mode
* \ref cy_en_crypto_sha_mode_t
*
* \param salt
* The pointer to salt.
*
* \param saltLength
* The size of a salt.
*
* \param ikm
* The pointer to the Input Key material.
*
* \param ikmLength
* The length of the IKM.
*
* \param info
* The pointer to info.
*
* \param infoLength
* The length of the info.
*
* \param okm
* The pointer to Output key material.
*
* \param okmLength
* The length of okm to be generated.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
__STATIC_INLINE cy_en_crypto_status_t Cy_Crypto_Core_Hkdf(CRYPTO_Type *base, cy_en_crypto_sha_mode_t mode,
                                          uint8_t  const *salt,
                                          uint32_t saltLength,
                                          uint8_t  const *ikm,
                                          uint32_t ikmLength,
                                          uint8_t  const *info,
                                          uint32_t infoLength,
                                          uint8_t *okm,
                                          uint32_t okmLength)
{

    cy_en_crypto_status_t tmpResult = CY_CRYPTO_NOT_SUPPORTED;

    if (CY_CRYPTO_V1)
    {
        #if defined(CY_CRYPTO_CFG_HW_V1_ENABLE)
        tmpResult = CY_CRYPTO_NOT_SUPPORTED;
        (void)base; /* Suppress a compiler warning about unused variables */
        (void)mode; /* Suppress a compiler warning about unused variables */
        (void)salt; /* Suppress a compiler warning about unused variables */
        (void)saltLength; /* Suppress a compiler warning about unused variables */
        (void)ikm; /* Suppress a compiler warning about unused variables */
        (void)ikmLength; /* Suppress a compiler warning about unused variables */
        (void)info; /* Suppress a compiler warning about unused variables */
        (void)infoLength; /* Suppress a compiler warning about unused variables */
        (void)okm; /* Suppress a compiler warning about unused variables */
        (void)okmLength; /* Suppress a compiler warning about unused variables */
        #endif /* defined(CY_CRYPTO_CFG_HW_V1_ENABLE) */
    }
    else
    {
        #if defined(CY_CRYPTO_CFG_HW_V2_ENABLE)
        tmpResult = Cy_Crypto_Core_V2_Hkdf(base, mode, salt, saltLength, ikm, ikmLength, info, infoLength,
                                          okm, okmLength);
        #endif /* defined(CY_CRYPTO_CFG_HW_V2_ENABLE) */
    }

    return tmpResult;

}

/** \} group_crypto_lld_kdf_functions */

#endif /* (CPUSS_CRYPTO_SHA == 1) && defined(CY_CRYPTO_CFG_HKDF_C) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTO */

#endif /* #if !defined (CY_CRYPTO_CORE_HKDF_H) */


/* [] END OF FILE */
