/**
 * @defgroup pkg_mbedtls_config Mbed TLS package compile configurations
 * @ingroup pkg_mbedtls
 *
 * @{
 * @file
 *
 * @brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef RIOT_MBEDTLS_CONFIG_H
#define RIOT_MBEDTLS_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ECP supprted curves
 * @{
 */
/* Short Weierstrass curves (supporting ECP, ECDH, ECDSA) */
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
/* Montgomery curves (supporting ECP) */
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_DP_CURVE448_ENABLED
/** @} */

/**
 * @brief   Use precomputed AES tables stored in ROM.
 *
 * Uncomment this macro to use precomputed AES tables stored in ROM.
 * Comment this macro to generate AES tables in RAM at runtime.
 *
 * Tradeoff: Using precomputed ROM tables reduces RAM usage by ~8kb
 * (or ~2kb if \c MBEDTLS_AES_FEWER_TABLES is used) and reduces the
 * initialization time before the first AES operation can be performed.
 * It comes at the cost of additional ~8kb ROM use (resp. ~2kb if \c
 * MBEDTLS_AES_FEWER_TABLES below is used), and potentially degraded
 * performance if ROM access is slower than RAM access.
 *
 * This option is independent of \c MBEDTLS_AES_FEWER_TABLES.
 */
#define MBEDTLS_AES_ROM_TABLES

/**
 * @brief   Use less ROM/RAM for AES tables.
 *
 * Uncommenting this macro omits 75% of the AES tables from
 * ROM / RAM (depending on the value of \c MBEDTLS_AES_ROM_TABLES)
 * by computing their values on the fly during operations
 * (the tables are entry-wise rotations of one another).
 *
 * Tradeoff: Uncommenting this reduces the RAM / ROM footprint
 * by ~6kb but at the cost of more arithmetic operations during
 * runtime. Specifically, one has to compare 4 accesses within
 * different tables to 4 accesses with additional arithmetic
 * operations within the same table. The performance gain/loss
 * depends on the system and memory details.
 *
 * This option is independent of \c MBEDTLS_AES_ROM_TABLES.
 */
#define MBEDTLS_AES_FEWER_TABLES

/**
 * @cond
 * This translates RIOT exposed options to Mbed TLS macros, it is hidden from Doxygen.
 */
#undef MBEDTLS_SHA256_ALT
#if defined(CONFIG_MBEDTLS_SHA256_ALT)
#define MBEDTLS_SHA256_ALT                          CONFIG_MBEDTLS_SHA256_ALT
#endif
#undef MBEDTLS_ENTROPY_HARDWARE_ALT
#if defined(CONFIG_MBEDTLS_ENTROPY_HARDWARE_ALT)
#define MBEDTLS_ENTROPY_HARDWARE_ALT                CONFIG_MBEDTLS_ENTROPY_HARDWARE_ALT
#endif
#undef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED            CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED        CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED      CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED        CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED            CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED        CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED      CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED    CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED     CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED       CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#endif
#undef MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#if defined (CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
#define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED        CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#endif
#undef MBEDTLS_NO_PLATFORM_ENTROPY
#if defined(CONFIG_MBEDTLS_NO_PLATFORM_ENTROPY)
#define MBEDTLS_NO_PLATFORM_ENTROPY                 CONFIG_MBEDTLS_NO_PLATFORM_ENTROPY
#endif
#undef MBEDTLS_ENTROPY_FORCE_SHA256
#if defined(CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256)
#define MBEDTLS_ENTROPY_FORCE_SHA256                CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256
#endif
#undef MBEDTLS_PKCS1_V15
#if defined(CONFIG_MBEDTLS_PKCS1_V15)
#define MBEDTLS_PKCS1_V15                           CONFIG_MBEDTLS_PKCS1_V15
#endif
#undef MBEDTLS_PKCS1_V21
#if defined(CONFIG_MBEDTLS_PKCS1_V21)
#define MBEDTLS_PKCS1_V21                           CONFIG_MBEDTLS_PKCS1_V21
#endif
#undef MBEDTLS_SELF_TEST
#if defined(CONFIG_MBEDTLS_SELF_TEST)
#define MBEDTLS_SELF_TEST                           CONFIG_MBEDTLS_SELF_TEST
#endif
#undef MBEDTLS_SHA512_NO_SHA384
#if defined(CONFIG_MBEDTLS_SHA512_NO_SHA384)
#define MBEDTLS_SHA512_NO_SHA384                    CONFIG_MBEDTLS_SHA512_NO_SHA384
#endif
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID
#if defined(CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID)
#define MBEDTLS_SSL_DTLS_CONNECTION_ID              CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID
#endif
#undef MBEDTLS_SSL_ENCRYPT_THEN_MAC
#if defined(CONFIG_MBEDTLS_SSL_ENCRYPT_THEN_MAC)
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC                CONFIG_MBEDTLS_SSL_ENCRYPT_THEN_MAC
#endif
#undef MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#if defined(CONFIG_MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET          CONFIG_MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#endif
#undef MBEDTLS_SSL_PROTO_TLS1
#if defined(CONFIG_MBEDTLS_SSL_PROTO_TLS1)
#define MBEDTLS_SSL_PROTO_TLS1                      CONFIG_MBEDTLS_SSL_PROTO_TLS1
#endif
#undef MBEDTLS_SSL_PROTO_TLS1_1
#if defined(CONFIG_MBEDTLS_SSL_PROTO_TLS1_1)
#define MBEDTLS_SSL_PROTO_TLS1_1                    CONFIG_MBEDTLS_SSL_PROTO_TLS1_1
#endif
#undef MBEDTLS_SSL_PROTO_TLS1_2
#if defined(CONFIG_MBEDTLS_SSL_PROTO_TLS1_2)
#define MBEDTLS_SSL_PROTO_TLS1_2                    CONFIG_MBEDTLS_SSL_PROTO_TLS1_2
#endif
#undef MBEDTLS_SSL_PROTO_DTLS
#if defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
#define MBEDTLS_SSL_PROTO_DTLS                      CONFIG_MBEDTLS_SSL_PROTO_DTLS
#endif
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#if defined(CONFIG_MBEDTLS_SSL_DTLS_ANTI_REPLAY)
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY                CONFIG_MBEDTLS_SSL_DTLS_ANTI_REPLAY
#endif
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#if defined(CONFIG_MBEDTLS_SSL_DTLS_HELLO_VERIFY)
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY               CONFIG_MBEDTLS_SSL_DTLS_HELLO_VERIFY
#endif
#undef MBEDTLS_SSL_DTLS_SRTP
#if defined(CONFIG_MBEDTLS_SSL_DTLS_SRTP)
#define MBEDTLS_SSL_DTLS_SRTP                       CONFIG_MBEDTLS_SSL_DTLS_SRTP
#endif
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#if defined(CONFIG_MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE)
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE          CONFIG_MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#endif
#undef MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#if defined(CONFIG_MBEDTLS_SSL_DTLS_BADMAC_LIMIT)
#define MBEDTLS_SSL_DTLS_BADMAC_LIMIT               CONFIG_MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#endif
#undef MBEDTLS_SSL_SERVER_NAME_INDICATION
#if defined(CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION)
#define MBEDTLS_SSL_SERVER_NAME_INDICATION          CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION
#endif
#undef MBEDTLS_SSL_TRUNCATED_HMAC
#if defined(CONFIG_MBEDTLS_SSL_TRUNCATED_HMAC)
#define MBEDTLS_SSL_TRUNCATED_HMAC                  CONFIG_MBEDTLS_SSL_TRUNCATED_HMAC
#endif
#undef MBEDTLS_THREADING_ALT
#if defined(CONFIG_MBEDTLS_THREADING_ALT)
#define MBEDTLS_THREADING_ALT                       CONFIG_MBEDTLS_THREADING_ALT
#endif
#undef MBEDTLS_AES_C
#if defined(CONFIG_MBEDTLS_AES)
#define MBEDTLS_AES_C                               CONFIG_MBEDTLS_AES
#endif
#undef MBEDTLS_ASN1_PARSE_C
#if defined(CONFIG_MBEDTLS_ASN1_PARSE)
#define MBEDTLS_ASN1_PARSE_C                        CONFIG_MBEDTLS_ASN1_PARSE
#endif
#undef MBEDTLS_ASN1_WRITE_C
#if defined(CONFIG_MBEDTLS_ASN1_WRITE)
#define MBEDTLS_ASN1_WRITE_C                        CONFIG_MBEDTLS_ASN1_WRITE
#endif
#undef MBEDTLS_BASE64_C
#if defined(CONFIG_MBEDTLS_BASE64)
#define MBEDTLS_BASE64_C                            CONFIG_MBEDTLS_BASE64
#endif
#undef MBEDTLS_BIGNUM_C
#if defined(CONFIG_MBEDTLS_BIGNUM)
#define MBEDTLS_BIGNUM_C                            CONFIG_MBEDTLS_BIGNUM
#endif
#undef MBEDTLS_CAMELLIA_C
#if defined(CONFIG_MBEDTLS_CAMELLIA)
#define MBEDTLS_CAMELLIA_C                          CONFIG_MBEDTLS_CAMELLIA
#endif
#undef MBEDTLS_ARIA
#if defined(CONFIG_MBEDTLS_ARIA)
#define MBEDTLS_ARIA_C                              CONFIG_MBEDTLS_ARIA
#endif
#undef MBEDTLS_CCM_C
#if defined(CONFIG_MBEDTLS_CCM)
#define MBEDTLS_CCM_C                               CONFIG_MBEDTLS_CCM
#endif
#undef MBEDTLS_CIPHER_C
#if defined(CONFIG_MBEDTLS_CIPHER)
#define MBEDTLS_CIPHER_C                            CONFIG_MBEDTLS_CIPHER
#endif
#undef MBEDTLS_CMAC_C
#if defined(CONFIG_MBEDTLS_CMAC)
#define MBEDTLS_CMAC_C                              CONFIG_MBEDTLS_CMAC
#endif
#undef MBEDTLS_CTR_DRBG_C
#if defined(CONFIG_MBEDTLS_CTR_DRBG)
#define MBEDTLS_CTR_DRBG_C                          CONFIG_MBEDTLS_CTR_DRBG
#endif
#undef MBEDTLS_DES_C
#if defined(CONFIG_MBEDTLS_DES)
#define MBEDTLS_DES_C                               CONFIG_MBEDTLS_DES
#endif
#undef MBEDTLS_DHM_C
#if defined(CONFIG_MBEDTLS_DHM)
#define MBEDTLS_DHM_C                               CONFIG_MBEDTLS_DHM
#endif
#undef MBEDTLS_ECDH_C
#if defined(CONFIG_MBEDTLS_ECDH)
#define MBEDTLS_ECDH_C                              CONFIG_MBEDTLS_ECDH
#endif
#undef MBEDTLS_ECDSA_C
#if defined(CONFIG_MBEDTLS_ECDSA)
#define MBEDTLS_ECDSA_C                             CONFIG_MBEDTLS_ECDSA
#endif
#undef MBEDTLS_ECJPAKE_C
#if defined(CONFIG_MBEDTLS_ECJPAKE)
#define MBEDTLS_ECJPAKE_C                           CONFIG_MBEDTLS_ECJPAKE
#endif
#undef MBEDTLS_ECP_C
#if defined(CONFIG_MBEDTLS_ECP)
#define MBEDTLS_ECP_C                               CONFIG_MBEDTLS_ECP
#endif
#undef MBEDTLS_ENTROPY_C
#if defined(CONFIG_MBEDTLS_ENTROPY)
#define MBEDTLS_ENTROPY_C                           CONFIG_MBEDTLS_ENTROPY
#endif
#undef MBEDTLS_GCM_C
#if defined(CONFIG_MBEDTLS_GCM)
#define MBEDTLS_GCM_C                               CONFIG_MBEDTLS_GCM
#endif
#undef MBEDTLS_HKDF_C
#if defined(CONFIG_MBEDTLS_HKDF)
#define MBEDTLS_HKDF_C                              CONFIG_MBEDTLS_HKDF
#endif
#undef MBEDTLS_HMAC_DRBG_C
#if defined(CONFIG_MBEDTLS_HMAC_DRBG)
#define MBEDTLS_HMAC_DRBG_C                         CONFIG_MBEDTLS_HMAC_DRBG
#endif
#undef MBEDTLS_NIST_KW_C
#if defined(CONFIG_MBEDTLS_NIST_KW)
#define MBEDTLS_NIST_KW_C                           CONFIG_MBEDTLS_NIST_KW
#endif
#undef MBEDTLS_MD_C
#if defined(CONFIG_MBEDTLS_MD)
#define MBEDTLS_MD_C                                CONFIG_MBEDTLS_MD
#endif
#undef MBEDTLS_MD2_C
#if defined(CONFIG_MBEDTLS_MD2)
#define MBEDTLS_MD2_C                               CONFIG_MBEDTLS_MD2
#endif
#undef MBEDTLS_MD4_C
#if defined(CONFIG_MBEDTLS_MD4)
#define MBEDTLS_MD4_C                               CONFIG_MBEDTLS_MD4
#endif
#undef MBEDTLS_MD5_C
#if defined(CONFIG_MBEDTLS_MD5)
#define MBEDTLS_MD5_C                               CONFIG_MBEDTLS_MD5
#endif
#undef MBEDTLS_OID_C
#if defined(CONFIG_MBEDTLS_OID)
#define MBEDTLS_OID_C                               CONFIG_MBEDTLS_OID
#endif
#undef MBEDTLS_PEM_PARSE_C
#if defined(CONFIG_MBEDTLS_PEM_PARSE)
#define MBEDTLS_PEM_PARSE_C                         CONFIG_MBEDTLS_PEM_PARSE
#endif
#undef MBEDTLS_PEM_WRITE_C
#if defined(CONFIG_MBEDTLS_PEM_WRITE)
#define MBEDTLS_PEM_WRITE_C                         CONFIG_MBEDTLS_PEM_WRITE
#endif
#undef MBEDTLS_PK_C
#if defined(CONFIG_MBEDTLS_PK)
#define MBEDTLS_PK_C                                CONFIG_MBEDTLS_PK
#endif
#undef MBEDTLS_PK_PARSE_C
#if defined(CONFIG_MBEDTLS_PK_PARSE)
#define MBEDTLS_PK_PARSE_C                          CONFIG_MBEDTLS_PK_PARSE
#endif
#undef MBEDTLS_PK_WRITE_C
#if defined(CONFIG_MBEDTLS_PK_WRITE)
#define MBEDTLS_PK_WRITE_C                          CONFIG_MBEDTLS_PK_WRITE
#endif
#undef MBEDTLS_PKCS5_C
#if defined(CONFIG_MBEDTLS_PKCS5)
#define MBEDTLS_PKCS5_C                             CONFIG_MBEDTLS_PKCS5
#endif
#undef MBEDTLS_PKCS11_C
#if defined(CONFIG_MBEDTLS_PKCS11)
#define MBEDTLS_PKCS11_C                            CONFIG_MBEDTLS_PKCS11
#endif
#undef MBEDTLS_PKCS12_C
#if defined(CONFIG_MBEDTLS_PKCS12)
#define MBEDTLS_PKCS12_C                            CONFIG_MBEDTLS_PKCS12
#endif
#undef MBEDTLS_RSA_C
#if defined(CONFIG_MBEDTLS_RSA)
#define MBEDTLS_RSA_C                               CONFIG_MBEDTLS_RSA
#endif
#undef MBEDTLS_SHA1_C
#if defined(CONFIG_MBEDTLS_SHA1)
#define MBEDTLS_SHA1_C                              CONFIG_MBEDTLS_SHA1
#endif
#undef MBEDTLS_SHA256_C
#if defined(CONFIG_MBEDTLS_SHA256)
#define MBEDTLS_SHA256_C                            CONFIG_MBEDTLS_SHA256
#endif
#undef MBEDTLS_SHA512_C
#if defined(CONFIG_MBEDTLS_SHA512)
#define MBEDTLS_SHA512_C                            CONFIG_MBEDTLS_SHA512
#endif
#undef MBEDTLS_SSL_TICKET_C
#if defined(CONFIG_MBEDTLS_SSL_TICKET)
#define MBEDTLS_SSL_TICKET_C                        CONFIG_MBEDTLS_SSL_TICKET
#endif
#undef MBEDTLS_SSL_CLI_C
#if defined(CONFIG_MBEDTLS_SSL_CLI)
#define MBEDTLS_SSL_CLI_C                           CONFIG_MBEDTLS_SSL_CLI
#endif
#undef MBEDTLS_SSL_SRV_C
#if defined(CONFIG_MBEDTLS_SSL_SRV)
#define MBEDTLS_SSL_SRV_C                           CONFIG_MBEDTLS_SSL_SRV
#endif
#undef MBEDTLS_SSL_TLS_C
#if defined(CONFIG_MBEDTLS_SSL_TLS)
#define MBEDTLS_SSL_TLS_C                           CONFIG_MBEDTLS_SSL_TLS
#endif
#undef MBEDTLS_THREADING_C
#if defined(CONFIG_MBEDTLS_THREADING)
#define MBEDTLS_THREADING_C                         CONFIG_MBEDTLS_THREADING
#endif
#undef MBEDTLS_X509_USE_C
#if defined(CONFIG_MBEDTLS_X509_USE)
#define MBEDTLS_X509_USE_C                          CONFIG_MBEDTLS_X509_USE
#endif
#undef MBEDTLS_X509_CRT_PARSE_C
#if defined(CONFIG_MBEDTLS_X509_CRT_PARSE)
#define MBEDTLS_X509_CRT_PARSE_C                    CONFIG_MBEDTLS_X509_CRT_PARSE
#endif
#undef MBEDTLS_X509_CRL_PARSE_C
#if defined(CONFIG_MBEDTLS_X509_CRL_PARSE)
#define MBEDTLS_X509_CRL_PARSE_C                    CONFIG_MBEDTLS_X509_CRL_PARSE
#endif
#undef MBEDTLS_X509_CSR_PARSE_C
#if defined(CONFIG_MBEDTLS_X509_CSR_PARSE)
#define MBEDTLS_X509_CSR_PARSE_C                    CONFIG_MBEDTLS_X509_CSR_PARSE
#endif
#undef MBEDTLS_X509_CREATE_C
#if defined(CONFIG_MBEDTLS_X509_CREATE)
#define MBEDTLS_X509_CREATE_C                       CONFIG_MBEDTLS_X509_CREATE
#endif
#undef MBEDTLS_X509_CRT_WRITE_C
#if defined(CONFIG_MBEDTLS_X509_CRT_WRITE)
#define MBEDTLS_X509_CRT_WRITE_C                    CONFIG_MBEDTLS_X509_CRT_WRITE
#endif
#undef MBEDTLS_X509_CSR_WRITE_C
#if defined(CONFIG_MBEDTLS_X509_CSR_WRITE)
#define MBEDTLS_X509_CSR_WRITE_C                    CONFIG_MBEDTLS_X509_CSR_WRITE
#endif
/** @endcond */

#include "mbedtls/check_config.h"

#ifdef __cplusplus
}
#endif

#endif /* RIOT_MBEDTLS_CONFIG_H */
/** @} */
