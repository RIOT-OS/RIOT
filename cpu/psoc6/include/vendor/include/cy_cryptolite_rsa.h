/***************************************************************************//**
* \file cy_cryptolite_rsa.h
* \version 2.80
*
* \brief
*  This file provides provides constant and parameters
*  for the API of the RSA in the Cryptolite driver.
*
********************************************************************************
* Copyright 2020-2021 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_RSA_H)
#define CY_CRYPTOLITE_RSA_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"
#include "cy_cryptolite_vu.h"

#if (CRYPTOLITE_VU_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_RSA_C)

/* Max key size to support must be defined */
#if defined (CY_CRYPTOLITE_CFG_RSA_MAX_KEY_512)
    #define CY_CRYPTOLITE_RSA_BITSIZE  (512U)
#elif defined (CY_CRYPTOLITE_CFG_RSA_MAX_KEY_1024)
    #define CY_CRYPTOLITE_RSA_BITSIZE (1024U)
#elif defined (CY_CRYPTOLITE_CFG_RSA_MAX_KEY_2048)
    #define CY_CRYPTOLITE_RSA_BITSIZE (2048U)
#elif defined(CY_CRYPTOLITE_CFG_RSA_MAX_KEY_3072)
    #define CY_CRYPTOLITE_RSA_BITSIZE (3072U)
#elif defined(CY_CRYPTOLITE_CFG_RSA_MAX_KEY_4096)
    #define CY_CRYPTOLITE_RSA_BITSIZE (4096U)
#else
    #define CY_CRYPTOLITE_RSA_BITSIZE (4096U)
#endif


/* The buffer size needed by the IP for the internal RSA calculation*/
#define CY_CRYPTOLITE_RSA_BUFFER_SIZE (4U*VU_BITS_TO_WORDS(2U*CY_CRYPTOLITE_RSA_BITSIZE+1U) \
                 + 4U*VU_BITS_TO_WORDS(2U*CY_CRYPTOLITE_RSA_BITSIZE+1U)\
                 + 4U*VU_BITS_TO_WORDS(CY_CRYPTOLITE_RSA_BITSIZE) + 4U*VU_BITS_TO_WORDS(CY_CRYPTOLITE_RSA_BITSIZE))


/**
* \addtogroup group_cryptolite_data_structures
* \{
*/

/**
* All fields for the context structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in the function calls. Firmware must
* ensure that the defined instance of this structure remains in scope
* while the drive is in use.
*
* The driver uses this structure to store and manipulate the RSA public key and
* additional coefficients to accelerate RSA calculation.
*
*  RSA key contained from two fields:
*  - n - modulus part of the key
*  - e - exponent part of the key.
*
* Other fields are accelerating coefficients and can be calculated by
* Cy_Cryptolite_Rsa_Coef API.
*
* \note All the buffers must be 4 byte aligned in the
* \ref cy_stc_cryptolite_rsa_pub_key_t and must also be in little-endian order.<br>
* Use Cy_Cryptolite_InvertEndianness function to convert to or from
* little-endian order.
*/
CY_ALIGN(4)
typedef struct
{
    /** The pointer to the modulus part of public key. */
    uint8_t *moduloPtr;
    /** The modulus length, in bits, maximum supported size is 4096 Bits */
    uint32_t moduloLength;

    /** The pointer to the exponent part of public key */
    uint8_t *pubExpPtr;
    /** The exponent length, in bits, maximum supported size is 256Bit */
    uint32_t pubExpLength;

    /** The pointer to the Barrett coefficient. Memory for it should be
        allocated by user with size moduloLength + 1. */
    uint8_t *barretCoefPtr;

    /** The pointer to the binary inverse of the modulo. Memory for it
        should be allocated by user with size moduloLength. */
    uint8_t *inverseModuloPtr;

    /** The pointer to the (2^moduloLength mod modulo). Memory for it should
        be allocated by user with size moduloLength */
    uint8_t *rBarPtr;

    /** when set to 'true', pre-calculated coefficients must be provided in 'barretCoefPtr, 'inverseModuloPtr' and 'rBarPtr',
        can be calculated by  Cy_Cryptolite_Rsa_Coef API .
        When set to 'false', all the acceleration step coefficients will be calculated as part of \ref Cy_Cryptolite_Rsa_Proc */
    bool preCalculatedCoeff;

} cy_stc_cryptolite_rsa_pub_key_t;

/*All buffers must be 4 byte aligned*/
typedef struct cy_stc_cryptolite_rsa_buffer_t
{
    /** \cond INTERNAL */
    CY_ALIGN(4) uint8_t p_buffer[CY_CRYPTOLITE_RSA_BUFFER_SIZE];
    /*Assigned dummy byte for the 4 byte alignment*/
    CY_ALIGN(4) uint32_t dummy[1u];
    /** \endcond */
}cy_stc_cryptolite_rsa_buffer_t;

typedef struct cy_stc_cryptolite_context_rsa_t
{
    /** \cond INTERNAL */
    uint8_t *p_buffer;
   /* two vector unit structures, each structure is 16 bytes*/
    cy_stc_cryptolite_descr_t vu_desptr[2];
    /** \endcond */
}cy_stc_cryptolite_context_rsa_t;

/** \} group_cryptolite_data_structures */


/**
* \addtogroup group_cryptolite_lld_asymmetric_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Init
****************************************************************************//**
*
* Initialize the RSA context.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the CRYPTOLITE context.
*
* \param rsaBuffers
* The pointer to the RSA buffers.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_rsa_signature_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Init(CRYPTOLITE_Type *base,
                                              cy_stc_cryptolite_context_rsa_t *cfContext,
                                              cy_stc_cryptolite_rsa_buffer_t *rsaBuffers);



/*******************************************************************************
* Function Name: Cy_Cryptolite_Core_Rsa_Proc
****************************************************************************//**
*
* RSA process algorithm based on the Montgomery algorithm
* using Barrett reduction
*
* https://en.wikipedia.org/wiki/RSA_%28cryptosystem%29
*
* Key, message, processedMessage  buffers must be 4 byte aligned and end with 4 byte boundary.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the CRYPTOLITE context.
*
* \param key
* The pointer to the \ref cy_stc_cryptolite_rsa_pub_key_t structure that stores
* public key.
*
* \param message
* The pointer to the message to be processed.
*
* \param messageSize
* The length of the message to be processed.
*
* \param processedMessage
* The pointer to processed message.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_rsa_signature_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Proc(CRYPTOLITE_Type *base,
                                              cy_stc_cryptolite_context_rsa_t *cfContext,
                                              cy_stc_cryptolite_rsa_pub_key_t *key,
                                              uint8_t const *message,
                                              uint32_t messageSize,
                                              uint8_t *processedMessage);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Coef
****************************************************************************//**
*
* Calculation constant coefficients to speed-up Montgomery algorithm.
* These coefficients are:
*                         coefficient for Barrett reduction,
*                         binary inverse of the modulo,
*                         result of (2^moduloLength mod modulo)
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the CRYPTOLITE context.
*
* \param key
* The pointer to the \ref cy_stc_cryptolite_rsa_pub_key_t structure that stores a
* public key.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Coef(CRYPTOLITE_Type *base,
                                              cy_stc_cryptolite_context_rsa_t *cfContext,
                                              cy_stc_cryptolite_rsa_pub_key_t *key);



/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Free
****************************************************************************//**
*
* Cleanup the RSA context.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the CRYPTOLITE context.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_rsa_signature_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Free(CRYPTOLITE_Type *base,
                                              cy_stc_cryptolite_context_rsa_t *cfContext);

#ifdef CY_CRYPTOLITE_CFG_RSA_VERIFY_ENABLED

/*******************************************************************************
* Function Name: Cy_Cryptolite_Rsa_Verify
****************************************************************************//**
*
* RSA verification with checks for content, paddings and signature format.
* SHA digest of the message and decrypted message should be calculated before.
* Supports only PKCS1-v1_5 format, inside of this format supported padding
* using only SHA, cases with MD2 and MD5 are not supported.
* PKCS1-v1_5 described here, page 31:
* http://www.emc.com/collateral/white-papers/h11300-pkcs-1v2-2-rsa-cryptography-standard-wp.pdf
*
* The digest and decryptedSignature buffers must be 4 byte aligned
*
* Returns the verification result \ref cy_en_cryptolite_sig_verify_result_t.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cfContext
* The pointer to the CRYPTOLITE context.
*
* \param verResult
* The pointer to the verification result \ref cy_en_cryptolite_sig_verify_result_t.
*
* \param digestType
* SHA mode used for hash calculation \ref cy_en_cryptolite_sha_mode_t.
*
* \param digest
* The pointer to the hash of the message or the message whose signature is to be verified.
*
* \param digestLength
* The length of the message whose signature is to be verified and is applicable for CY_CRYPTOLITE_MODE_SHA_NONE mode.
*
* \param decryptedSignature
* The pointer to the decrypted signature to be verified.
*
* \param decryptedSignatureLength
* The length of the decrypted signature to be verified (in bytes)
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_rsa_signature_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Rsa_Verify(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_rsa_t *cfContext,
                            cy_en_cryptolite_sig_verify_result_t *verResult,
                            cy_en_cryptolite_sha_mode_t digestType,
                            uint8_t const *digest,
                            uint32_t digestLength,
                            uint8_t const *decryptedSignature,
                            uint32_t decryptedSignatureLength);
#endif

/** \} group_cryptolite_lld_asymmetric_functions */

#endif /*#if (CY_CRYPTOLITE_CFG_RSA_C)*/
#endif /* #if (CPUSS_CRYPTOLITE_VU == 1) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_RSA_H) */

/* [] END OF FILE */
