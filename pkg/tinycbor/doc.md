/**
 * @defgroup pkg_tinycbor TinyCBOR library
 * @ingroup  pkg
 * @brief    Provides the TinyCBOR (Concise Binary Object Representation)
 * library
 * @see https://github.com/intel/tinycbor/
 *
 * # TinyCBOR
 *
 * TinyCBOR is a CBOR encoder and decoder with a very small footprint, optimized
 * for very fast operation.
 *
 * The main encoder and decoder functions don't use dynamic memory allocation.
 *
 * # Usage
 *
 * Just add it as a package in your application's Makefile:
 *
 * ```makefile
 * USEPKG += tinycbor
 * ```
 *
 * TinyCBOR has optional floating point support. In RIOT-OS this is enabled by
 * adding the following line to your application's Makefile:
 *
 * ```makefile
 * USEMODULE += tinycbor_float
 * ```
 */
