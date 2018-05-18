/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Hardware crypto device tests.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/hwcrypto.h"

/**
 * @brief   Dummy data buffer.
 */
static uint8_t data[64] __attribute__((aligned));

/**
 * @brief   Dummy key.
 */
static uint8_t key[32] __attribute__((aligned));

/**
 * @brief   Dummy initialization vector.
 */
static uint8_t iv[32] __attribute__((aligned));

/**
 * @brief   Dummy counter vector.
 */
static uint8_t counter[32] __attribute__((aligned));

static void printf_buffer(uint8_t *buf, size_t len, char *label)
{
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");
}

#if defined(HAVE_HWCRYPTO_AES128) || defined(HAVE_HWCRYPTO_AES256)
static void test_cipher(hwcrypto_t dev, char *name, hwcrypto_cipher_t cipher, hwcrypto_mode_t mode, uint8_t key_size, uint8_t block_size)
{
    int result;

    printf("Testing %s\n", name);

    /* check if hash is supported */
    if (!hwcrypto_cipher_supported(dev, cipher)) {
        puts("Cipher not supported.");
        return;
    }

    /* acquire hwcrypto module */
    result = hwcrypto_acquire(dev);

    if (result != HWCRYPTO_OK) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* initialize cipher */
    result = hwcrypto_cipher_init(dev, cipher, mode);

    if (result != HWCRYPTO_OK) {
        printf("Unable to init cipher: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_KEY, key, key_size);

    if (result != HWCRYPTO_OK) {
        printf("Unable to set key: %d\n", result);
        hwcrypto_release(dev);

        return;
    }
    printf_buffer(key, key_size, "key");

    if (mode == HWCRYPTO_MODE_CBC || mode == HWCRYPTO_MODE_CFB || mode == HWCRYPTO_MODE_OFB) {
        result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_IV, iv, 16);

        if (result != HWCRYPTO_OK) {
            printf("Unable to set iv: %d\n", result);
            hwcrypto_release(dev);

            return;
        }
        printf_buffer(iv, 16, "IV");
    }
    else if (mode == HWCRYPTO_MODE_CTR) {
        result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_COUNTER, counter, 16);

        if (result != HWCRYPTO_OK) {
            printf("Unable to set counter: %d\n", result);
            hwcrypto_release(dev);

            return;
        }
        printf_buffer(counter, 16, "Counter");
    }

    /* test encryption */
    printf_buffer(data, block_size, "Plain data");
    result = hwcrypto_cipher_encrypt(dev, data, data, block_size);

    if (result != block_size) {
        printf("Unable to encrypt: %d\n", result);
        hwcrypto_release(dev);

        return;
    }
    printf_buffer(data, block_size, "Encrypted data");

    /* test decryption */
    result = hwcrypto_cipher_decrypt(dev, data, data, block_size);

    if (result != block_size) {
        printf("Unable to decrypt: %d\n", result);
        hwcrypto_release(dev);

        return;
    }
    printf_buffer(data, block_size, "Decrypted data");

    /* release it */
    hwcrypto_release(dev);
}
#endif

#if defined(HAVE_HWCRYPTO_SHA1) || defined(HAVE_HWCRYPTO_SHA256)
static void test_hash(hwcrypto_t dev, char *name, hwcrypto_hash_t hash, uint8_t block_size)
{
    int result;

    printf("Testing %s\n", name);

    /* check if hash is supported */
    if (!hwcrypto_hash_supported(dev, hash)) {
        puts("Hash not supported.");
        return;
    }

    /* acquire hwcrypto module */
    result = hwcrypto_acquire(dev);

    if (result != HWCRYPTO_OK) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* initialize hash */
    result = hwcrypto_hash_init(dev, hash);

    if (result != HWCRYPTO_OK) {
        printf("Unable to init hash: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* test hashing */
    result = hwcrypto_hash_update(dev, data, block_size);

    if (result != block_size) {
        printf("Unable to hash: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    result = hwcrypto_hash_final(dev, data, block_size);

    if (result != block_size) {
        printf("Unable to finalize hash: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* release it */
    hwcrypto_release(dev);
}
#endif

int main(void)
{
    puts("HWCRYPTO peripheral driver test\n");
    puts("In this test, the supported cipher and hash functions will be "
         "tested on each hardware crypto peripheral. It will test the "
         "implementation only, not the results.\n");

    for (unsigned i = 0; i < sizeof(key); i++) {
        key[i] = i;
    }
    for (unsigned i = 0; i < sizeof(iv); i++) {
        iv[i] = i;
    }
    for (unsigned i = 0; i < sizeof(data); i++) {
        data[i] = sizeof(data) - i;
    }
    for (unsigned i = 0; i < sizeof(counter); i++) {
        counter[i] = 0;
    }

    /* run a test for every hardware crypto peripheral */
    for (int i = 0; i < (int) HWCRYPTO_NUMOF; i++) {
        hwcrypto_t dev = HWCRYPTO_DEV(i);

#ifdef HAVE_HWCRYPTO_AES128
        /* AES-128 tests */
        test_cipher(dev, "AES-128 ECB", HWCRYPTO_AES128, HWCRYPTO_MODE_ECB, 16, 16);
        test_cipher(dev, "AES-128 CBC", HWCRYPTO_AES128, HWCRYPTO_MODE_CBC, 16, 16);
        test_cipher(dev, "AES-128 CFB", HWCRYPTO_AES128, HWCRYPTO_MODE_CFB, 16, 16);
        test_cipher(dev, "AES-128 OFB", HWCRYPTO_AES128, HWCRYPTO_MODE_OFB, 16, 16);
        test_cipher(dev, "AES-128 CTR", HWCRYPTO_AES128, HWCRYPTO_MODE_CTR, 16, 16);
#endif

#ifdef HAVE_HWCRYPTO_AES256
        /* AES-256 tests */
        test_cipher(dev, "AES-256 ECB", HWCRYPTO_AES256, HWCRYPTO_MODE_ECB, 32, 16);
        test_cipher(dev, "AES-256 CBC", HWCRYPTO_AES256, HWCRYPTO_MODE_CBC, 32, 16);
        test_cipher(dev, "AES-256 CFB", HWCRYPTO_AES256, HWCRYPTO_MODE_CFB, 32, 16);
        test_cipher(dev, "AES-256 OFB", HWCRYPTO_AES256, HWCRYPTO_MODE_OFB, 32, 16);
        test_cipher(dev, "AES-256 CTR", HWCRYPTO_AES256, HWCRYPTO_MODE_CTR, 32, 16);
#endif

#ifdef HAVE_HWCRYPTO_SHA1
        /* SHA-1 tests */
        test_hash(dev, "SHA-1", HWCRYPTO_SHA1, 20);
#endif

#ifdef HAVE_HWCRYPTO_SHA224
        /* SHA-224 tests */
        test_hash(dev, "SHA-224", HWCRYPTO_SHA224, 28);
#endif

#ifdef HAVE_HWCRYPTO_SHA256
        /* SHA-256 tests */
        test_hash(dev, "SHA-256", HWCRYPTO_SHA256, 32);
#endif

#ifdef HAVE_HWCRYPTO_SHA384
        /* SHA-384 tests */
        test_hash(dev, "SHA-384", HWCRYPTO_SHA384, 48);
#endif

#ifdef HAVE_HWCRYPTO_SHA512
        /* SHA-512 tests */
        test_hash(dev, "SHA-512", HWCRYPTO_SHA512, 64);
#endif
    }

    puts("Testing done!\n");

    return 0;
}
