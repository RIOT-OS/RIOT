/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PSA_CRYPTO_PSA_KEY_TYPE_H
#define PSA_CRYPTO_PSA_KEY_TYPE_H

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        key/type.h
 * @brief       Key type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Encoding of a key type.
 *
 * @details This is a structured bitfield that identifies the category and type of key. The range
 *          of key type values is divided as follows:
 *          - @ref PSA_KEY_TYPE_NONE == @c 0:
 *            Reserved as an invalid key type.
 *          - @c 0x0001–0x7fff:
 *            Specification-defined key types. Key types defined by this standard always have bit
 *            15 clear. Unallocated key type values in this range are reserved for future use.
 *          - @c 0x8000–0xffff:
 *            Implementation-defined key types. Implementations that define additional key types
 *            must use an encoding with bit 15 set.
 */
typedef uint16_t psa_key_type_t;

/**
 * @brief   The type of PSA elliptic curve family identifiers.
 *
 * @details The curve identifier is required to create an ECC key using the
 *          @ref PSA_KEY_TYPE_ECC_KEY_PAIR() or @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY()
 *          macros.
 *
 *          The specific ECC curve within a family is identified by the @c key_bits
 *          attribute of the key.
 */
typedef uint8_t psa_ecc_family_t;

/**
 * @brief   The type of PSA finite-field Diffie-Hellman group family identifiers.
 *
 * @details The group family identifier is required to create a finite-field Diffie-Hellman
 *          key using the @ref PSA_KEY_TYPE_DH_KEY_PAIR() or @ref PSA_KEY_TYPE_DH_PUBLIC_KEY()
 *          macros.
 *
 *          The specific Diffie-Hellman group within a family is identified by the @c key_bits
 *          attribute of the key.
 */
typedef uint8_t psa_dh_family_t;

/**
 * @brief   An invalid key type value.
 *
 * @details Zero is not the encoding of any key type.
 */
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)

/**
 * @brief   Vendor-defined key type flag.
 *
 * @details Key types defined by this standard will never have the
 *          @ref PSA_KEY_TYPE_VENDOR_FLAG bit set. Vendors who define additional key types
 *          must use an encoding with the @ref PSA_KEY_TYPE_VENDOR_FLAG bit set and should
 *          respect the bitwise structure used by standard encodings whenever practical.
 */
#define PSA_KEY_TYPE_VENDOR_FLAG ((psa_key_type_t)0x8000)

/**
 * @brief   Mask for key type categories.
 */
#define PSA_KEY_TYPE_CATEGORY_MASK ((psa_key_type_t)0x7000)

/**
 * @brief   Raw key data type.
 */
#define PSA_KEY_TYPE_CATEGORY_RAW ((psa_key_type_t)0x1000)

/**
 * @brief   Symmetric key type.
 */
#define PSA_KEY_TYPE_CATEGORY_SYMMETRIC ((psa_key_type_t)0x2000)

/**
 * @brief   Asymmetric public key type.
 */
#define PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY ((psa_key_type_t)0x4000)

/**
 * @brief   Asymmetric key pair type.
 */
#define PSA_KEY_TYPE_CATEGORY_KEY_PAIR ((psa_key_type_t)0x7000)

/**
 * @brief   Asymmetric key pair flag.
 */
#define PSA_KEY_TYPE_CATEGORY_FLAG_PAIR ((psa_key_type_t)0x3000)

/**
 * @brief   Whether a key type is an unstructured array of bytes.
 *          This encompasses both symmetric keys and non-key data.
 *
 * @param   type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) \
    (((type) & 0x7000) == 0x1000 || ((type) & 0x7000) == 0x2000)

/**
 * @brief Whether a key type is asymmetric: either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) \
    (((type) & 0x4000) == 0x4000)

/**
 * @brief Whether a key type is the public part of a key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) \
    (((type) & 0x7000) == 0x4000)

/**
 * @brief Whether a key type is a key pair containing a private part and a public part.
 *
 * @param type  A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_KEY_PAIR(type) \
    (((type) & 0x7000) == 0x7000)

/**
 * @brief   Raw data.
 *
 * @details A “key” of this type cannot be used for any cryptographic operation. Applications can
 *          use this type to store arbitrary data in the keystore.
 *
 *          The bit size of a raw key must be a non-zero multiple of 8. The maximum size of a raw
 *          key is implementation defined.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HKDF (non-secret inputs)
 *          - @ref PSA_ALG_TLS12_PRF (non-secret inputs)
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS (non-secret inputs)
 */
#define PSA_KEY_TYPE_RAW_DATA ((psa_key_type_t)0x1001)

/**
 * @brief   HMAC key.
 *
 * @details The key policy determines which underlying hash algorithm the key can be used for.
 *
 *          The bit size of an HMAC key must be a non-zero multiple of 8. An HMAC key is typically
 *          the same size as the output of the underlying hash algorithm. An HMAC key that is
 *          longer than the block size of the underlying hash algorithm will be hashed before use.
 *
 *          When an HMAC key is created that is longer than the block size, it is implementation
 *          defined whether the implementation stores the original HMAC key, or the hash of the
 *          HMAC key. If the hash of the key is stored, the key size reported by
 *          @ref psa_get_key_attributes() will be the size of the hashed key.
 *
 * @note    @ref PSA_HASH_LENGTH(@p alg) provides the output size of hash algorithm @c alg,
 *          in bytes.
 *          @ref PSA_HASH_BLOCK_LENGTH(@p alg) provides the block size of hash algorithm
 *          @c alg, in bytes.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HMAC
 */
#define PSA_KEY_TYPE_HMAC ((psa_key_type_t)0x1100)

/**
 * @brief   A secret for key derivation.
 *
 * @details This key type is for high-entropy secrets only. For low-entropy secrets,
 *          @ref PSA_KEY_TYPE_PASSWORD should be used instead.
 *
 *          These keys can be used in the @ref PSA_KEY_DERIVATION_INPUT_SECRET or
 *          @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step of key derivation algorithms.
 *
 *          The key policy determines which key derivation algorithm the key can be used for.
 *
 *          The bit size of a secret for key derivation must be a non-zero multiple of 8. The
 *          maximum size of a secret for key derivation is implementation defined.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HKDF (secret input)
 *          - @ref PSA_ALG_TLS12_PRF (secret input)
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS (secret input)
 */
#define PSA_KEY_TYPE_DERIVE ((psa_key_type_t)0x1200)

/**
 * @brief   A low-entropy secret for password hashing or key derivation.
 *
 * @details This key type is suitable for passwords and passphrases which are typically intended to
 *          be memorizable by humans, and have a low entropy relative to their size. It can be used
 *          for randomly generated or derived keys with maximum or near-maximum entropy, but
 *          @ref PSA_KEY_TYPE_DERIVE is more suitable for such keys. It is not suitable for
 *          passwords with extremely low entropy, such as numerical PINs.
 *
 *          These keys can be used in the @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step of key
 *          derivation algorithms. Algorithms that accept such an input were designed to accept
 *          low-entropy secret and are known as password hashing or key stretching algorithms.
 *
 *          These keys cannot be used in the @ref PSA_KEY_DERIVATION_INPUT_SECRET input step of key
 *          derivation algorithms, as the algorithms expect such an input to have high entropy.
 *
 *          The key policy determines which key derivation algorithm the key can be used for, among
 *          the permissible subset defined above.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (password input)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (password input)
 */
#define PSA_KEY_TYPE_PASSWORD ((psa_key_type_t)0x1203)

/**
 * @brief   A secret value that can be used to verify a password hash.
 *
 * @details The key policy determines which key derivation algorithm the key can be used for, among
 *          the same permissible subset as for @ref PSA_KEY_TYPE_PASSWORD.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (key output and verification)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (key output and verification)
 */
#define PSA_KEY_TYPE_PASSWORD_HASH ((psa_key_type_t)0x1205)

/**
 * @brief   A secret value that can be used when computing a password hash.
 *
 * @details The key policy determines which key derivation algorithm the key can be used for, among
 *          the subset of algorithms that can use pepper.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (salt input)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (salt input)
 */
#define PSA_KEY_TYPE_PEPPER ((psa_key_type_t)0x1206)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the AES block cipher.
 *
 * @details The size of the key is related to the AES algorithm variant. For algorithms except the
 *          XTS block cipher mode, the following key sizes are used:
 *          - AES-128 uses a 16-byte key : @c key_bits = 128
 *          - AES-192 uses a 24-byte key : @c key_bits = 192
 *          - AES-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - AES-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - AES-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - AES-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The AES block cipher is defined in FIPS Publication 197: Advanced Encryption Standard
 *          (AES) [FIPS197](https://doi.org/10.6028/NIST.FIPS.197).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_AES ((psa_key_type_t)0x2400)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the ARIA block cipher.
 *
 *          The size of the key is related to the ARIA algorithm variant. For algorithms except the
 *          XTS block cipher mode, the following key sizes are used:
 *          - ARIA-128 uses a 16-byte key : @c key_bits = 128
 *          - ARIA-192 uses a 24-byte key : @c key_bits = 192
 *          - ARIA-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - ARIA-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - ARIA-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - ARIA-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The ARIA block cipher is defined in A Description of the ARIA Encryption Algorithm
 *          [RFC5794](https://datatracker.ietf.org/doc/html/rfc5794).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_ARIA ((psa_key_type_t)0x2406)

/**
 * @brief   Key for a cipher or MAC algorithm based on DES or 3DES (Triple-DES).
 *
 * @details The size of the key determines which DES algorithm is used:
 *          - Single DES uses an 8-byte key : @c key_bits = 64
 *          - 2-key 3DES uses a 16-byte key : @c key_bits = 128
 *          - 3-key 3DES uses a 24-byte key : @c key_bits = 192
 *
 * @warning Single DES and 2-key 3DES are weak and strongly deprecated and are only recommended for
 *          decrypting legacy data.
 *          3-key 3DES is weak and deprecated and is only recommended for use in legacy protocols.
 *
 *          The DES and 3DES block ciphers are defined in NIST Special Publication 800-67:
 *          Recommendation for the Triple Data Encryption Algorithm (TDEA) Block Cipher
 *          [SP800-67](https://doi.org/10.6028/NIST.SP.800-67r2).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 */
#define PSA_KEY_TYPE_DES ((psa_key_type_t)0x2301)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the Camellia block cipher.
 *
 * @details The size of the key is related to the Camellia algorithm variant. For algorithms except
 *          the XTS block cipher mode, the following key sizes are used:
 *          - Camellia-128 uses a 16-byte key : @c key_bits = 128
 *          - Camellia-192 uses a 24-byte key : @c key_bits = 192
 *          - Camellia-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - Camellia-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - Camellia-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - Camellia-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The Camellia block cipher is defined in Specification of Camellia — a 128-bit Block
 *          Cipher [NTT-CAM](https://info.isl.ntt.co.jp/crypt/eng/camellia/specifications) and
 *          also described in A Description of the Camellia Encryption
 *          Algorithm [RFC3713](https://tools.ietf.org/html/rfc3713).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_CAMELLIA ((psa_key_type_t)0x2403)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the SM4 block cipher.
 *
 * @details For algorithms except the XTS block cipher mode, the SM4 key size is 128 bits
 *          (16 bytes).
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the SM4 key size is 256 bits
 *          (two 16-byte keys).
 *
 *          The SM4 block cipher is defined in GM/T 0002-2012: SM4 block cipher algorithm
 *          [CSTC0002](http://www.gmbz.org.cn/main/viewfile/20180108015408199368.html)
 *          (English version [CSTC0002/E](http://www.gmbz.org.cn/main/postDetail.html?
 *          id=20180404044052)).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_SM4 ((psa_key_type_t)0x2405)

/**
 * @brief   Key for the ARC4 stream cipher.
 *
 * @warning The ARC4 cipher is weak and deprecated and is only recommended for use in legacy
 *          protocols.
 *
 * @details The ARC4 cipher supports key sizes between 40 and 2048 bits, that are multiples of 8.
 *          (5 to 256 bytes)
 *
 *          Use algorithm @ref PSA_ALG_STREAM_CIPHER to use this key with the ARC4 cipher.
 */
#define PSA_KEY_TYPE_ARC4 ((psa_key_type_t)0x2002)

/**
 * @brief   Key for the ChaCha20 stream cipher or the ChaCha20-Poly1305 AEAD algorithm.
 *
 * @details The ChaCha20 key size is 256 bits (32 bytes).
 *          - Use algorithm @ref PSA_ALG_STREAM_CIPHER to use this key with the ChaCha20 cipher for
 *            unauthenticated encryption. See @ref PSA_ALG_STREAM_CIPHER for details of this
 *            algorithm.
 *          - Use algorithm @ref PSA_ALG_CHACHA20_POLY1305 to use this key with the ChaCha20 cipher
 *            and Poly1305 authenticator for AEAD. See @ref PSA_ALG_CHACHA20_POLY1305 for details
 *            of this algorithm.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_STREAM_CIPHER
 *          - @ref PSA_ALG_CHACHA20_POLY1305
 */
#define PSA_KEY_TYPE_CHACHA20 ((psa_key_type_t)0x2004)

/**
 * @brief   RSA key pair: both the private and public key.
 *
 * @details The size of an RSA key is the bit size of the modulus.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_RSA_OAEP
 *          - @ref PSA_ALG_RSA_PKCS1V15_CRYPT
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW
 *          - @ref PSA_ALG_RSA_PSS
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT
 */
#define PSA_KEY_TYPE_RSA_KEY_PAIR ((psa_key_type_t)0x7001)

/**
 * @brief   RSA public key.
 *
 * @details The size of an RSA key is the bit size of the modulus.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_RSA_OAEP (encryption only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_CRYPT (encryption only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN (signature verification only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW (signature verification only)
 *          - @ref PSA_ALG_RSA_PSS (signature verification only)
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT (signature verification only)
 */
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY ((psa_key_type_t)0x4001)

/**
 * @brief Whether a key type is an RSA key. This includes both key pairs and public keys.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_RSA(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == 0x4001)

/**
 * @brief   Base value for ECC key pairs
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR_BASE ((psa_key_type_t)0x7100)

/**
 * @brief   Elliptic curve key pair: both the private and public key.
 *
 * @details The size of an elliptic curve key is the bit size associated with the curve, that is,
 *          the bit size of q for a curve over a field Fq. See the documentation of each Elliptic
 *          curve family for details.
 *
 *          @b Compatible @b algorithms
 *          -   Elliptic curve key pairs can be used in Asymmetric signature and Key agreement
 *              algorithms.
 *          -   The set of compatible algorithms depends on the Elliptic curve key family. See the
 *              Elliptic curve family for details.
 *
 * @param   curve A value of type @ref psa_ecc_family_t that identifies the ECC curve to be used.
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve)         \
    (PSA_KEY_TYPE_ECC_KEY_PAIR_BASE | (curve))

/**
 * @brief   Base value for ECC public keys
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE ((psa_key_type_t)0x4100)

/**
 * @brief   Mask for ECC curves
 */
#define PSA_KEY_TYPE_ECC_CURVE_MASK ((psa_key_type_t)0x00ff)

/**
 * @brief   Elliptic curve public key.
 *
 * @details The size of an elliptic curve public key is the same as the corresponding private key.
 *          See @ref PSA_KEY_TYPE_ECC_KEY_PAIR() and the documentation of each Elliptic curve
 *          family for details.
 *
 *          @b Compatible @b algorithms
 *          Elliptic curve public keys can be used for verification in asymmetric signature
 *          algorithms.
 *          The set of compatible algorithms depends on the elliptic curve key family. See each
 *          elliptic curve family for details.
 *
 * @param   curve A value of type @ref psa_ecc_family_t that identifies the ECC curve to be used.
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)              \
    (PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE | (curve))

/**
 * @brief   SEC Koblitz curves over prime fields.
 *
 * @details This family comprises the following curves:
 *          - `secp192k1` : @c key_bits = 192
 *          - `secp224k1` : @c key_bits = 225
 *          - `secp256k1` : @c key_bits = 256
 *
 *          They are defined in SEC 2: Recommended Elliptic Curve Domain Parameters
 *          [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_K1 ((psa_ecc_family_t)0x17)

/**
 * @brief   SEC random curves over prime fields.
 *
 * @details This family comprises the following curves:
 *          - `secp192r1` : @c key_bits = 192
 *          - `secp224r1` : @c key_bits = 224
 *          - `secp256r1` : @c key_bits = 256
 *          - `secp384r1` : @c key_bits = 384
 *          - `secp521r1` : @c key_bits = 521
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_R1 ((psa_ecc_family_t)0x12)

/**
 * @warning This family of curves is weak and deprecated.
 *
 * @details This family comprises the following curves:
 *          - `secp160r2` : @c key_bits = 160 (Deprecated)
 *
 *          It is defined in the superseded SEC 2: Recommended Elliptic Curve Domain Parameters,
 *          Version 1.0 [SEC2v1](https://www.secg.org/SEC2-Ver-1.0.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_R2 ((psa_ecc_family_t)0x1b)

/**
 * @brief   SEC Koblitz curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163k1` : @c key_bits = 163 (Deprecated)
 *          - `sect233k1` : @c key_bits = 233
 *          - `sect239k1` : @c key_bits = 239
 *          - `sect283k1` : @c key_bits = 283
 *          - `sect409k1` : @c key_bits = 409
 *          - `sect571k1` : @c key_bits = 571
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163k1` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_K1 ((psa_ecc_family_t)0x27)

/**
 * @brief   SEC random curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163r1` : @c key_bits = 163 (Deprecated)
 *          - `sect233r1` : @c key_bits = 233
 *          - `sect283r1` : @c key_bits = 283
 *          - `sect409r1` : @c key_bits = 409
 *          - `sect571r1` : @c key_bits = 571
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163r1` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_R1 ((psa_ecc_family_t)0x22)

/**
 * @brief   SEC additional random curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163r2` : @c key_bits = 163 (Deprecated)
 *
 *          It is defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163r2` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_R2 ((psa_ecc_family_t)0x2b)

/**
 * @brief   Brainpool `P` random curves.
 *
 * @details This family comprises the following curves:
 *          - `brainpoolP160r1` : @c key_bits = 160 (Deprecated)
 *          - `brainpoolP192r1` : @c key_bits = 192
 *          - `brainpoolP224r1` : @c key_bits = 224
 *          - `brainpoolP256r1` : @c key_bits = 256
 *          - `brainpoolP320r1` : @c key_bits = 320
 *          - `brainpoolP384r1` : @c key_bits = 384
 *          - `brainpoolP512r1` : @c key_bits = 512
 *
 *          They are defined in Elliptic Curve Cryptography (ECC) Brainpool Standard Curves and
 *          Curve Generation [RFC5639](https://tools.ietf.org/html/rfc5639.html).
 *
 * @warning The 160-bit curve `brainpoolP160r1` is weak and deprecated and is only recommended for
 *          use in legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 ((psa_ecc_family_t)0x30)

/**
 * @brief   Curve used primarily in France and elsewhere in Europe.
 *
 * @details This family comprises one 256-bit curve:
 *          - `FRP256v1` : @c key_bits = 256
 *
 *          This is defined by Publication d'un paramétrage de courbe elliptique visant des
 *          applications de passeport électronique et de l'administration électronique française
 *          [FRP](https://www.ssi.gouv.fr/agence/rayonnement-scientifique/
 *          publications-scientifiques/articles-ouvrages-actes).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_FRP ((psa_ecc_family_t)0x33)

/**
 * @brief   Montgomery curves.
 *
 * @details This family comprises the following Montgomery curves:
 *          - `Curve25519` : @c key_bits = 255
 *          - `Curve448` : @c key_bits = 448
 *
 *          `Curve25519` is defined in Curve25519: new Diffie-Hellman speed records
 *          [Curve25519](https://www.iacr.org/archive/pkc2006/39580209/39580209.pdf).
 *          `Curve448` is defined in Ed448-Goldilocks, a new elliptic curve
 *          [Curve448](https://eprint.iacr.org/2015/625.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_MONTGOMERY ((psa_ecc_family_t)0x41)

/**
 * @brief   Twisted Edwards curves.
 *
 * @details This family comprises the following twisted Edwards curves:
 *          - `Edwards25519` : @c key_bits = 255. This curve is birationally equivalent to
 *            `Curve25519`.
 *          - `Edwards448` : @c key_bits = 448. This curve is birationally equivalent to `Curve448`.
 *
 *          Edwards25519 is defined in Twisted Edwards curves
 *          [Ed25519](https://eprint.iacr.org/2008/013.pdf). Edwards448 is defined in
 *          Ed448-Goldilocks, a new elliptic curve [Curve448](https://eprint.iacr.org/2015/625.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PURE_EDDSA
 *          - @ref PSA_ALG_ED25519PH (Edwards25519 only)
 *          - @ref PSA_ALG_ED448PH (Edwards448 only)
 */
#define PSA_ECC_FAMILY_TWISTED_EDWARDS ((psa_ecc_family_t)0x42)

/**
 * @brief Whether a key type is an elliptic curve key, either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4100)

/**
 * @brief Whether a key type is an elliptic curve key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7100)

/**
 * @brief Whether a key type is an elliptic curve public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4100)

/**
 * @brief   Extract the curve family from an elliptic curve key type.
 *
 * @param   type An elliptic curve key type: a value of type @ref psa_key_type_t such that
 *          @ref PSA_KEY_TYPE_IS_ECC(@p type) is true.
 *
 * @return  The elliptic curve family id, if type is a supported elliptic curve key.
 *          Unspecified if type is not a supported elliptic curve key.
 */
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) \
    ((psa_ecc_family_t)((type) & 0x00ff))

/**
 * @brief   Finite-field Diffie-Hellman key pair: both the private key and public key.
 *
 * @details @b Compatible @b algorithms
 *          - @ref PSA_ALG_FFDH
 *
 * @param   group A value of type @ref psa_dh_family_t that identifies the Diffie-Hellman group
 *          family to be used.
 */
#define PSA_KEY_TYPE_DH_KEY_PAIR(group) \
    ((psa_key_type_t)(0x7200 | (group)))

/**
 * @brief   Finite-field Diffie-Hellman public key.
 *
 * @details @b Compatible @b algorithms
 *          None. Finite-field Diffie-Hellman public keys are exported to use in a key agreement
 *          algorithm, and the peer key is provided to the @ref PSA_ALG_FFDH key agreement
 *          algorithm as a buffer of key data.
 *
 * @param   group A value of type @ref psa_dh_family_t that identifies the Diffie-Hellman group
 *          family to be used.
 */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group) \
    ((psa_key_type_t)(0x4200 | (group)))

/**
 * @brief   Finite-field Diffie-Hellman groups defined for TLS in RFC 7919.
 *
 * @details This family includes groups with the following key sizes (in bits): 2048, 3072, 4096,
 *          6144, 8192. An implementation can support all of these sizes or only a subset.
 *
 *          Keys is this group can only be used with the @ref PSA_ALG_FFDH key agreement algorithm.
 *
 *          These groups are defined by Negotiated Finite Field Diffie-Hellman Ephemeral Parameters
 *          for Transport Layer Security (TLS) [RFC7919](https://tools.ietf.org/html/rfc7919.html)
 *          Appendix A.
 */
#define PSA_DH_FAMILY_RFC7919 ((psa_dh_family_t)0x03)

/**
 * @brief   The key pair type corresponding to a public key type.
 *
 * @details If type is a key pair type, it will be left unchanged.
 *
 * @param   type A public key type or key pair type.
 *
 * @return  The corresponding key pair type. If type is not a public key or a key pair, the return
 *          value is undefined.
 */
#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
    ((psa_key_type_t)((type) | 0x3000))

/**
 * @brief   The public key type corresponding to a key pair type.
 *          You may also pass a key pair type as type, it will be left unchanged.
 *
 * @param   type A public key type or key pair type.
 *
 * @return  The corresponding public key type.
 *          If type is not a public key or a key pair, the return value is undefined.
 */
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
    ((psa_key_type_t)((type) & ~0x3000))

/**
 * @brief Whether a key type is a Diffie-Hellman key, either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4200)

/**
 * @brief Whether a key type is a Diffie-Hellman key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7200)

/**
 * @brief Whether a key type is a Diffie-Hellman public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4200)

/**
 * @brief   Extract the group family from a Diffie-Hellman key type.
 *
 * @param   type A Diffie-Hellman key type: a value of type @ref psa_key_type_t such that
 *          @ref PSA_KEY_TYPE_IS_DH(@p type) is true.
 *
 * @return  The Diffie-Hellman group family id, if type is a supported Diffie-Hellman key.
 *          Unspecified if type is not a supported Diffie-Hellman key.
 */
#define PSA_KEY_TYPE_DH_GET_FAMILY(type) \
    ((psa_dh_family_t)((type) & 0x00ff))

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PSA_CRYPTO_PSA_KEY_TYPE_H */
