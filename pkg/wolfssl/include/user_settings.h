/* user_settings.h : custom configuration for wolfcrypt/wolfSSL */

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* System */
#ifndef WOLFSSL_RIOT_OS
#   define WOLFSSL_RIOT_OS 1
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
#define WOLFSSL_SMALL_STACK

/* Single precision math */
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_SMALL
#define SP_WORD_SIZE 32


#ifndef MODULE_WOLFSSL_SOCKET
#   define WOLFSSL_GNRC
#   define WOLFSSL_USER_IO
#endif

#ifndef MODULE_WOLFSSL_TLS
#   ifndef MODULE_WOLFSSL_TLS13
#       define WOLFCRYPT_ONLY
#   else
#       define NO_OLD_TLS
#   endif
#endif

#ifdef BOARD_NATIVE
//#   define WOLFSSL_GENERAL_ALIGNMENT 8
#else
#   define WOLFSSL_GENERAL_ALIGNMENT 4
#   ifdef CPU_ARM
#      define TFM_ARM
#   endif
#endif

/* defined somewhere else */
int strncasecmp(const char *s1, const char * s2, unsigned int sz);

/* Fixme: add mt */
#define SINGLE_THREADED

/* Global settings */
#define SMALL_SESSION_CACHE
#define WOLFSSL_DH_CONST
#define WORD64_AVAILABLE
#define TFM_TIMING_RESISTANT
#define USE_CERT_BUFFERS_2048
#define NO_RC4

/* Random */
#undef WC_NO_RNG
#ifndef MODULE_WOLFCRYPT_RANDOM
#   define WC_NO_RNG
#endif


/* DTLS */
#undef WOLFSSL_DTLS
#ifdef MODULE_WOLFSSL_DTLS
#   define WOLFSSL_DTLS
#endif

/* FFDHE_2048 */
#undef HAVE_FFDHE_2048
#ifdef MODULE_WOLFCRYPT_FFDHE_2048
#   define HAVE_FFDHE_2048
#endif

/* ChaCha20 */
#undef HAVE_CHACHA
#ifdef MODULE_WOLFCRYPT_CHACHA
#   define HAVE_CHACHA
#endif

/* Poly 1305 */
#undef HAVE_POLY1305
#ifdef MODULE_WOLFCRYPT_POLY1305
#   define HAVE_POLY1305
#   define HAVE_ONE_TIME_AUTH
#endif

/* x25519 */
#undef HAVE_CURVE25519
#ifdef MODULE_WOLFCRYPT_CURVE25519
#   define HAVE_CURVE25519
#   define CURVE25519_SMALL
#endif

/* ed25519 */
#undef HAVE_ED25519
#ifdef MODULE_WOLFCRYPT_ED25519
#   define HAVE_ED25519
#   define ED25519_SMALL
#endif

/* AES */
#undef NO_AES
#undef NO_CODING
#undef NO_CMAC
#ifndef MODULE_WOLFCRYPT_AES
#	define NO_AES
#endif
#ifndef MODULE_WOLFCRYPT_CMAC
#	define NO_CMAC
#endif
#ifndef MODULE_WOLFCRYPT_CODING
#	define NO_CODING
#endif

/* ASN */
#ifndef MODULE_WOLFCRYPT_ASN
#	define NO_ASN
#endif

/* HMAC */
#ifndef MODULE_WOLFCRYPT_HMAC
#	define NO_HMAC
#endif

#undef NO_SHA
#ifndef MODULE_WOLFCRYPT_SHA
#   define NO_SHA
#else
#   define USE_SLOW_SHA
#   define USE_SLOW_SHA2
#endif

/* SHA-2 */
#undef HAVE_SHA512
#undef HAVE_SHA384
#undef WOLFSSL_SHA384
#undef WOLFSSL_SHA512
#ifdef MODULE_WOLFCRYPT_SHA512
#   define HAVE_SHA384
#   define HAVE_SHA512
#   define WOLFSSL_SHA384
#   define WOLFSSL_SHA512
#   define USE_SLOW_SHA512
#endif

/* SHA-3 Keccac */
#undef WOLFSSL_SHA3
#ifdef MODULE_WOLFCRYPT_SHA3
#	define WOLFSSL_SHA3
#endif

/* ecc */
#undef HAVE_ECC
#ifdef MODULE_WOLFCRYPT_ECC
  #define HAVE_ECC
  #define FP_ECC
  #define WOLFSSL_HAVE_SP_ECC
  #define ECC_TIMING_RESISTANT
#endif

#undef HAVE_BLAKE2B
#ifdef MODULE_WOLFCRYPT_BLAKE2B
#	define HAVE_BLAKE2B
#endif

#undef HAVE_CAMELLIA
#ifdef MODULE_WOLFCRYPT_CAMELLIA
#	define HAVE_CAMELLIA
#endif

#undef HAVE_IDEA
#ifdef MODULE_WOLFCRYPT_IDEA
#	define HAVE_IDEA
#endif

#undef HAVE_HC128
#ifdef MODULE_WOLFCRYPT_HC128
#	define HAVE_HC128
#endif

#undef HAVE_PKCS7
#ifdef MODULE_WOLFCRYPT_PKCS7
#	define HAVE_PKCS7
#endif

/* PKCS12 */
#undef NO_PKCS12
#ifndef MODULE_WOLFCRYPT_PKCS12
#	define NO_PKCS12
#endif

/* PWDBASED */
#undef NO_PWDBASED
#ifndef MODULE_WOLFCRYPT_PWDBASED
#	define NO_PWDBASED
#endif

#undef HAVE_LIBZ
#ifdef MODULE_WOLFCRYPT_COMPRESS
#	define HAVE_LIBZ
#endif

/* rsa */
#ifdef MODULE_WOLFCRYPT_RSA
#   define HAVE_RSA
#   define RSA_LOW_MEM
#   define WC_RSA_BLINDING
#else
#	define NO_RSA
#endif

/* 3-DES */
#undef NO_DES3
#ifndef MODULE_WOLFCRYPT_DES3
#	define NO_DES3
#endif

/* DH */
#undef NO_DH
#ifndef MODULE_WOLFCRYPT_DH
#	define NO_DH
#endif

/* DSA */
#undef NO_DSA
#ifndef MODULE_WOLFCRYPT_DSA
#	define NO_DSA
#endif

/* MD2 */
#undef WOLFSSL_MD2
#ifdef MODULE_WOLFSSL_MD2
  #define WOLFSSL_MD2
#endif

/* MD4 */
#undef NO_MD4
#ifndef MODULE_WOLFCRYPT_MD4
#	define NO_MD4
#endif

/* RABBIT */
#undef NO_RABBIT
#ifndef MODULE_WOLFCRYPT_RABBIT
#	define NO_RABBIT
#endif

/* MD5 */
#undef NO_MD5
#ifndef MODULE_WOLFCRYPT_MD5
#	define NO_MD5
#endif

/* ripe-md */
#undef WOLFSSL_RIPEMD
#ifdef MODULE_WOLFCRYPT_RIPEMD
#   define WOLFSSL_RIPEMD
#endif

/* signature wrapper */
#undef NO_SIG_WRAPPER
#ifndef MODULE_WOLFCRYPT_SIGNATURE
#	define NO_SIG_WRAPPER
#endif

/* SRP */
#undef HAVE_SRP
#ifdef MODULE_WOLFCRYPT_SRP
  #define HAVE_SRP
#endif

/** TLS Options **/
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
    #define HAVE_TLS_EXTENSIONS
    #define BUILD_TLS_AES_128_GCM_SHA256
#endif

#ifdef __cplusplus
}
#endif

#endif /* USER_SETTINGS_H */
