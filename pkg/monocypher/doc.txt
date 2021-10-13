/**
 * @defgroup pkg_monocypher Monocypher cryptographic library
 * @ingroup  pkg
 * @brief    Provides the Monocypher cryptographic library.
 *
 * # Monocypher RIOT package
 *
 * Monocypher is a high performance cryptographic library. It provides functions
 * for authenticated encryption, hashing, password key derivation, key
 * exchange, and public key signatures.
 *
 * You can find the API and more information
 * [here](https://monocypher.org/manual)
 *
 * ## Requirements
 *
 * @note    Monocypher only supports 32bit platforms.
 *
 * Monocypher requires around 4K of stack space depending slightly on the
 * platform, so beware that you're allocating at
 * least `THREAD_STACKSIZE_DEFAULT + 4096` bytes.
 *
 * You can do it easily by adding:
 *
 * ```makefile
 * CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT + 4096)'
 * ```
 *
 * to your makefile.
 *
 * ## Usage
 *
 * Just add it as a package in your application:
 *
 * ```makefile
 * USEPKG += monocypher
 * ```
 *
 * @see     https://monocypher.org/
 */
