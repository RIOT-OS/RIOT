/***************************************************************************//**
* \file cy_cryptolite_ecdsa.h
* \version 2.80
*
* \brief
*  This file provides constant and parameters
*  for the API of the ECDSA PDL in the Cryptolite driver.
*
********************************************************************************
* Copyright 2020-2022 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_ECDSA_H)
#define CY_CRYPTOLITE_ECDSA_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"
#include "cy_cryptolite_vu.h"

#if (CRYPTOLITE_VU_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_ECP_C) && defined(CY_CRYPTOLITE_CFG_ECDSA_C)

/** Pointer to a random number supplier function */
typedef int (*cy_func_get_random_data_t)(void *rndInfo, uint8_t *rndData, size_t rndSize);

#if defined(CY_CRYPTOLITE_CFG_EDDSA_C)

/**
* \addtogroup group_cryptolite_lld_asymmetric_functions
* \{
*/

/** Pointer to a sha initialization function */
typedef cy_en_cryptolite_status_t (*cy_cryptolite_ed25519_sha512_init_t) (void *context);
/** Pointer to a sha update function */
typedef cy_en_cryptolite_status_t (*cy_cryptolite_ed25519_sha512_update_t) (void *context, uint8_t const *input, uint32_t  inputSize);
/** Pointer to a sha start function */
typedef cy_en_cryptolite_status_t (*cy_cryptolite_ed25519_sha512_start_t) (void *context);
/** Pointer to a sha output function */
typedef cy_en_cryptolite_status_t (*cy_cryptolite_ed25519_sha512_finish_t) (void *context, uint8_t *output);
/** Pointer to a sha deinitialization function */
typedef cy_en_cryptolite_status_t (*cy_cryptolite_ed25519_sha512_free_t) (void *context);

/** \} group_cryptolite_lld_asymmetric_functions */

#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_C) */

/**
* \addtogroup group_cryptolite_enums
* \{
*/

/** List of supported elliptic curve IDs */
typedef enum {
    CY_CRYPTOLITE_ECC_ECP_NONE = 0,
#ifdef CY_CRYPTOLITE_CFG_ECP_DP_SECP192R1_ENABLED
    CY_CRYPTOLITE_ECC_ECP_SECP192R1,
#endif
#ifdef CY_CRYPTOLITE_CFG_ECP_DP_SECP224R1_ENABLED
    CY_CRYPTOLITE_ECC_ECP_SECP224R1,
#endif
#ifdef CY_CRYPTOLITE_CFG_ECP_DP_SECP256R1_ENABLED
    CY_CRYPTOLITE_ECC_ECP_SECP256R1,
#endif
#ifdef CY_CRYPTOLITE_CFG_ECP_DP_SECP384R1_ENABLED
    CY_CRYPTOLITE_ECC_ECP_SECP384R1,
#endif
#ifdef CY_CRYPTOLITE_CFG_ECP_DP_SECP521R1_ENABLED
    CY_CRYPTOLITE_ECC_ECP_SECP521R1,
#endif
#if defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED)
    CY_CRYPTOLITE_ECC_ECP_ED25519,
#endif /* defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED) */
#if defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED)
    CY_CRYPTOLITE_ECC_ECP_EC25519,
#endif /* defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED) */
    /* Count of supported curves */
    CY_CRYPTOLITE_ECC_ECP_CURVES_CNT
} cy_en_cryptolite_ecc_curve_id_t;

#if defined(CY_CRYPTOLITE_CFG_EDDSA_C)
/**
 * EDDSA signature operation type.
 *
 * It identifies the signature operation type (pure, ctx or prehash).
 */
typedef enum cy_en_cryptolite_eddsa_sig_type_t
{
    CY_CRYPTOLITE_EDDSA_PURE = 0,   /*!< Pure - uses the entire message, without hashing it previously. */
    CY_CRYPTOLITE_EDDSA_CTX,        /*!< Deterministic context - uses the entire message, without hashing it previously. */
    CY_CRYPTOLITE_EDDSA_PREHASH,    /*!< Pre-hashed message - uses the already hashed message. */
    CY_CRYPTOLITE_EDDSA_NONE        /*!< None - not a valid choice. */
} cy_en_cryptolite_eddsa_sig_type_t;
#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_C) */

/** \} group_cryptolite_enums */

/** A point on a ECC curve */
/** The x and y buffers must be 4 byte aligned*/
typedef struct {
    /** The x co-ordinate */
    void *x;
    /** The y co-ordinate */
    void *y;
} cy_stc_cryptolite_ecc_point;

/** An ECC key type */
typedef enum cy_en_cryptolite_ecc_key_type {
   PK_PUBLIC     = 0u,
   PK_PRIVATE    = 1u
} cy_en_cryptolite_ecc_key_type_t;

/** An ECC key */
typedef struct {
    /** Type of key, PK_PRIVATE or PK_PUBLIC */
    cy_en_cryptolite_ecc_key_type_t type;
    /** See \ref cy_en_cryptolite_ecc_curve_id_t */
    cy_en_cryptolite_ecc_curve_id_t curveID;
    /** The public key */
    cy_stc_cryptolite_ecc_point pubkey;
    /** The private key */
    void *k;
} cy_stc_cryptolite_ecc_key;


/** Structure defines a NIST GF(p) curve */
typedef struct {
    /**  The curve ID */
    cy_en_cryptolite_ecc_curve_id_t id;
    /** The size of the curve in bits */
    uint32_t size;
    /** name of curve */
    const char_t *name;
    /** ECC calculation default algorithm */
    cy_en_cryptolite_ecc_red_mul_algs_t algo;
    /** The prime that defines the field the curve is in (encoded in hex) */
    const uint8_t *prime;
    /** Barrett coefficient for reduction modulo ECC prime (hex) */
    const uint8_t *barrett_p;
    /** The order of the curve (hex) */
    const uint8_t *order;
    /** Barrett coefficient for reduction modulo ECC order (hex) */
    const uint8_t *barrett_o;
    /** The x co-ordinate of the base point on the curve (hex) */
    const uint8_t *Gx;
    /** The y co-ordinate of the base point on the curve (hex) */
    const uint8_t *Gy;
} cy_stc_cryptolite_ecc_dp_type;

/** Structure defines a Edwards GF(p) curve */
typedef struct {
    /**  The curve ID */
    cy_en_cryptolite_ecc_curve_id_t id;
    /** The size of the curve in bits */
    uint32_t size;
    /** name of curve */
    const char_t *name;
    /** ECC calculation default algorithm */
    cy_en_cryptolite_ecc_red_mul_algs_t algo;
    /** The prime that defines the field the curve is in (encoded in hex) */
    const uint8_t *prime;
    /** Barrett coefficient for reduction modulo ECC prime (hex) */
    const uint8_t *barrett_p;
    /** The order of the curve (hex) */
    const uint8_t *order;
    /** Barrett coefficient for reduction modulo ECC order (hex) */
    const uint8_t *barrett_o;
    /** The x co-ordinate of the base point on the curve (hex) */
    const uint8_t *Gx;
    /** The y co-ordinate of the base point on the curve (hex) */
    const uint8_t *Gy;
    /** D (hex) */
    const uint8_t *d;
   /** A (hex) */
    const uint8_t *a;
    /** The size of the barret_order coefs in bits */
    uint32_t barret_osize;
    /** The size of the barret_prime coefs in bits */
    uint32_t barret_psize;
} cy_stc_cryptolite_ec25519_dp_type;

/** Structure defines a ecdsa context buffers
    must be aligned to 4 byte boundary
 */
typedef struct cy_stc_cryptolite_ecc_buffer_t
{
    /** \cond INTERNAL */
    CY_ALIGN(4) uint8_t my_BARRETT_U[4*VU_BITS_TO_WORDS(BIT_SIZE+1)];
    CY_ALIGN(4) uint8_t my_P[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t dividend[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_r[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_s[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_u1[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_u2[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_o[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_gx[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_gy[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_qx[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_qy[4*VU_BITS_TO_WORDS(BIT_SIZE)];
    CY_ALIGN(4) uint8_t p_buf[4*VU_BITS_TO_WORDS(BIT_SIZE+1)];
    /** \endcond */
}cy_stc_cryptolite_ecc_buffer_t;

/**
* \addtogroup group_cryptolite_data_structures
* \{
*/

#if defined(CY_CRYPTOLITE_CFG_EDDSA_C)
/** Structure defines a ed25519 sha function pointers */
typedef struct cy_stc_cryptolite_ed25519_sha512_t
{
    void *shactx;                               /*!< sha context */
    cy_cryptolite_ed25519_sha512_init_t sha_init;          /*!< Initialize sha context */
    cy_cryptolite_ed25519_sha512_update_t sha_update;      /*!< sha update */
    cy_cryptolite_ed25519_sha512_start_t sha_start;        /*!< sha start */
    cy_cryptolite_ed25519_sha512_finish_t sha_finish;      /*!< sha finish */
    cy_cryptolite_ed25519_sha512_free_t sha_free;          /*!< deinitialize sha context */
}cy_stc_cryptolite_ed25519_sha512_t;

#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_C) */

/** Structure defines a ecdsa context */
typedef struct cy_stc_cryptolite_context_ecdsa_t
{
    /** \cond INTERNAL */
    uint8_t *my_BARRETT_U;
    uint8_t *my_P;
    uint8_t *dividend;
    uint8_t *p_r;
    uint8_t *p_s;
    uint8_t *p_u1;
    uint8_t *p_u2;
    uint8_t *p_o;
    uint8_t *p_gx;
    uint8_t *p_gy;
    uint8_t *p_qx;
    uint8_t *p_qy;
    uint8_t *p_buf;
    uint32_t bitsize;
#if defined(CY_CRYPTOLITE_CFG_EDDSA_C)
    cy_stc_cryptolite_ed25519_sha512_t *ed25519_sha;
#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_C) */
    /** two vector unit structures, each structure is 16 bytes */
    cy_stc_cryptolite_descr_t vu_desptr[2];
    /** \endcond */
}cy_stc_cryptolite_context_ecdsa_t;

/** \} group_cryptolite_data_structures */

/* Sizes for NIST P-curves */
#define CY_CRYPTOLITE_ECC_P192_SIZE            (192u)      /* 2^192 - 2^64 - 1 */
#define CY_CRYPTOLITE_ECC_P192_BYTE_SIZE       VU_BITS_TO_BYTES(CY_CRYPTOLITE_ECC_P192_SIZE)

#define CY_CRYPTOLITE_ECC_P224_SIZE            (224u)      /* 2^224 - 2^96  + 1 */
#define CY_CRYPTOLITE_ECC_P224_BYTE_SIZE       VU_BITS_TO_BYTES(CY_CRYPTOLITE_ECC_P224_SIZE)

#define CY_CRYPTOLITE_ECC_P256_SIZE            (256u)      /* 2^256 - 2^224 + 2^192 + 2^96 - 1 */
#define CY_CRYPTOLITE_ECC_P256_BYTE_SIZE       VU_BITS_TO_BYTES(CY_CRYPTOLITE_ECC_P256_SIZE)

#define CY_CRYPTOLITE_ECC_P384_SIZE            (384u)      /* 2^384 - 2^128 - 2^96 + 2^32 - 1 */
#define CY_CRYPTOLITE_ECC_P384_BYTE_SIZE       VU_BITS_TO_BYTES(CY_CRYPTOLITE_ECC_P384_SIZE)

#define CY_CRYPTOLITE_ECC_P521_SIZE            (521u)      /* 2^521 - 1 */
#define CY_CRYPTOLITE_ECC_P521_BYTE_SIZE       VU_BITS_TO_BYTES(CY_CRYPTOLITE_ECC_P521_SIZE)

#define CY_CRYPTOLITE_ECC_MAX_SIZE             (CY_CRYPTOLITE_ECC_P521_SIZE)
#define CY_CRYPTOLITE_ECC_MAX_BYTE_SIZE        (CY_CRYPTOLITE_ECC_P521_BYTE_SIZE)

#if defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED) || defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED)
#define CY_CRYPTOLITE_ECC_ED25519_SIZE         (255u)      /* 2^555 - 19 */
#define CY_CRYPTOLITE_ECC_ED25519_BYTE_SIZE    CY_CRYPTOLITE_BYTE_SIZE_OF_BITS(CY_CRYPTOLITE_ECC_ED25519_SIZE)
#define CY_CRYPTOLITE_ECC_EC25519_SIZE         (255u)      /* 2^555 - 19 */
#define CY_CRYPTOLITE_ECC_EC25519_BYTE_SIZE    CY_CRYPTOLITE_BYTE_SIZE_OF_BITS(CY_CRYPTOLITE_ECC_EC25519_SIZE)
#endif /* defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED) || defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED)  */

cy_stc_cryptolite_ecc_dp_type *Cy_Cryptolite_ECC_GetCurveParams(cy_en_cryptolite_ecc_curve_id_t curveId);
cy_stc_cryptolite_ec25519_dp_type *Cy_Cryptolite_EC25519_GetCurveParams(cy_en_cryptolite_ecc_curve_id_t curveId);


#if defined(CY_CRYPTOLITE_CFG_EDDSA_C) || defined (CY_DOXYGEN)

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_MulMod( CRYPTOLITE_Type *base,
    cy_stc_cryptolite_context_ecdsa_t *cfContext,
    uint8_t *p_z,
    uint8_t *p_a,
    uint8_t *p_b,
    uint32_t size);

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_SquareMod( CRYPTOLITE_Type *base,
    cy_stc_cryptolite_context_ecdsa_t *cfContext,
    uint8_t *p_z,
    uint8_t *p_a,
    uint32_t size);

#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_C) */

#if defined(CY_CRYPTOLITE_CFG_ECDSA_C)
/**
* \addtogroup group_cryptolite_lld_asymmetric_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_Init
****************************************************************************//**
*
* Init ECC Context.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \param eccBuffer
* The pointer to the cy_stc_cryptolite_ecc_buffer_t.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Ecc_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_Init(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                            cy_stc_cryptolite_ecc_buffer_t *eccBuffer);


/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_Free
****************************************************************************//**
*
* Clean up ECC Context.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Ecc_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_Free(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext);



/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_SignHash
****************************************************************************//**
*
* Function to generate an ECC signature.
* key, hash and messageKey must be in little endian.
* Cy_Cryptolite_InvertEndianness() function is used for converting the endianness.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the cy_stc_cryptolite_context_ecdsa_t.
*
* \param hash
* The hash (message digest) to be signed.
*
* \param hashlen
* The length of the hash (octets).
*
* \param sig
* The pointer to the buffer to store the generated signature 'R' followed by 'S'.
*
* \param key
* The  private ECC key to sign the hash. See \ref cy_stc_cryptolite_ecc_key.
*
* \param messageKey
* The random number for generating the signature.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_SignHash(CRYPTOLITE_Type *base,
cy_stc_cryptolite_context_ecdsa_t *cfContext, const uint8_t *hash, uint32_t hashlen, uint8_t *sig,
        const cy_stc_cryptolite_ecc_key *key, const uint8_t *messageKey);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Core_ECC_VerifyHash
****************************************************************************//**
*
* Verify an ECC signature.
* sig, hash and key must be in little endian.
* Cy_Cryptolite_InvertEndianness() function is used for converting the endianness.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param cfContext
* The pointer to the ecdsa context.
*
* \param sig
* The signature to verify, 'R' followed by 'S'.
*
* \param siglen
* The length of signature.
*
* \param hash
* The hash (message digest) that was signed.
*
* \param hashlen
* The length of the hash (octets).
*
* \param stat
* Result of signature, 0xAAAAAAAA = invalid, 0x55555555 = valid.
*
* \param key
* The corresponding public ECC key. See \ref cy_stc_cryptolite_ecc_key.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Ecc_verify
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_VerifyHash(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                            const uint8_t *sig, uint32_t siglen, const uint8_t *hash, uint32_t hashlen,
                            cy_en_cryptolite_sig_verify_result_t *stat, const cy_stc_cryptolite_ecc_key *key);


/*******************************************************************************
* Function Name: Cy_Cryptolite_ECC_SharedSecret
****************************************************************************//**
*
* Generate a Shared Secret key from one private key and others public key.
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
* \param key
* The generated public ECC key. See \ref cy_stc_cryptolite_ecc_key.
*
* \param sharedSecret
* The pointer to store the generated shared Secret.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_ECC_SharedSecret(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                            cy_en_cryptolite_ecc_curve_id_t curveID,const uint8_t *privateKey,
                            const cy_stc_cryptolite_ecc_key *key,
                            uint8_t const *sharedSecret);
#endif

#if defined(CY_CRYPTOLITE_CFG_EDDSA_C)

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_Init(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    cy_stc_cryptolite_ecc_buffer_t *eccBuffer,
                                    cy_stc_cryptolite_ed25519_sha512_t *shaFunctions);


cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_Free(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext);

#endif /*CY_CRYPTOLITE_CFG_EDDSA_C*/

#if defined(CY_CRYPTOLITE_CFG_EDDSA_SIGN_C)

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_Sign(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    const uint8_t *hash,
                                    uint32_t hashlen,
                                    uint8_t *sig,
                                    const cy_stc_cryptolite_ecc_key *key,
                                    cy_en_cryptolite_eddsa_sig_type_t sigType,
                                    const uint8_t *sigctx,
                                    uint32_t sigctx_len);

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_PointMultiplication(CRYPTOLITE_Type *base,
                                        cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                        const uint8_t *ecpGX,
                                        const uint8_t *ecpGY,
                                        const uint8_t *ecpD,
                                        uint8_t *ecpQX,
                                        uint8_t *ecpQY);


#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_SIGN_C) */

#if defined(CY_CRYPTOLITE_CFG_EDDSA_VERIFY_C)
cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_PointDecode(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    const uint8_t *publicKey,
                                    uint8_t *pubKey_x, uint8_t *pubKey_y);

cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_Verify(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    uint8_t *sig,
                                    const uint8_t *hash, uint32_t hashlen,
                                    const cy_stc_cryptolite_ecc_key *key,
                                    uint32_t *stat,
                                    cy_en_cryptolite_eddsa_sig_type_t sigType,
                                    const uint8_t *sigctx, uint32_t sigctx_len);
#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_VERIFY_C) */

#if defined (CY_CRYPTOLITE_CFG_EDDSA_GENKEY_C)
cy_en_cryptolite_status_t Cy_Cryptolite_ED25519_MakePublicKey(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    const uint8_t *privateKey,
                                    cy_stc_cryptolite_ecc_key *publicKey);
#endif /* defined(CY_CRYPTOLITE_CFG_EDDSA_GENKEY_C) */

#if defined (CY_CRYPTOLITE_CFG_EC25519_GENKEY_C)
cy_en_cryptolite_status_t Cy_Cryptolite_EC25519_MakePublicKey(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    const uint8_t *privateKey,
                                    cy_stc_cryptolite_ecc_key *publicKey);

cy_en_cryptolite_status_t Cy_Cryptolite_EC25519_MakePrivateKey(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    uint8_t *privateKey,
                                    cy_func_get_random_data_t GetRandomDataFunc,
                                    void *randomDataInfo);
#endif /* defined (CY_CRYPTOLITE_CFG_EC25519_GENKEY_C) */

#if defined (CY_CRYPTOLITE_CFG_EC25519_C)
cy_en_cryptolite_status_t Cy_Cryptolite_EC25519_PointMultiplication(CRYPTOLITE_Type *base,
                                    cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                    uint8_t *p_r,
                                    const uint8_t *p_x,
                                    const uint8_t *p_d);
#endif /* defined (CY_CRYPTOLITE_CFG_EC25519_C) */

/** \} group_cryptolite_lld_asymmetric_functions */

#endif /* #if (CY_CRYPTOLITE_CFG_ECDSA_C) */
#endif /* #if (CPUSS_CRYPTOLITE_VU == 1) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */
#endif /* #if !defined (CY_CRYPTOLITE_ECDSA_H) */

/* [] END OF FILE */
