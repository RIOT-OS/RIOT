/**
 * @defgroup pkg_edhoc_c EDHOC-C
 * @ingroup  pkg
 * @brief    Support for Ephemeral Diffie-Hellman Over COSE (EDHOC)
 * @see      https://github.com/openwsn-berkeley/EDHOC-C
 *
 * # EDHOC-C RIOT package
 *
 * ## Usage
 *
 * Just add it as a package in your application:
 *
 * ```makefile
 * USEPKG += edhoc-c
 * ```
 *
 * ### Backends
 *
 * EDHOC-C does not implement crypographic algorithms, instead it uses external
 * libraries as a backend. Libraries that provide all requirements are
 * currently @ref pkg_tinycrypt and @ref pkg_wolfssl. Pick one through the
 * following module:
 *
 * ```makefile
 * USEMODULE += edhoc-c_crypto_wolfssl
 * USEMODULE += edhoc-c_crypto_tinycrypt
 * ```
 *
 * EDHOC-C does not implement a CBOR library either, currently the only possible
 * backend is @ref pkg_nanocbor. Select it through the following module:
 *
 * ```makefile
 * USEMODULE += edhoc-c_cbor_nanocbor
 * ```
 *
 * Don't forget to include the header for the EDHOC-C public API:
 *
 * ```c
 * #include <edhoc/edhoc.h>
 * ```
 *
 * ### Current Support
 *
 * - EDHOC-C supports @ref pkg_hacl as a crypto backend but it's using a different
 *   version than the one supported currently in RIOT
 * - x509 certificates require MBED-TLS, which is currently not supported in
 *   RIOT so only RPK and CBOR certificates are supported.
 */
