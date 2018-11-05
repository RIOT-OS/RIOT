/**
 * @defgroup pkg_libhydrogen LibHydrogen cryptographic library
 * @ingroup  pkg
 * @brief    A lightweight, secure, easy-to-use crypto library suitable for constrained environments.
 *
 * # LibHydrogen RIOT package
 *
 * The Hydrogen library is a small, easy-to-use, hard-to-misuse cryptographic
 * library. It provides functions for random numbers, generic hashing, key
 * derivation, secret-key encryption, public-key signatures, key exchange and
 * password hashing.
 *
 * Full documentation can be found on the [LibHydrogen wiki](https://github.com/jedisct1/libhydrogen/wiki).
 *
 * ## Usage
 *
 * Add it as a package in your application's Makefile:
 *
 * ```makefile
 * USEPKG += libhydrogen
 * ```
 *
 * Include the LibHydrogen header in your code:
 *
 * ```c
 * #include "hydrogen.h"
 * ```
 *
 * @see https://github.com/jedisct1/libhydrogen
 */
