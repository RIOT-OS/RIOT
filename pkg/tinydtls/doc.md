/**
 * @defgroup pkg_tinydtls  TinyDTLS for RIOT
 * @ingroup  pkg
 * @ingroup  net net_dtls
 * @brief    Provides the Eclipse TinyDTLS to RIOT
 * @see      https://projects.eclipse.org/projects/iot.tinydtls
 *
 * Usage
 * -----
 *
 * Add as a package in the Makefile of your application:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEPKG += tinydtls
 * # a cryptographically secure implementation of PRNG is needed
 * USEMODULE += prng_sha1prng
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Supported Cipher Suites
 * -----------------------
 *
 * TinyDTLS only has support for `TLS_PSK_WITH_AES_128_CCM_8` and
 * `TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8`. To choose which cipher suite
 * to use, add one of the following to your Makefile:
 *
 * For `TLS_PSK_WITH_AES_128_CCM_8` support (default):
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DCONFIG_DTLS_PSK
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * For `TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8` support:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DCONFIG_DTLS_ECC
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
 * @defgroup    tinydtls_config Tinydtls compile time configuration
 * @ingroup     pkg_tinydtls config
 * @brief       Provides compile-time configuration for tinydtls
 *
 * @{
 */

 /**
 * @brief Adds support for TLS_PSK_WITH_AES_128_CCM_8 when defined
 * @note  Activated by default if @ref CONFIG_DTLS_ECC is not defined
 */
#ifndef CONFIG_DTLS_PSK
#define CONFIG_DTLS_PSK
#endif

/**
 * @brief Adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 when defined
 */
#ifndef CONFIG_DTLS_ECC
#define CONFIG_DTLS_ECC
#endif

/**
 * @brief The maximum number of DTLS context at the same time
 */
#ifndef CONFIG_DTLS_CONTEXT_MAX
#define CONFIG_DTLS_CONTEXT_MAX    (2)
#endif

 /**
 * @brief The maximum number of concurrent DTLS handshakes
 */
#ifndef CONFIG_DTLS_HANDSHAKE_MAX
#define CONFIG_DTLS_HANDSHAKE_MAX  (2)
#endif

 /**
 * @brief The maximum number of concurrently used cipher keys
 */
#ifndef DTLS_SECURITY_MAX
#define DTLS_SECURITY_MAX   (CONFIG_DTLS_HANDSHAKE_MAX + CONFIG_DTLS_PEER_MAX)
#endif

 /**
 * @brief The maximum number of hash functions that can be used in parallel
 */
#ifndef DTLS_HASH_MAX
#define DTLS_HASH_MAX       (3 * CONFIG_DTLS_PEER_MAX)
#endif
/** @} */
