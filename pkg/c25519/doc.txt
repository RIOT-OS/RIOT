/**
 * @defgroup pkg_c25519 C25519 cryptographic library
 * @ingroup  pkg
 * @ingroup  sys
 * @brief    Curve25519 and Ed25519 for low-memory systems
 *
 * This package contains portable public-domain implementations of Daniel J.
 * Bernstein's Curve255191 Diffie-Hellman function, and of the Ed25519
 * signature system. The memory consumption is low enough that they could be
 * reasonably considered for most microcontroller applications. In particular,
 * Curve25519 scalar multiplication uses less than half a kB of peak stack
 * usage.
 *
 * ## Requirements
 *
 * C25519 requires around 1K of stack, so beware that you're allocating at
 * least `THREAD_STACKSIZE_DEFAULT + 1K` bytes.
 *
 * You can do it easily by adding:
 *
 * ```makefile
 * CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT + 1024)'
 * ```
 *
 * to your makefile.
 *
 * ## Usage
 *
 * Just add it as a package in your application:
 *
 * ```makefile
 * USEPKG += c25519
 * ```
 *
 * @see      https://www.dlbeer.co.nz/oss/c25519.html
 */
