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

#if !IS_ACTIVE(CONFIG_KCONFIG_USEPKG_MBEDTLS) || defined(DOXYGEN)

/**
 *
 * @brief Enable the checkup functions (*_self_test).
 */
#ifndef CONFIG_MBEDTLS_SELF_TEST
#define CONFIG_MBEDTLS_SELF_TEST 1
#endif

/**
 *
 * @brief Enable the platform-specific entropy code.
 *
 * Module:  mbedtls/library/entropy.c
 * Caller:
 *
 * Requires: MBEDTLS_SHA512_C or MBEDTLS_SHA256_C
 *
 * This module provides a generic entropy pool
 */
#ifndef CONFIG_MBEDTLS_ENTROPY_C
#define CONFIG_MBEDTLS_ENTROPY_C 1
#endif

/**
 *
 * @brief Enable the SHA-224 and SHA-256 cryptographic hash algorithms.
 *
 * Module:  mbedtls/library/sha256.c
 * Caller:  mbedtls/library/entropy.c
 *          mbedtls/library/md.c
 *          mbedtls/library/ssl_cli.c
 *          mbedtls/library/ssl_srv.c
 *          mbedtls/library/ssl_tls.c
 *
 * This module adds support for SHA-224 and SHA-256.
 * This module is required for the SSL/TLS 1.2 PRF function.
 */
#ifndef CONFIG_MBEDTLS_SHA256_C
#define CONFIG_MBEDTLS_SHA256_C 1
#endif

/**
 *
 * @brief MBEDTLS__MODULE_NAME__ALT: Enable a macro to let mbed TLS use your
 * alternate core implementation of a symmetric crypto, an arithmetic or hash
 * module (e.g. platform specific assembly optimized implementations). Keep
 * in mind that the function prototypes should remain the same.
 *
 * This replaces the whole module. If you only want to replace one of the
 * functions, use one of the MBEDTLS__FUNCTION_NAME__ALT flags.
 *
 * Example: In case you enable MBEDTLS_SHA256_ALT, mbed TLS will no longer
 * provide the "struct mbedtls_sha256_context" definition and omit the base
 * function declarations and implementations. "sha256_alt.h" will be included from
 * "sha256.h" to include the new function definitions.
 *
 * Enable a macro to enable alternate implementation of the corresponding
 * module.
 *
 * @warning   MD2, MD4, MD5, ARC4, DES and SHA-1 are considered weak and their
 *            use constitutes a security risk. If possible, we recommend
 *            avoiding dependencies on them, and considering stronger message
 *            digests and ciphers instead.
 *
 */
#ifndef CONFIG_MBEDTLS_SHA256_ALT
#define CONFIG_MBEDTLS_SHA256_ALT 1
#endif

/**
 *
 * @brief Enable the threading abstraction layer. By default mbed TLS
 * assumes it is used in a non-threaded environment or that contexts
 * are not shared between threads. If you do intend to use contexts
 * between threads, you will need to enable this layer to prevent race
 * conditions. See also our Knowledge Base article about threading:
 * https://tls.mbed.org/kb/development/thread-safety-and-multi-threading
 *
 * This allows different threading implementations (self-implemented or
 * provided).
 *
 * You will have to enable either MBEDTLS_THREADING_ALT or
 * MBEDTLS_THREADING_PTHREAD.
 *
 * Enable this layer to allow use of mutexes within mbed TLS
 *
 * @note In RIOT, we enable this layer by default and utilize
 *       MBEDTLS_THREADING_ALT with RIOT mutexes.
 */
#ifndef CONFIG_MBEDTLS_THREADING_C
#define CONFIG_MBEDTLS_THREADING_C 1
#endif

/**
 * @brief Provide your own alternate threading implementation.
 *
 * Requires: MBEDTLS_THREADING_C
 *
 * This to allows your own alternate threading implementation.
 */
#if !defined (CONFIG_MBEDTLS_THREADING_ALT) && defined (CONFIG_MBEDTLS_THREADING_C)
#define CONFIG_MBEDTLS_THREADING_ALT 1
#endif

/**
 *
 * @brief Enable the generic SSL/TLS code.
 */
#ifndef CONFIG_MBEDTLS_SSL_TLS_C
#define CONFIG_MBEDTLS_SSL_TLS_C 1
#endif

/**
 *
 * @brief Enable the generic cipher layer.
 */
#ifndef CONFIG_MBEDTLS_CIPHER_C
#define CONFIG_MBEDTLS_CIPHER_C 1
#endif

/**
 *
 * @brief  Enable support for RFC 6066 server name indication (SNI) in SSL.
 */
#ifndef CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION
#define CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION 1
#endif

/**
 *
 * @brief  Enable the generic message digest layer.
 */
#ifndef CONFIG_MBEDTLS_MD_C
#define CONFIG_MBEDTLS_MD_C 1
#endif

/**
 *
 * @brief  Enable support for TLS 1.2 (and DTLS 1.2 if DTLS is enabled).
 */
#ifndef CONFIG_MBEDTLS_SSL_PROTO_TLS1_2
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_2 1
#endif

/**
 *
 * @brief  Enable the ECDHE-ECDSA based ciphersuite modes in SSL / TLS.
 */
#ifndef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED 1
#endif

/**
 *
 * @brief  Enable the ECDHE-RSA based ciphersuite modes in SSL / TLS.
 */
#ifndef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define CONFIGMBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED 1
#endif

/**
 *
 * @brief  Enable the RSA public-key cryptosystem.
 */
#ifndef CONFIG_MBEDTLS_RSA_C
#define CONFIG_MBEDTLS_RSA_C 1
#endif

/**
 *
 * @brief  Enable X.509 certificate parsing.
 */
#ifndef CONFIG_MBEDTLS_X509_CRT_PARSE_C
#define CONFIG_MBEDTLS_X509_CRT_PARSE_C 1
#endif

/**
 *
 * @brief  Enable X.509 core for using certificates.
 */
#ifndef CONFIG_MBEDTLS_X509_USE_C
#define CONFIG_MBEDTLS_X509_USE_C 1
#endif

/**
 *
 * @brief  Enable PEM decoding / parsing.
 */
#ifndef CONFIG_MBEDTLS_PEM_PARSE_C
#define CONFIG_MBEDTLS_PEM_PARSE_C 1
#endif

/**
 *
 * @brief  Enable the Base64 module.
 */
#ifndef CONFIG_MBEDTLS_BASE64_C
#define CONFIG_MBEDTLS_BASE64_C 1
#endif

/**
 *
 * @brief  Enable support for PKCS#1 v1.5 encoding.
 */
#ifndef CONFIG_MBEDTLS_PKCS1_V15
#define CONFIG_MBEDTLS_PKCS1_V15 1
#endif

/**
 *
 * @brief  Enable the multi-precision integer library.
 */
#ifndef CONFIG_MBEDTLS_BIGNUM_C
#define CONFIG_MBEDTLS_BIGNUM_C 1
#endif

/**
 *
 * @brief  Enable the OID database.
 */
#ifndef CONFIG_MBEDTLS_OID_C
#define CONFIG_MBEDTLS_OID_C 1
#endif

/**
 *
 * @brief  Enable the generic ASN1 parser.
 */
#ifndef CONFIG_MBEDTLS_ASN1_PARSE_C
#define CONFIG_MBEDTLS_ASN1_PARSE_C 1
#endif

/**
 *
 * @brief  Enable the generic public (asymmetric) key parser.
 */
#ifndef CONFIG_MBEDTLS_PK_PARSE_C
#define CONFIG_MBEDTLS_PK_PARSE_C 1
#endif

/**
 *
 * @brief   Enable the generic public (asymmetric) key layer.
 */
#ifndef CONFIG_MBEDTLS_PK_C
#define CONFIG_MBEDTLS_PK_C 1
#endif

/**
 *
 * @brief   Enable the SSL/TLS client code.
 */
#ifndef CONFIG_MBEDTLS_SSL_CLI_C
#define CONFIG_MBEDTLS_SSL_CLI_C 1
#endif

/**
 *
 * @brief   Enable the AES block cipher.
 */
#ifndef CONFIG_MBEDTLS_AES_C
#define CONFIG_MBEDTLS_AES_C 1
#endif

/**
 *
 * @brief   Enable the TCP and UDP over IPv6/IPv4 networking routines.
 */
#ifndef CONFIG_MBEDTLS_NET_C
#define CONFIG_MBEDTLS_NET_C 1
#endif

/**
 *
 * @brief   Enable the CTR_DRBG AES-based random generator.
 */
#ifndef CONFIG_MBEDTLS_CTR_DRBG_C
#define CONFIG_MBEDTLS_CTR_DRBG_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve Diffie-Hellman library.
 */
#ifndef CONFIG_MBEDTLS_ECDH_C
#define CONFIG_MBEDTLS_ECDH_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve DSA library.
 */
#ifndef CONFIG_MBEDTLS_ECDSA_C
#define CONFIG_MBEDTLS_ECDSA_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve over GF(p) library.
 */
#ifndef CONFIG_MBEDTLS_ECP_C
#define CONFIG_MBEDTLS_ECP_C 1
#endif

/**
 *
 * @brief   EMBEDTLS_ECP_XXXX_ENABLED: Enables specific curves within the Elliptic Curve
 * module.
 */
#ifndef CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED 1
#endif

/**
 *
 * @brief   Enable the generic ASN1 writer.
 */
#ifndef CONFIG_MBEDTLS_ASN1_WRITE_C
#define CONFIG_MBEDTLS_ASN1_WRITE_C 1
#endif

/**
 *
 * @brief   Enable the Diffie-Hellman-Merkle module.
 */
#ifndef CONFIG_MBEDTLS_DHM_C
#define CONFIG_MBEDTLS_DHM_C 1
#endif

/**
 *
 * @brief   Enable the Galois/Counter Mode (GCM).
 */
#ifndef CONFIG_MBEDTLS_GCM_C
#define CONFIG_MBEDTLS_GCM_C 1
#endif

/**
 *
 * @brief   Enable the platform abstraction layer
 */
#ifndef CONFIG_MBEDTLS_PLATFORM_C
#define CONFIG_MBEDTLS_PLATFORM_C 1
#endif

/**
 *
 * @brief   Enable the debug functions.
 */
#ifndef CONFIG_MBEDTLS_DEBUG_C
#define CONFIG_MBEDTLS_DEBUG_C 1
#endif


#endif /* !CONFIG_KCONFIG_USEPKG_MBEDTLS || DOXYGEN */

#if !IS_ACTIVE(CONFIG_KCONFIG_USEMODULE_MBEDTLS_ENTROPY) || defined(DOXYGEN)

/**
 *
 * @brief Enable this macro to let mbed TLS use your own implementation of a
 * hardware entropy collector.
 *
 * Your function must be called mbedtls_hardware_poll(), have the same
 * prototype as declared in entropy_poll.h, and accept NULL as first argument.
 *
 * Enable to use your own hardware entropy collector.
 */
#ifndef CONFIG_MBEDTLS_ENTROPY_HARDWARE_ALT
#define CONFIG_MBEDTLS_ENTROPY_HARDWARE_ALT 1
#endif

/**
 *
 * @brief Do not use built-in platform entropy functions.
 * This is useful if your platform does not support
 * standards like the /dev/urandom or Windows CryptoAPI.
 *
 * Disable the built-in platform entropy functions.
 */
#ifndef CONFIG_MBEDTLS_NO_PLATFORM_ENTROPY
#define CONFIG_MBEDTLS_NO_PLATFORM_ENTROPY 1
#endif

/**
 *
 * @brief Force the entropy accumulator to use a SHA-256 accumulator instead of the
 * default SHA-512 based one (if both are available).
 *
 * Requires: MBEDTLS_SHA256_C
 *
 * On 32-bit systems SHA-256 can be much faster than SHA-512. Use this option
 * if you have performance concerns.
 *
 * This option is only useful if both MBEDTLS_SHA256_C and
 * MBEDTLS_SHA512_C are defined. Otherwise the available hash module is used.
 */
#ifndef CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256
#define CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256 1
#endif

#endif /* !CONFIG_KCONFIG_USEMODULE_MBEDTLS_ENTROPY || DOXYGEN */

/**
 * @cond
 * This translates RIOT exposed options to Mbed TLS macros, it is hidden from Doxygen.
 */
#if CONFIG_MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_ALT 1
#endif
#if CONFIG_MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_ENTROPY_HARDWARE_ALT 1
#endif
#if CONFIG_MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_PLATFORM_ENTROPY 1
#endif
#if CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_ENTROPY_FORCE_SHA256 1
#endif
#if CONFIG_MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_C 1
#endif
#if CONFIG_MBEDTLS_THREADING_ALT
#define MBEDTLS_THREADING_ALT 1
#endif
#if CONFIG_MBEDTLS_SELF_TEST
#define MBEDTLS_SELF_TEST 1
#endif
#if CONFIG_MBEDTLS_ENTROPY_C
#define MBEDTLS_ENTROPY_C 1
#endif
#if CONFIG_MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_C 1
#endif

/**
 *
 * @brief Enable the generic SSL/TLS code.
 *
 * Module:  library/ssl_tls.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *
 * Requires: MBEDTLS_CIPHER_C, MBEDTLS_MD_C
 *           and at least one of the MBEDTLS_SSL_PROTO_XXX defines
 *
 * This module is required for SSL/TLS.
 */
#if CONFIG_MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_TLS_C 1
#endif

/**
 *
 * @brief Enable the generic cipher layer.
 *
 * Module:  library/cipher.c
 * Caller:  library/ssl_tls.c
 *
 * Uncomment to enable generic cipher wrappers.
 */
#if CONFIG_MBEDTLS_CIPHER_C
#define MBEDTLS_CIPHER_C 1
#endif

/**
 *
 * @brief  Enable support for RFC 6066 server name indication (SNI) in SSL.
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 *
 * Comment this macro to disable support for server name indication in SSL
 */
#if CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_SERVER_NAME_INDICATION 1
#endif

/**
 *
 * @brief  Enable the generic message digest layer.
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 *
 * Comment this macro to disable support for server name indication in SSL
 */
#if CONFIG_MBEDTLS_MD_C
#define MBEDTLS_MD_C 1
#endif

/**
 *
 * @brief  Enable support for TLS 1.2 (and DTLS 1.2 if DTLS is enabled).
 *
 * Requires: MBEDTLS_SHA1_C or MBEDTLS_SHA256_C or MBEDTLS_SHA512_C
 *           (Depends on ciphersuites)
 *
 * Comment this macro to disable support for TLS 1.2 / DTLS 1.2
 */
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_TLS1_2 1
#endif

/**
 *
 * @brief  Enable the ECDHE-ECDSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_ECDSA_C, MBEDTLS_X509_CRT_PARSE_C
 */
#if CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED 1
#endif

/**
 *
 * @brief  Enable the ECDHE-RSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15,
 *           MBEDTLS_X509_CRT_PARSE_C
 */
#if CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED 1
#endif

/**
 *
 * @brief  Enable the RSA public-key cryptosystem.
 *
 * Module:  library/rsa.c
 *          library/rsa_internal.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509.c
 *
 * This module is used by the following key exchanges:
 *      RSA, DHE-RSA, ECDHE-RSA, RSA-PSK
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_OID_C
 */
#if CONFIG_MBEDTLS_RSA_C
#define MBEDTLS_RSA_C 1
#endif

/**
 *
 * @brief  Enable X.509 certificate parsing.
 *
 * Module:  library/x509_crt.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is required for X.509 certificate parsing.
 */
#if CONFIG_MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRT_PARSE_C 1
#endif

/**
 *
 * @brief  Enable X.509 core for using certificates.
 *
 * Module:  library/x509.c
 * Caller:  library/x509_crl.c
 *          library/x509_crt.c
 *          library/x509_csr.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_BIGNUM_C, MBEDTLS_OID_C,
 *           MBEDTLS_PK_PARSE_C
 *
 * This module is required for the X.509 parsing modules.
 */
#if CONFIG_MBEDTLS_X509_USE_C
#define MBEDTLS_X509_USE_C 1
#endif

/**
 *
 * @brief  Enable PEM decoding / parsing.
 *
 * Module:  library/pem.c
 * Caller:  library/dhm.c
 *          library/pkparse.c
 *          library/x509_crl.c
 *          library/x509_crt.c
 *          library/x509_csr.c
 *
 * Requires: MBEDTLS_BASE64_C
 *
 * This modules adds support for decoding / parsing PEM files.
 */
#if CONFIG_MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_PARSE_C 1
#endif
/**
 *
 * @brief  Enable the Base64 module.
 *
 * Module:  library/base64.c
 * Caller:  library/pem.c
 *
 * This module is required for PEM support (required by X.509).
 */
#if CONFIG_MBEDTLS_BASE64_C
#define MBEDTLS_BASE64_C 1
#endif

/**
 *
 * @brief  Enable support for PKCS#1 v1.5 encoding.
 *
 * Requires: MBEDTLS_RSA_C
 *
 * This enables support for PKCS#1 v1.5 operations.
 */
#if CONFIG_MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V15 1
#endif

/**
 *
 * @brief  Enable the multi-precision integer library.
 *
 * Module:  library/bignum.c
 * Caller:  library/dhm.c
 *          library/ecp.c
 *          library/ecdsa.c
 *          library/rsa.c
 *          library/rsa_internal.c
 *          library/ssl_tls.c
 *
 * This module is required for RSA, DHM and ECC (ECDH, ECDSA) support.
 */
#if CONFIG_MBEDTLS_BIGNUM_C
#define MBEDTLS_BIGNUM_C 1
#endif

/**
 *
 * @brief  Enable the OID database.
 *
 * Module:  library/oid.c
 * Caller:  library/asn1write.c
 *          library/pkcs5.c
 *          library/pkparse.c
 *          library/pkwrite.c
 *          library/rsa.c
 *          library/x509.c
 *          library/x509_create.c
 *          library/x509_crl.c
 *          library/x509_crt.c
 *          library/x509_csr.c
 *          library/x509write_crt.c
 *          library/x509write_csr.c
 *
 * This modules translates between OIDs and internal values.
 */
#if CONFIG_MBEDTLS_OID_C
#define MBEDTLS_OID_C 1
#endif

/**
 *
 * @brief  Enable the generic ASN1 parser.
 *
 * Module:  library/asn1.c
 * Caller:  library/x509.c
 *          library/dhm.c
 *          library/pkcs12.c
 *          library/pkcs5.c
 *          library/pkparse.c
 */
#if CONFIG_MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_PARSE_C 1
#endif

/**
 *
 * @brief  Enable the generic public (asymmetric) key parser.
 *
 * Module:  library/pkparse.c
 * Caller:  library/x509_crt.c
 *          library/x509_csr.c
 *
 * Requires: MBEDTLS_PK_C
 *
 * Uncomment to enable generic public key parse functions.
 */
#if CONFIG_MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_PARSE_C 1
#endif

/**
 *
 * @brief   Enable the generic public (asymmetric) key layer.
 *
 * Module:  library/pk.c
 * Caller:  library/ssl_tls.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *
 * Requires: MBEDTLS_RSA_C or MBEDTLS_ECP_C
 *
 * Uncomment to enable generic public key wrappers.
 */
#if CONFIG_MBEDTLS_PK_C
#define MBEDTLS_PK_C 1
#endif

/**
 *
 * @brief   Enable the SSL/TLS client code.
 *
 * Module:  library/ssl_cli.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *
 * This module is required for SSL/TLS client support.
 */
#if CONFIG_MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_CLI_C 1
#endif

/**
 *
 * @brief   Enable the AES block cipher.
 *
 * Module:  library/aes.c
 * Caller:  library/cipher.c
 *          library/pem.c
 *          library/ctr_drbg.c
 */
#if CONFIG_MBEDTLS_AES_C
#define MBEDTLS_AES_C 1
#endif

/**
 *
 * @brief   Enable the TCP and UDP over IPv6/IPv4 networking routines.
 *
 * Module:  library/net_sockets.c
 *
 * This module provides networking routines
 */
#if CONFIG_MBEDTLS_NET_C
#define MBEDTLS_NET_C 1
#endif

/**
 *
 * @brief   Enable the CTR_DRBG AES-based random generator.
 *
 * Module:  library/ctr_drbg.c
 * Caller:
 *
 * Requires: MBEDTLS_AES_C
 *
 * This module provides the CTR_DRBG AES random number generator.
 */
#if CONFIG_MBEDTLS_CTR_DRBG_C
#define MBEDTLS_CTR_DRBG_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve Diffie-Hellman library.
 *
 * Module:  library/ecdh.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *
 * This module is used by the following key exchanges:
 *      ECDHE-ECDSA, ECDHE-RSA, DHE-PSK
 *
 * Requires: MBEDTLS_ECP_C
 */
#if CONFIG_MBEDTLS_ECDH_C
#define MBEDTLS_ECDH_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve DSA library.
 *
 * Module:  library/ecdsa.c
 * Caller:
 *
 * This module is used by the following key exchanges:
 *      ECDHE-ECDSA
 *
 * Requires: MBEDTLS_ECP_C, MBEDTLS_ASN1_WRITE_C, MBEDTLS_ASN1_PARSE_C,
 *           and at least one MBEDTLS_ECP_DP_XXX_ENABLED for a
 *           short Weierstrass curve.
 */
#if CONFIG_MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_C 1
#endif

/**
 *
 * @brief   Enable the elliptic curve over GF(p) library.
 *
 * Module:  library/ecp.c
 * Caller:  library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_BIGNUM_C and at least one MBEDTLS_ECP_DP_XXX_ENABLED
 */
#if CONFIG_MBEDTLS_ECP_C
#define MBEDTLS_ECP_C 1
#endif

/**
 *
 * @brief   EMBEDTLS_ECP_XXXX_ENABLED: Enables specific curves within the Elliptic Curve
 * module.  By default all supported curves are enabled.
 *
 */
#if CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED 1
#endif

/**
 *
 * @brief   Enable the generic ASN1 writer.
 *
 * Module:  library/asn1write.c
 * Caller:  library/ecdsa.c
 *          library/pkwrite.c
 *          library/x509_create.c
 *          library/x509write_crt.c
 *          library/x509write_csr.c
 */
#if CONFIG_MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_ASN1_WRITE_C 1
#endif

/**
 *
 * @brief   Enable the Diffie-Hellman-Merkle module.
 *
 * Module:  library/dhm.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *
 * This module is used by the following key exchanges:
 *      DHE-RSA, DHE-PSK
 */
#if CONFIG_MBEDTLS_DHM_C
#define MBEDTLS_DHM_C 1
#endif

/**
 *
 * @brief   Enable the Galois/Counter Mode (GCM).
 *
 * Module:  library/gcm.c
 *
 * Requires: MBEDTLS_AES_C or MBEDTLS_CAMELLIA_C or MBEDTLS_ARIA_C
 *
 * This module enables the AES-GCM and CAMELLIA-GCM ciphersuites, if other
 * requisites are enabled as well.
 */
#if CONFIG_MBEDTLS_GCM_C
#define MBEDTLS_GCM_C 1
#endif

/**
 *
 * @brief   Enable the platform abstraction layer that allows you to re-assign
 * functions like calloc(), free(), snprintf(), printf(), fprintf(), exit().
 *
 * Enabling MBEDTLS_PLATFORM_C enables to use of MBEDTLS_PLATFORM_XXX_ALT
 * or MBEDTLS_PLATFORM_XXX_MACRO directives, allowing the functions mentioned
 * above to be specified at runtime or compile time respectively.
 *
 * \note This abstraction layer must be enabled on Windows (including MSYS2)
 * as other module rely on it for a fixed snprintf implementation.
 *
 * Module:  library/platform.c
 * Caller:  Most other .c files
 *
 * This module enables abstraction of common (libc) functions.
 */
#if CONFIG_MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_C 1
#endif

/**
 *
 * @brief   Enable the debug functions.
 *
 * Module:  library/debug.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * This module provides debugging functions.
 */
#if CONFIG_MBEDTLS_DEBUG_C
#define MBEDTLS_DEBUG_C 1
#endif
/** @endcond */

// #endif
#include "mbedtls/check_config.h"

#ifdef __cplusplus
}
#endif

#endif /* RIOT_MBEDTLS_CONFIG_H */
/** @} */
