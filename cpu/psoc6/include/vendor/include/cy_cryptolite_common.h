/***************************************************************************//**
* \file cy_cryptolite_common.h
* \version 2.80
*
* \brief
*  This file provides common constants and parameters
*  for the Cryptolite driver.
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

#if !defined (CY_CRYPTOLITE_COMMON_H)
#define CY_CRYPTOLITE_COMMON_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cy_cryptolite_hw.h"
#include "cy_cryptolite_config.h"

/**
* \addtogroup group_cryptolite_macros
* \{
*/
/** Driver major version */
#define CY_CRYPTOLITE_DRV_VERSION_MAJOR         2

/** Driver minor version */
#define CY_CRYPTOLITE_DRV_VERSION_MINOR         80

/** Cryptolite Driver PDL ID */
#define CY_CRYPTOLITE_ID                        CY_PDL_DRV_ID(0x74u)
/** \} group_cryptolite_macros */

/** \cond INTERNAL */



/* Calculates the actual size in bytes of the bits value */
#define CY_CRYPTOLITE_BYTE_SIZE_OF_BITS(x)      (uint32_t)(((uint32_t)(x) + 7U) >> 3U)

/* Calculates the word siz of the nearest byte size */
#define CY_CRYPTOLITE_WORD_SIZE_OF_BYTES(x)      (uint32_t)(((uint32_t)(x) + 3U) >> 2U)


/** \endcond */

/**
* \addtogroup group_cryptolite_enums
* \{
*/

/** Errors of the Cryptolite block */
typedef enum
{
    /** Operation completed successfully. */
    CY_CRYPTOLITE_SUCCESS               = 0x00u,
    /** The Crypto operation parameters are incorrect. */
    CY_CRYPTOLITE_BAD_PARAMS            = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x01u,
    /** The Crypto HW is busy. */
    CY_CRYPTOLITE_HW_BUSY               = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x02u,
    /** The Crypto AHB bus error. */
    CY_CRYPTOLITE_BUS_ERROR             = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x03u,
    /** The Crypto feature not supported error. */
    CY_CRYPTOLITE_NOT_SUPPORTED         = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x04u,
   /** The size of input data is not multiple of 16. */
    CY_CRYPTOLITE_SIZE_NOT_X16          = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x05u,
    /** The Address passed is not aligned to 4 bytes. */
    CY_CRYPTOLITE_ALIGNMENT_ERROR       = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x06u,
    /** The TRNG is not enabled. */
    CY_CRYPTOLITE_TRNG_NOT_ENABLED      = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x07u,
    /** The TRNG is unhealthy. */
    CY_CRYPTOLITE_TRNG_UNHEALTHY        = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x08u,
        /** The Data in the buffer is not aligned. */
    CY_CRYPTOLITE_BUFFER_NOT_ALIGNED    = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x09u,
        /** The Hardware error occurred. */
    CY_CRYPTOLITE_HW_ERROR              = CY_CRYPTOLITE_ID | CY_PDL_STATUS_ERROR   | 0x0Au
} cy_en_cryptolite_status_t;

/** \} group_cryptolite_enums */



/** \cond INTERNAL */
/** The cryptolite task descriptor structure.
* All fields for the structure are internal. Firmware never reads or
* writes these values.
*/
typedef struct cy_stc_cryptolite_descr_t {
   uint32_t data0;
   uint32_t data1;
   uint32_t data2;
   uint32_t data3;
} cy_stc_cryptolite_descr_t;
/** \endcond */


/**
* \addtogroup group_cryptolite_enums
* \{
*/
/** Defines modes of SHA method */
typedef enum
{
    CY_CRYPTOLITE_MODE_SHA1          = 0x00u,   /**< Sets the SHA1 mode */
    CY_CRYPTOLITE_MODE_SHA224        = 0x01u,   /**< Sets the SHA224 mode */
    CY_CRYPTOLITE_MODE_SHA256        = 0x02u,   /**< Sets the SHA256 mode */
    CY_CRYPTOLITE_MODE_SHA384        = 0x03u,   /**< Sets the SHA384 mode */
    CY_CRYPTOLITE_MODE_SHA512        = 0x04u,   /**< Sets the SHA512 mode */
    CY_CRYPTOLITE_MODE_SHA512_256    = 0x05u,   /**< Sets the SHA512/256 mode */
    CY_CRYPTOLITE_MODE_SHA512_224    = 0x06u,   /**< Sets the SHA512/224 mode */
    CY_CRYPTOLITE_MODE_SHA_NONE      = 0x07u,   /**< Sets the SHA NONE mode */
} cy_en_cryptolite_sha_mode_t;


/** Signature verification status */
typedef enum
{
    /** The signature is valid */
    CY_CRYPTOLITE_SIG_VALID     = 0x05555555u,
    /** The signature is invalid */
    CY_CRYPTOLITE_SIG_INVALID   = 0x0AAAAAAAu,
} cy_en_cryptolite_sig_verify_result_t;

/** AES CCM verification status */
typedef enum
{
    /** The Tag is valid */
    CY_CRYPTOLITE_TAG_VALID     = 0x05555555u,
    /** The Tag is invalid */
    CY_CRYPTOLITE_TAG_INVALID   = 0x0AAAAAAAu,
} cy_en_cryptolite_ccm_auth_result_t;

/** \} group_cryptolite_enums */


#if defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP521R1_ENABLED)
    #define BIT_SIZE ((uint32_t)521)
#elif defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP384R1_ENABLED)
    #define BIT_SIZE ((uint32_t)384)
#elif defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP256R1_ENABLED)
    #define BIT_SIZE ((uint32_t)256)
#elif defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP224R1_ENABLED)
    #define BIT_SIZE ((uint32_t)224)
#elif defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP192R1_ENABLED)
    #define BIT_SIZE ((uint32_t)192)
#else
    #define BIT_SIZE ((uint32_t)256)
#endif

typedef enum cy_en_cryptolite_ecc_red_mul_algs_t {
    CY_CRYPTOLITE_NIST_P_CURVE_SPECIFIC_RED_ALG = 0,
    CY_CRYPTOLITE_NIST_P_SHIFT_MUL_RED_ALG,
    CY_CRYPTOLITE_NIST_P_BARRETT_RED_ALG
} cy_en_cryptolite_ecc_red_mul_algs_t;


/**
* \addtogroup group_cryptolite_enums
* \{
*/

/** Defines the direction of the Crypto methods */
typedef enum
{
    /** The forward mode, plain text will be encrypted into cipher text */
    CY_CRYPTOLITE_ENCRYPT   = 0x00u,
    /** The reverse mode, cipher text will be decrypted into plain text */
    CY_CRYPTOLITE_DECRYPT   = 0x01u
} cy_en_cryptolite_dir_mode_t;

/** \} group_cryptolite_enums */


/** \cond INTERNAL */
/** Internal hash size for the SHA512 mode (in bytes) */
#define CY_CRYPTOLITE_SHA512_HASH_SIZE          (64u)
/** Internal round mem size for the SHA512 mode (in bytes) */
#define CY_CRYPTOLITE_SHA512_ROUND_MEM_SIZE     (640uL)

#define CY_REMAP_ADDRESS_CRYPTOLITE(addr)   (CY_PLATFORM_REMAP_ADDRESS_CRYPTOLITE(addr))
/** \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_COMMON_H) */
/** \} group_cryptolite */

/* [] END OF FILE */
