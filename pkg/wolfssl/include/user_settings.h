#pragma once

/* user_settings.h : custom configuration for wolfcrypt/wolfSSL */

#ifdef __cplusplus
extern "C" {
#endif

/* System */
#ifndef WOLFSSL_RIOT_OS
#define WOLFSSL_RIOT_OS 1
#endif
#include <random.h>
#define CUSTOM_RAND_GENERATE random_uint32
#define CUSTOM_RAND_TYPE uint32_t
#define NO_WRITEV
#define NO_DEV_RANDOM
#define NO_FILESYSTEM
#define NO_WOLFSSL_MEMORY
#define NO_MAIN_DRIVER
#define NO_SIG_WRAPPER
#define NO_OLD_RNGNAME
#define HAVE_STRINGS_H
#define WOLFSSL_IPV6

/* Single precision math */
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_SMALL
#define SP_WORD_SIZE 32
#define WOLFSSL_SP

/* GNRC support enabled if not
 * using sockets
 */
#ifndef MODULE_WOLFSSL_SOCKET
#define WOLFSSL_GNRC
#define WOLFSSL_USER_IO
#else
#include <sys/socket.h>
#endif

/* Select wolfcrypt only / +wolfssl
 * at compile time (via USEMODULE)
 */
#ifndef MODULE_WOLFSSL_TLS
#ifndef MODULE_WOLFSSL_TLS13
#define WOLFCRYPT_ONLY
#else
#define NO_OLD_TLS
#define HAVE_TLS_EXTENSIONS
#define HAVE_AES_DECRYPT
#define HAVE_AESGCM
#define GCM_SMALL
#define HAVE_AESCCM
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_DIRECT
#endif
#else
#define HAVE_TLS_EXTENSIONS
#endif

/* Align on 32-bit (exc. native32,
 * don't modify default alignment.)
 */
#ifndef BOARD_NATIVE32
#define WOLFSSL_GENERAL_ALIGNMENT 4
#endif

/* ARM-specific optimizations */
#ifdef CPU_ARM
#define TFM_ARM
#endif

/* defined somewhere else */
int strncasecmp(const char *s1, const char * s2, size_t sz);

#define SINGLE_THREADED

/* Global settings */
#define SMALL_SESSION_CACHE
#define WOLFSSL_DH_CONST
#define WORD64_AVAILABLE
#define TFM_TIMING_RESISTANT
#define USE_CERT_BUFFERS_2048
#define NO_RC4

/* Modules */
#undef WC_NO_RNG
#ifndef MODULE_WOLFCRYPT_RANDOM
#define WC_NO_RNG
#endif

#undef WOLFSSL_DTLS
#ifdef MODULE_WOLFSSL_DTLS
#define WOLFSSL_DTLS
#endif

#undef WOLFSSL_DTLS13
#ifdef MODULE_WOLFSSL_DTLS13
#define WOLFSSL_DTLS13
#define HAVE_AEAD
#endif

#undef HAVE_FFDHE_2048
#ifdef MODULE_WOLFCRYPT_FFDHE_2048
#define HAVE_FFDHE_2048
#endif

#undef HAVE_CHACHA
#ifdef MODULE_WOLFCRYPT_CHACHA
#define HAVE_CHACHA
#endif

#undef HAVE_POLY1305
#ifdef MODULE_WOLFCRYPT_POLY1305
#define HAVE_POLY1305
#define HAVE_ONE_TIME_AUTH
#endif

#undef HAVE_CURVE25519
#ifdef MODULE_WOLFCRYPT_CURVE25519
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#endif

#undef HAVE_ED25519
#ifdef MODULE_WOLFCRYPT_ED25519
#define HAVE_ED25519
#define ED25519_SMALL
#endif

#undef NO_AES
#undef NO_CODING
#undef NO_CMAC
#ifndef MODULE_WOLFCRYPT_AES
#define NO_AES
#endif
#ifndef MODULE_WOLFCRYPT_CMAC
#define NO_CMAC
#endif
#ifndef MODULE_WOLFCRYPT_CODING
#define NO_CODING
#endif

#ifndef MODULE_WOLFCRYPT_ASN
#define NO_ASN
#endif

#ifndef MODULE_WOLFCRYPT_HMAC
#define NO_HMAC
#endif

#undef NO_SHA
#ifndef MODULE_WOLFCRYPT_SHA
#define NO_SHA
#else
#define USE_SLOW_SHA
#define USE_SLOW_SHA2
#endif

#undef HAVE_SHA512
#undef HAVE_SHA384
#undef WOLFSSL_SHA384
#undef WOLFSSL_SHA512
#ifdef MODULE_WOLFCRYPT_SHA512
#define HAVE_SHA384
#define HAVE_SHA512
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define USE_SLOW_SHA512
#endif

#undef WOLFSSL_SHA3
#ifdef MODULE_WOLFCRYPT_SHA3
#define WOLFSSL_SHA3
#endif

#undef HAVE_ECC
#ifdef MODULE_WOLFCRYPT_ECC
#define HAVE_ECC
#define FP_ECC
#define WOLFSSL_HAVE_SP_ECC
#define WOLFSSL_HAVE_SP_ECC
#define ECC_TIMING_RESISTANT
#define HAVE_SUPPORTED_CURVES
#endif

#undef HAVE_BLAKE2B
#ifdef MODULE_WOLFCRYPT_BLAKE2B
#define HAVE_BLAKE2B
#endif

#undef HAVE_CAMELLIA
#ifdef MODULE_WOLFCRYPT_CAMELLIA
#define HAVE_CAMELLIA
#endif

#undef HAVE_IDEA
#ifdef MODULE_WOLFCRYPT_IDEA
#define HAVE_IDEA
#endif

#undef HAVE_HC128
#ifdef MODULE_WOLFCRYPT_HC128
#define HAVE_HC128
#endif

#undef HAVE_PKCS7
#ifdef MODULE_WOLFCRYPT_PKCS7
#define HAVE_PKCS7
#endif

#undef NO_PKCS12
#ifndef MODULE_WOLFCRYPT_PKCS12
#define NO_PKCS12
#endif

#undef NO_PWDBASED
#ifndef MODULE_WOLFCRYPT_PWDBASED
#define NO_PWDBASED
#endif

#undef WOLFSSL_STATIC_PSK
#ifdef MODULE_WOLFSSL_PSK
#define WOLFSSL_STATIC_PSK
#endif

#undef HAVE_LIBZ
#ifdef MODULE_WOLFCRYPT_COMPRESS
#define HAVE_LIBZ
#endif

#ifdef MODULE_WOLFCRYPT_RSA
#define HAVE_RSA
#define RSA_LOW_MEM
#define WC_RSA_BLINDING
#define WOLFSSL_STATIC_RSA
#define WOLFSSL_HAVE_SP_DH
#define WOLFSSL_HAVE_SP_RSA
#else
#define NO_RSA
#endif

#undef NO_DES3
#ifndef MODULE_WOLFCRYPT_DES3
#define NO_DES3
#endif

#undef NO_DH
#ifndef MODULE_WOLFCRYPT_DH
#define NO_DH
#endif

#undef NO_DSA
#ifndef MODULE_WOLFCRYPT_DSA
#define NO_DSA
#endif

#undef WOLFSSL_MD2
#ifdef MODULE_WOLFSSL_MD2
#define WOLFSSL_MD2
#endif

#undef NO_MD4
#ifndef MODULE_WOLFCRYPT_MD4
#define NO_MD4
#endif

#undef NO_RABBIT
#ifndef MODULE_WOLFCRYPT_RABBIT
#define NO_RABBIT
#endif

#undef NO_MD5
#ifndef MODULE_WOLFCRYPT_MD5
#define NO_MD5
#endif

#undef WOLFSSL_RIPEMD
#ifdef MODULE_WOLFCRYPT_RIPEMD
#define WOLFSSL_RIPEMD
#endif

#undef NO_SIG_WRAPPER
#ifndef MODULE_WOLFCRYPT_SIGNATURE
#define NO_SIG_WRAPPER
#endif

#undef HAVE_SRP
#ifdef MODULE_WOLFCRYPT_SRP
#define HAVE_SRP
#endif

#undef HAVE_OCSP
#ifdef MODULE_WOLFSSL_OCSP
#define HAVE_OCSP
#endif

#undef HAVE_CRL
#ifdef MODULE_WOLFSSL_CRL
#define HAVE_CRL
#endif

#undef HAVE_TLS13
#ifdef MODULE_WOLFSSL_TLS13
#define HAVE_TLS13
#define WOLFSSL_TLS13
#define BUILD_TLS_AES_128_GCM_SHA256
#define NO_OLD_TLS
#define HAVE_TLS_EXTENSIONS
#define HAVE_AES_DECRYPT
#define HAVE_AESGCM
#define GCM_SMALL
#define HAVE_AESCCM
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_DIRECT
#define HAVE_FFDHE_4096
#define HAVE_HKDF
#define WC_RSA_PSS
#define WOLFSSL_SEND_HRR_COOKIE
#endif

#ifdef MODULE_WOLFSSL_DEBUG
#define DEBUG_WOLFSSL
#define WOLFSSL_LOG_PRINTF
#endif

#ifdef __cplusplus
}
#endif

/* The following defines should prevent declaration of name-colliding "Aes"
 * structure on SAML21, SAME54 platform code
 *
 * TODO: avoid pulling in all vendor files through cpu_conf.h
 */
#define _SAML21_AES_COMPONENT_
#define _SAMD51_AES_COMPONENT_
#define _SAME51_AES_COMPONENT_
#define _SAME54_AES_COMPONENT_
#define _SAMR34_AES_COMPONENT_
