/**
 * @defgroup pkg_libb2 BLAKE2 hashing library
 * @ingroup  pkg
 * @brief    C library providing BLAKE2b, BLAKE2s, BLAKE2bp, BLAKE2sp

 *
 * # BLAKE2 RIOT package
 *
 * BLAKE2 is a cryptographic hash function specified in RFC 7693.
 * It claims to be faster than MD5, SHA-1, SHA-2, and SHA-3, yet at least as
 * secure as the latest standard SHA-3.
 *
 * BLAKE2 comes in two flavors:
 *
 * - BLAKE2b (or just BLAKE2) is optimized for 64-bit platforms—including NEON-enabled ARMs—and
 *   produces digests of any size between 1 and 64 bytes
 * - BLAKE2s is optimized for 8- to 32-bit platforms and produces digests of
 *   any size between 1 and 32 bytes
 *
 * For more information, see the [BLAKE2 website](https://blake2.net/).
 *
 * ## Usage
 *
 * Add it as a package in your application's Makefile:
 *
 * ```makefile
 * USEPKG += libb2
 * ```
 *
 * Include the BLAKE2 header in your code:
 *
 * ```c
 * #include "blake2.h"
 * ```
 *
 * @see https://github.com/BLAKE2/libb2
 */
