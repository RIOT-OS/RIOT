/***************************************************************************//**
* \file cy_cryptolite_config.h
* \version 2.80
*
* \brief
*  This file provides default configuration parameters
*  for the Cryptolite PDL driver.
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


#if !defined(CY_CRYPTOLITE_CONFIG_H)
#define CY_CRYPTOLITE_CONFIG_H

#if defined(CY_CRYPTOLITE_USER_CONFIG_FILE)

#include CY_CRYPTOLITE_USER_CONFIG_FILE

#else
/* Default configuration:
 *
 * - SHA256
 * - HMAC
 * - RSA encrypt/decrypt
 * - RSA PKCS#1.l5 verification
 * - ECP multiplication
 * - ECDSA verification
 */
/*SHA and HMAC*/
#define CY_CRYPTOLITE_CFG_SHA_C
#define CY_CRYPTOLITE_CFG_HMAC_C
#define CY_CRYPTOLITE_CFG_SHA2_256_ENABLED
#define CY_CRYPTOLITE_CFG_HKDF_C

#define CY_CRYPTOLITE_CFG_TRNG_C
#define CY_CRYPTOLITE_CFG_AES_C
#define CY_CRYPTOLITE_CFG_CMAC_C
#define CY_CRYPTOLITE_CFG_CBC_MAC_C

/* AES Cipher modes */
#define CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC
#define CY_CRYPTOLITE_CFG_CIPHER_MODE_CFB
#define CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR
#define CY_CRYPTOLITE_CFG_CIPHER_MODE_CCM

/* RSA functionality */
#define CY_CRYPTOLITE_CFG_RSA_C
/* RSA PKCS 1.5 verification */
#define CY_CRYPTOLITE_CFG_RSA_VERIFY_ENABLED
/* RSA Max Key size */
#define CY_CRYPTOLITE_CFG_RSA_MAX_KEY_4096
/* RSA verify SHA mode */
#define CY_CRYPTOLITE_CFG_RSA_VERIFY_SHA1
#define CY_CRYPTOLITE_CFG_RSA_VERIFY_SHA256
#define CY_CRYPTOLITE_CFG_RSA_VERIFY_SHA512

/* Only NIST-P curves are currently supported */
#define CY_CRYPTOLITE_CFG_ECP_C

#define CY_CRYPTOLITE_CFG_ECP_DP_SECP192R1_ENABLED
#define CY_CRYPTOLITE_CFG_ECP_DP_SECP224R1_ENABLED
#define CY_CRYPTOLITE_CFG_ECP_DP_SECP256R1_ENABLED
#define CY_CRYPTOLITE_CFG_ECP_DP_SECP384R1_ENABLED
//#define CY_CRYPTOLITE_CFG_ECP_DP_SECP521R1_ENABLED
#define CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED
#define CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED

/* ECDSA functionality */
#define CY_CRYPTOLITE_CFG_ECDSA_C
/* ECDSA Sign */
#define CY_CRYPTOLITE_CFG_ECDSA_SIGN_C
/* ECDSA verification */
#define CY_CRYPTOLITE_CFG_ECDSA_VERIFY_C

/* EC25519 functionality */
#define CY_CRYPTOLITE_CFG_EC25519_C
/* EC25519 key generation */
#define CY_CRYPTOLITE_CFG_EC25519_GENKEY_C

/* EDDSA functionality */
#define CY_CRYPTOLITE_CFG_EDDSA_C
/* EDDSA sign */
#define CY_CRYPTOLITE_CFG_EDDSA_SIGN_C
/* EDDSA verify */
#define CY_CRYPTOLITE_CFG_EDDSA_VERIFY_C
/* EDDSA key generation */
#define CY_CRYPTOLITE_CFG_EDDSA_GENKEY_C

#endif

#include "cy_device_headers.h"

/* Check hardware configuration */
#if !defined(CY_IP_MXCRYPTOLITE)
#error "CY_IP_MXCRYPTOLITE configuration not available"
#endif /* !defined(CY_IP_MXCRYPTOLITE) */

/* Check SHA configuration */
#if defined(CY_CRYPTOLITE_CFG_SHA2_256_ENABLED) \
     && !defined(CY_CRYPTOLITE_CFG_SHA_C)
#error "CY_CRYPTOLITE_CFG_SHA_C is not defined to use SHA digests"
#endif

#if defined(CY_CRYPTOLITE_CFG_SHA_C) && (!defined(CY_CRYPTOLITE_CFG_SHA2_256_ENABLED))
#error "CY_CRYPTOLITE_CFG_SHA_C is defined but no SHA mode is selected"
#endif

/* Check HMAC configuration */
#if defined(CY_CRYPTOLITE_CFG_HMAC_C) && !defined(CY_CRYPTOLITE_CFG_SHA_C)
#error "CY_CRYPTOLITE_CFG_SHA_C is not defined to use HMAC calculation"
#endif /* defined(CY_CRYPTOLITE_CFG_HMAC_C) && !defined(CY_CRYPTOLITE_CFG_SHA_C) */

/* Check HKDF configuration */
#if defined(CY_CRYPTOLITE_CFG_HKDF_C) && !defined(CY_CRYPTOLITE_CFG_HMAC_C)
#error "CY_CRYPTOLITE_CFG_HMAC_C is not defined to use HMAC calculation"
#endif /* defined(CY_CRYPTOLITE_CFG_HKDF_C) && !defined(CY_CRYPTOLITE_CFG_HMAC_C) */

/* Check RSA verify configuration */
#if defined(CY_CRYPTOLITE_CFG_RSA_VERIFY_ENABLED) && !defined(CY_CRYPTOLITE_CFG_RSA_C)
#error "CY_CRYPTOLITE_CFG_RSA_C is not defined to use RSA verification functionality"
#endif

#if defined(CY_CRYPTOLITE_CFG_RSA_VERIFY_ENABLED) && !defined(CY_CRYPTOLITE_CFG_SHA_C)
#error "CY_CRYPTOLITE_CFG_SHA_C is not defined to use RSA verification functionality"
#endif

/* Check ECDSA configuration */
#if (defined(CY_CRYPTOLITE_CFG_ECDSA_VERIFY_C) && !defined(CY_CRYPTOLITE_CFG_ECDSA_C))
#error "CY_CRYPTOLITE_CFG_ECDSA_C is not defined to use ECDSA functionalities"
#endif

#if (defined(CY_CRYPTOLITE_CFG_ECDSA_SIGN_C) && !defined(CY_CRYPTOLITE_CFG_ECDSA_C))
#error "CY_CRYPTOLITE_CFG_ECDSA_C is not defined to use ECDSA functionalities"
#endif

/* Check AES CCM configuration */
#if (defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CCM) && (!defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR) || !defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC)))
#error "CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR / CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC  is not defined to use AES CCM  functionalities"
#endif

/* Check AES CMAC configuration */
#if (defined(CY_CRYPTOLITE_CFG_CMAC_C) && (!defined(CY_CRYPTOLITE_CFG_AES_C) || !defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC)))
#error "CY_CRYPTOLITE_CFG_AES_C / CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC  is not defined to use AES CMAC  functionalities"
#endif

/* Check AES CBC-MAC configuration */
#if (defined(CY_CRYPTOLITE_CFG_CBC_MAC_C) && (!defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC)))
#error "CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC  is not defined to use CBC-MAC  functionalities"
#endif

#if defined(CY_CRYPTOLITE_CFG_ECDSA_C) && !defined(CY_CRYPTOLITE_CFG_ECP_C)
#define CY_CRYPTOLITE_CFG_ECP_C
#endif /* defined(CY_CRYPTOLITE_CFG_ECDSA_C) && !defined(CY_CRYPTOLITE_CFG_ECP_C) */

/* Check ECP configuration */
#if !(defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP192R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP224R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP256R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP384R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP521R1_ENABLED)) \
     && defined(CY_CRYPTOLITE_CFG_ECP_C)
#error "CY_CRYPTOLITE_CFG_ECP_C is defined but no curve is selected"
#endif

#if (defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP192R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP224R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP256R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP384R1_ENABLED) || \
     defined(CY_CRYPTOLITE_CFG_ECP_DP_SECP521R1_ENABLED)) \
     && !defined(CY_CRYPTOLITE_CFG_ECP_C)
#error "CY_CRYPTOLITE_CFG_ECP_C is not defined to use ECP functionality"
#endif

#if !(defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED)) \
     && defined(CY_CRYPTOLITE_CFG_EDDSA_C)
#error "CY_CRYPTOLITE_CFG_EDDSA_C is defined but no curve is selected"
#endif

#if (defined(CY_CRYPTOLITE_CFG_ECP_DP_ED25519_ENABLED)) \
     && !defined(CY_CRYPTOLITE_CFG_EDDSA_C)
#error "CY_CRYPTOLITE_CFG_EDDSA_C is not defined to use EDDSA functionality"
#endif

#if !(defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED)) \
     && defined(CY_CRYPTOLITE_CFG_EC25519_C)
#error "CY_CRYPTOLITE_CFG_EC25519_C is defined but no curve is selected"
#endif

#if (defined(CY_CRYPTOLITE_CFG_ECP_DP_EC25519_ENABLED)) \
     && !defined(CY_CRYPTOLITE_CFG_EC25519_C)
#error "CY_CRYPTOLITE_CFG_EC25519_C is not defined to use EC25519 functionality"
#endif

#if (defined(CY_CRYPTOLITE_CFG_EC25519_GENKEY_C)) \
     && !defined(CY_CRYPTOLITE_CFG_EC25519_C)
#error "CY_CRYPTOLITE_CFG_EC25519_C is not defined to use EC25519 functionality"
#endif

#endif /* #if !defined(CY_CRYPTOLITE_CONFIG_H) */

/* [] END OF FILE */
