/***************************************************************************//**
* \file cy_cryptolite_ecc_key_gen.h
* \version 2.80
*
* \brief
*  This file provides API for generating ECC key.
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

#if !defined (CY_CRYPTOLITE_ECC_KEY_GEN_H)
#define CY_CRYPTOLITE_ECC_KEY_GEN_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"
#include "cy_cryptolite_vu.h"
#include "cy_cryptolite_ecdsa.h"

#if (CRYPTOLITE_VU_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_ECP_C)



/**
* \addtogroup group_cryptolite_lld_keygen_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_MakeKeyPair
****************************************************************************//**
*
* Generate a Private & Public Key pair.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \param curveID
* The ECC curve id.
*
* \param key
* The ECC key. See \ref cy_stc_cryptolite_ecc_key.
*
* \param GetRandomDataFunc
* See \ref cy_func_get_random_data_t.
*
* \param randomDataInfo
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_MakeKeyPair(CRYPTOLITE_Type *base,
        cy_stc_cryptolite_context_ecdsa_t *cfContext,
        cy_en_cryptolite_ecc_curve_id_t curveID,
        cy_stc_cryptolite_ecc_key *key,
        cy_func_get_random_data_t GetRandomDataFunc, void *randomDataInfo);



/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_MakePrivateKey
****************************************************************************//**
*
* Generate a Private key.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \param curveID
* The ECC curve id.
*
* \param privateKey
* The pointer to the ECC private key.
*
* \param GetRandomDataFunc
* See \ref cy_func_get_random_data_t.
*
* \param randomDataInfo
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_MakePrivateKey(CRYPTOLITE_Type *base,
        cy_stc_cryptolite_context_ecdsa_t *cfContext,
        cy_en_cryptolite_ecc_curve_id_t curveID,
        const uint8_t *privateKey,
        cy_func_get_random_data_t GetRandomDataFunc, void *randomDataInfo);


/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_MakePublicKey
****************************************************************************//**
*
* Generate a public key.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \param curveID
* The ECC curve id.
*
* \param privateKey
* The pointer to the ECC private key.
*
* \param publicKey
* The generated public ECC key. See \ref cy_stc_cryptolite_ecc_key.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_MakePublicKey(CRYPTOLITE_Type *base,
        cy_stc_cryptolite_context_ecdsa_t *cfContext,
        cy_en_cryptolite_ecc_curve_id_t curveID,
        const uint8_t *privateKey,
        cy_stc_cryptolite_ecc_key *publicKey);

/** \} group_cryptolite_lld_keygen_functions */

#endif /* #if (CY_CRYPTOLITE_CFG_ECP_C) */
#endif /* #if (CPUSS_CRYPTOLITE_VU == 1) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */
#endif /* #if !defined (CY_CRYPTOLITE_ECC_KEY_GEN_H) */

/* [] END OF FILE */
