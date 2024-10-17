/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_values.h
 * @brief       Value definitions for PSA Crypto.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @note        Some of the macros in this file have already been copied here from
 *              the PSA API specification, but are not implemented, yet.
 *              They are marked by comments that either say "specification-defined"
 *              or "implementation-defined".
 *              These macros will be implemented successively in the future.
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_VALUES_H
#define PSA_CRYPTO_PSA_CRYPTO_VALUES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto_types.h"
#include "psa/hash/algorithm.h"
#include "psa/mac/algorithm.h"
#include "psa/cipher/algorithm.h"
#include "psa/aead/algorithm.h"
#include "psa/key_agreement/algorithm.h"
#include "psa/key_derivation/algorithm.h"
#include "psa/asymmetric_encryption/algorithm.h"
#include "psa/asymmetric_signature/algorithm.h"

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
 * @brief   A context for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_CONTEXT /* implementation-defined value */

/**
 * @brief   A cost parameter for password hashing or key stretching.
 *
 * @details This must be a direct input, passed to @ref psa_key_derivation_input_integer().
 */
#define PSA_KEY_DERIVATION_INPUT_COST /* implementation-defined value */

/**
 * @brief   An information string for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_INFO /* implementation-defined value */

/**
 * @brief   A label for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_LABEL /* implementation-defined value */

/**
 * @brief   A low-entropy secret input for password hashing or key stretching.
 *
 * @details This is usually a key of type @ref PSA_KEY_TYPE_PASSWORD passed to
 *          @ref psa_key_derivation_input_key() or a direct input passed to
 *          @ref psa_key_derivation_input_bytes() that is a password or passphrase. It can also be
 *          high-entropy secret, for example, a key of type @ref PSA_KEY_TYPE_DERIVE, or the shared
 *          secret resulting from a key agreement.
 *
 *          If the secret is a direct input, the derivation operation cannot be used to derive
 *          keys: the operation will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD /* implementation-defined value */

/**
 * @brief   A salt for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA or @ref PSA_KEY_TYPE_PEPPER.
 */
#define PSA_KEY_DERIVATION_INPUT_SALT /* implementation-defined value */

/**
 * @brief   A high-entropy secret input for key derivation.
 *
 * @details This is typically a key of type @ref PSA_KEY_TYPE_DERIVE passed to
 *          @ref psa_key_derivation_input_key(), or the shared secret resulting from a key
 *          agreement obtained via @ref psa_key_derivation_key_agreement().
 *
 *          The secret can also be a direct input passed to @ref psa_key_derivation_input_bytes().
 *          In this case, the derivation operation cannot be used to derive keys: the operation
 *          will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_SECRET /* implementation-defined value */

/**
 * @brief   A seed for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_SEED /* implementation-defined value */

/**
 * @brief   Use the maximum possible capacity for a key derivation operation.
 *
 * @details Use this value as the capacity argument when setting up a key derivation to specify
 *          that the operation will use the maximum possible capacity. The value of the maximum
 *          possible capacity depends on the key derivation algorithm.
 */
#define PSA_KEY_DERIVATION_UNLIMITED_CAPACITY \
/* implementation-defined value */

/**
 * @brief   The null key identifier.
 *
 * @details The null key identifier is always invalid, except when used without in a call to
 *          @ref psa_destroy_key() which will return @ref PSA_SUCCESS.
 */
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)

/**
 * @brief The maximum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)

/**
 * @brief The maximum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)

/**
 * @brief   Construct a lifetime from a persistence level and a location.
 *
 * @param   persistence The persistence level: a value of type @ref psa_key_persistence_t.
 * @param   location    The location indicator: a value of type @ref psa_key_location_t.
 *
 * @return  The constructed lifetime value.
 */
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))

/**
 * @brief   Extract the location indicator from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t)((lifetime) >> 8))

/**
 * @brief   Extract the persistence level from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t)((lifetime) & 0x000000ff))

/**
 * @brief   Whether a key lifetime indicates that the key is volatile.
 *
 *          A volatile key is automatically destroyed by the implementation when the application
 *          instance terminates. In particular, a volatile key is automatically destroyed on a
 *          power reset of the device.
 *
 *          A key that is not volatile is persistent. Persistent keys are preserved until the
 *          application explicitly destroys them or until an implementation-specific device
 *          management event occurs, for example, a factory reset.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 *
 * @return  1 if the key is volatile, otherwise 0.
 */
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)

/**
 * @brief   The default lifetime for persistent keys.
 *
 * @details A persistent key remains in storage until it is explicitly destroyed or until the
 *          corresponding storage area is wiped. This specification does not define any mechanism
 *          to wipe a storage area. Implementations are permitted to provide their own mechanism,
 *          for example, to perform a factory reset, to prepare for device refurbishment, or to
 *          uninstall an application.
 *
 *          This lifetime value is the default storage area for the calling application.
 *          Implementations can offer other storage areas designated by other lifetime values as
 *          implementation-specific extensions.
 */
#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t)0x00000001)

/**
 * @brief   The default lifetime for volatile keys.
 *
 * @details A volatile key only exists as long as its identifier is not destroyed. The key material
 *          is guaranteed to be erased on a power reset.
 *
 *          A key with this lifetime is typically stored in the RAM area of the PSA Crypto
 *          subsystem. However this is an implementation choice. If an implementation stores data
 *          about the key in a non-volatile memory, it must release all the resources associated
 *          with the key and erase the key material if the calling application terminates.
 */
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t)0x00000000)

/**
 * @brief   The local storage area for persistent keys.
 *
 * @details This storage area is available on all systems that can store persistent keys without
 *          delegating the storage to a third-party cryptoprocessor.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_LOCAL_STORAGE          ((psa_key_location_t)0x000000)

/**
 * @brief   The default secure element storage area for persistent keys.
 *
 * @details This storage location is available on systems that have one or more secure elements
 *          that are able to store keys.
 *
 *          Vendor-defined locations must be provided by the system for storing keys in additional
 *          secure elements.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t)0x000001)

/**
 * @brief   Mark vendor defined key locations
 */
#define PSA_KEY_LOCATION_VENDOR_FLAG            ((psa_key_location_t)0x800000)

/**
 * @brief   Minimum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MIN (PSA_KEY_LOCATION_VENDOR_FLAG)

/**
 * @brief   Maximum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MAX ((psa_key_location_t)0x8000ff)

/**
 * @brief   The default persistence level for persistent keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t)0x01)

/**
 * @brief   A persistence level indicating that a key is never destroyed.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t)0xff)

/**
 * @brief   The persistence level of volatile keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t)0x00)

/**
 * @brief   Vendor-defined key type flag.
 *
 * @details Key types defined by this standard will never have the
 *          @ref PSA_KEY_TYPE_VENDOR_FLAG bit set. Vendors who define additional key types
 *          must use an encoding with the @ref PSA_KEY_TYPE_VENDOR_FLAG bit set and should
 *          respect the bitwise structure used by standard encodings whenever practical.
 */
#define PSA_KEY_TYPE_VENDOR_FLAG                    ((psa_key_type_t)0x8000)

/**
 * @brief   Mask for key type categories
 */
#define PSA_KEY_TYPE_CATEGORY_MASK                  ((psa_key_type_t)0x7000)

/**
 * @brief   Raw key data type
 */
#define PSA_KEY_TYPE_CATEGORY_RAW                   ((psa_key_type_t)0x1000)

/**
 * @brief   Symmetric key type
 */
#define PSA_KEY_TYPE_CATEGORY_SYMMETRIC             ((psa_key_type_t)0x2000)

/**
 * @brief   Asymmetric public key type
 */
#define PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY            ((psa_key_type_t)0x4000)

/**
 * @brief   Asymmetric key pair type
 */
#define PSA_KEY_TYPE_CATEGORY_KEY_PAIR              ((psa_key_type_t)0x7000)

/**
 * @brief   Asymmetric key pair flag
 */
#define PSA_KEY_TYPE_CATEGORY_FLAG_PAIR             ((psa_key_type_t)0x3000)

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
 * @brief   Base value for ECC public keys
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE            ((psa_key_type_t)0x4100)

/**
 * @brief   Base value for ECC key pairs
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR_BASE              ((psa_key_type_t)0x7100)

/**
 * @brief   Mask for ECC curves
 */
#define PSA_KEY_TYPE_ECC_CURVE_MASK                 ((psa_key_type_t)0x00ff)

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
 * @brief   Elliptic curve public key.
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)              \
    (PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE | (curve))

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
 * @brief Whether a key type is asymmetric: either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) \
    (((type) & 0x4000) == 0x4000)

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
 * @brief Whether a key type is a key pair containing a private part and a public part.
 *
 * @param type  A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_KEY_PAIR(type) \
    (((type) & 0x7000) == 0x7000)

/**
 * @brief Whether a key type is the public part of a key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */

#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) \
    (((type) & 0x7000) == 0x4000)

/**
 * @brief Whether a key type is an RSA key. This includes both key pairs and public keys.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_RSA(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == 0x4001)

/**
 * @brief   Whether a key type is an unstructured array of bytes.
 *          This encompasses both symmetric keys and non-key data.
 *
 * @param   type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) \
    (((type) & 0x7000) == 0x1000 || ((type) & 0x7000) == 0x2000)

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
 * @brief   An invalid key type value.
 *
 * @details Zero is not the encoding of any key type.
 */
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)

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
 * @brief   Permission for the implementation to cache the key.
 *
 * @details This flag allows the implementation to make additional copies of the key material that
 *          are not in storage and not for the purpose of an ongoing operation. Applications can
 *          use it as a hint to keep the key around for repeated access.
 *
 *          An application can request that cached key material is removed from memory by calling
 *          @ref psa_purge_key().
 *
 *          The presence of this usage flag when creating a key is a hint:
 *          - An implementation is not required to cache keys that have this usage flag.
 *          - An implementation must not report an error if it does not cache keys.
 *
 *          If this usage flag is not present, the implementation must ensure key material is
 *          removed from memory as soon as it is not required for an operation or for maintenance
 *          of a volatile key.
 *
 *          This flag must be preserved when reading back the attributes for all keys, regardless
 *          of key type or implementation behavior.
 */
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)

/**
 * @brief   Permission to copy the key.
 *
 * @details This flag allows the use of @ref psa_copy_key() to make a copy of the key with the same
 *          policy or a more restrictive policy.
 *
 *          For lifetimes for which the key is located in a secure element which enforce the
 *          non-exportability of keys, copying a key outside the secure element also requires the
 *          usage flag @ref PSA_KEY_USAGE_EXPORT. Copying the key inside the secure element is
 *          permitted with just @ref PSA_KEY_USAGE_COPY if the secure element supports it. For keys
 *          with the lifetime @ref PSA_KEY_LIFETIME_VOLATILE or @ref PSA_KEY_LIFETIME_PERSISTENT,
 *          the usage flag @ref PSA_KEY_USAGE_COPY is sufficient to permit the copy.
 */
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)

/**
 * @brief   Permission to decrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric decryption operation, for an AEAD
 *          decryption-and-verification operation, or for an asymmetric decryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_decrypt()
 *          - @ref psa_cipher_decrypt_setup()
 *          - @ref psa_aead_decrypt()
 *          - @ref psa_aead_decrypt_setup()
 *          - @ref psa_asymmetric_decrypt()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)

/**
 * @brief   Permission to derive other keys or produce a password hash from this key.
 *
 * @details This flag allows the key to be used for a key derivation operation or for a key
 *          agreement operation, if otherwise permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with the following APIs:
 *          - @ref psa_key_derivation_key_agreement()
 *          - @ref psa_raw_key_agreement()
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_output_bytes() or @ref psa_key_derivation_output_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)

/**
 * @brief   Permission to encrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric encryption operation, for an AEAD
 *          encryption-and-authentication operation, or for an asymmetric encryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_encrypt()
 *          - @ref psa_cipher_encrypt_setup()
 *          - @ref psa_aead_encrypt()
 *          - @ref psa_aead_encrypt_setup()
 *          - @ref psa_asymmetric_encrypt()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)

/**
 * @brief   Permission to export the key.
 *
 * @details This flag allows the use of @ref psa_export_key() to export a key from the
 *          cryptoprocessor. A public key or the public part of a key pair can always be exported
 *          regardless of the value of this permission flag.
 *
 *          This flag can also be required to copy a key using @ref psa_copy_key() outside of a
 *          secure element. See also @ref PSA_KEY_USAGE_COPY.
 *
 *          If a key does not have export permission, implementations must not allow the key to be
 *          exported in plain form from the cryptoprocessor, whether through @ref psa_export_key()
 *          or through a proprietary interface. The key might still be exportable in a wrapped
 *          form, i.e. in a form where it is encrypted by another key.
 */
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)

/**
 * @brief   Permission to sign a message hash with the key.
 *
 * @details This flag allows the key to be used to sign a message hash as part of an asymmetric
 *          signature operation, if otherwise permitted by the key’s type and policy. The flag must
 *          be present on keys used when calling @ref psa_sign_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_SIGN_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_SIGN_HASH when creating a key, then the key always has the
 *          permissions conveyed by @ref PSA_KEY_USAGE_SIGN_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_SIGN_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)

/**
 * @brief   Permission to sign a message with the key.
 *
 * @details This flag allows the key to be used for a MAC calculation operation or for an
 *          asymmetric message signature operation, if otherwise permitted by the key’s type and
 *          policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_compute()
 *          - @ref psa_mac_sign_setup()
 *          - @ref psa_sign_message()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)

/**
 * @brief   Permission to verify the result of a key derivation, including password hashing.
 *
 * @details This flag allows the key to be used in a key derivation operation, if otherwise
 *          permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with @ref psa_key_derivation_verify_key().
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_verify_bytes() or @ref psa_key_derivation_verify_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)

/**
 * @brief   Permission to verify a message hash with the key.
 *
 * @details This flag allows the key to be used to verify a message hash as part of an asymmetric
 *          signature verification operation, if otherwise permitted by the key’s type and policy.
 *          The flag must be present on keys used when calling @ref psa_verify_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_VERIFY_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_VERIFY_HASH when creating a key, then the key always has
 *          the permissions conveyed by @ref PSA_KEY_USAGE_VERIFY_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_VERIFY_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)

/**
 * @brief   Permission to verify a message signature with the key.
 *
 * @details This flag allows the key to be used for a MAC verification operation or for an
 *          asymmetric message signature verification operation, if otherwise permitted by the
 *          key’s type and policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_verify()
 *          - @ref psa_mac_verify_setup()
 *          - @ref psa_verify_message()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)

/**
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement(), for any of the
 *          supported key types and key agreement algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE().
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE \
/* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type A supported key type.
 * @param   key_bits The size of the key in bits.
 *
 * @return  A sufficient output buffer size for the specified key type and size.
 *          0 if key type is not supported.
 *          If the parameters are not valid, the return value is unspecified.
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
/* implementation-defined value */

/**
 * @brief   A sufficient signature buffer size for @ref psa_sign_message() and
 *          @ref psa_sign_hash(), for any of the supported key types and asymmetric signature
 *          algorithms.
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_SIGN_OUTPUT_SIZE().
 */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */

/**
 * @brief   This macro returns the maximum supported length of the PSK for the TLS-1.2 PSK-to-MS
 *          key derivation.
 *
 * @details This implementation-defined value specifies the maximum length for the PSK input used
 *          with a @ref PSA_ALG_TLS12_PSK_TO_MS() key agreement algorithm.
 *
 *          Quoting Pre-Shared Key Ciphersuites for Transport Layer Security (TLS)
 *          [RFC4279](https://tools.ietf.org/html/rfc4279.html) §5.3:
 *          TLS implementations supporting these cipher suites MUST support arbitrary PSK
 *          identities up to 128 octets in length, and arbitrary PSKs up to 64 octets in length.
 *          Supporting longer identities and keys is RECOMMENDED.
 *
 *          Therefore, it is recommended that implementations define
 *          @ref PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE with a value greater than or equal to 64.
 */
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE /* implementation-defined value */

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_CRYPTO_VALUES_H */
/** @} */
