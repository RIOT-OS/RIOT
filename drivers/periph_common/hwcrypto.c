/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Shared hardware crypto code
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "periph/hwcrypto.h"

#ifndef PERIPH_HWCRYPTO_PROVIDES_CIPHER_SUPPORTED
/**
 * @brief Bitmap of supported ciphers.
 */
static const uint32_t cipher_bitmap = (
#ifdef HAVE_HWCRYPTO_AES128
    (1 << HWCRYPTO_AES128) +
#endif
#ifdef HAVE_HWCRYPTO_AES192
    (1 << HWCRYPTO_AES192) +
#endif
#ifdef HAVE_HWCRYPTO_AES256
    (1 << HWCRYPTO_AES256) +
#endif
#ifdef HAVE_HWCRYPTO_DES
    (1 << HWCRYPTO_DES) +
#endif
#ifdef HAVE_HWCRYPTO_3DES
    (1 << HWCRYPTO_3DES) +
#endif
#ifdef HAVE_HWCRYPTO_TWOFISH
    (1 << HWCRYPTO_TWOFISH) +
#endif
#ifdef HAVE_HWCRYPTO_RSA512
    (1 << HWCRYPTO_RSA512) +
#endif
#ifdef HAVE_HWCRYPTO_RSA1024
    (1 << HWCRYPTO_RSA1024) +
#endif
#ifdef HAVE_HWCRYPTO_RSA2048
    (1 << HWCRYPTO_RSA2048) +
#endif
#ifdef HAVE_HWCRYPTO_RSA4096
    (1 << HWCRYPTO_RSA4096) +
#endif
#ifdef HAVE_HWCRYPTO_ECC128
    (1 << HWCRYPTO_ECC128) +
#endif
#ifdef HAVE_HWCRYPTO_ECC192
    (1 << HWCRYPTO_ECC192) +
#endif
#ifdef HAVE_HWCRYPTO_ECC256
    (1 << HWCRYPTO_ECC256) +
#endif
    0);

inline bool hwcrypto_cipher_supported(hwcrypto_t dev, hwcrypto_cipher_t cipher)
{
    (void) dev;

    return cipher_bitmap & (1 << cipher);
}
#endif /* PERIPH_HWCRYPTO_PROVIDES_CIPHER_SUPPORTED */

#ifndef PERIPH_HWCRYPTO_PROVIDES_HASH_SUPPORTED
/**
 * @brief Bitmap of supported hashes.
 */
static const uint32_t hash_bitmap = (
#ifdef HAVE_HWCRYPTO_MD4
    (1 << HWCRYPTO_MD4) +
#endif
#ifdef HAVE_HWCRYPTO_MD5
    (1 << HWCRYPTO_MD5) +
#endif
#ifdef HAVE_HWCRYPTO_SHA1
    (1 << HWCRYPTO_SHA1) +
#endif
#ifdef HAVE_HWCRYPTO_SHA3
    (1 << HWCRYPTO_SHA3) +
#endif
#ifdef HAVE_HWCRYPTO_SHA224
    (1 << HWCRYPTO_SHA224) +
#endif
#ifdef HAVE_HWCRYPTO_SHA256
    (1 << HWCRYPTO_SHA256) +
#endif
#ifdef HAVE_HWCRYPTO_SHA384
    (1 << HWCRYPTO_SHA384) +
#endif
#ifdef HAVE_HWCRYPTO_SHA512
    (1 << HWCRYPTO_SHA512) +
#endif
    0);

inline bool hwcrypto_hash_supported(hwcrypto_t dev, hwcrypto_hash_t hash)
{
    (void) dev;

    return hash_bitmap & (1 << hash);
}
#endif /* PERIPH_HWCRYPTO_PROVIDES_HASH_SUPPORTED */
