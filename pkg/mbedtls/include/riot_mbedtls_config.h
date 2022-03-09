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
/** @endcond */

#include "mbedtls/check_config.h"

#ifdef __cplusplus
}
#endif

#endif /* RIOT_MBEDTLS_CONFIG_H */
/** @} */
