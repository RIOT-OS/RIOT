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
static uint8_t data[32] __attribute__((aligned));

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

    /* initialize hwcrypto module */
    result = hwcrypto_init(dev);

    if (result != 0) {
        printf("Unable to init: %d\n", result);
        return;
    }

    /* turn it on */
    hwcrypto_poweron(dev);

    /* acquire hwcrypto module */
    result = hwcrypto_acquire(dev);

    if (result != 0) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* initialize cipher */
    result = hwcrypto_cipher_init(dev, cipher, mode);

    if (result != 0) {
        printf("Unable to init cipher: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_KEY, key, key_size);

    if (result != 0) {
        printf("Unable to set key: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    if (mode == HWCRYPTO_MODE_CBC || mode == HWCRYPTO_MODE_CFB || mode == HWCRYPTO_MODE_OFB) {
        result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_IV, iv, 16);

        if (result != 0) {
            printf("Unable to set iv: %d\n", result);
            hwcrypto_release(dev);

            return;
        }
    }
    else if (mode == HWCRYPTO_MODE_CTR) {
        result = hwcrypto_cipher_set(dev, HWCRYPTO_OPT_COUNTER, counter, 16);

        if (result != 0) {
            printf("Unable to set counter: %d\n", result);
            hwcrypto_release(dev);

            return;
        }
    }

    /* test encryption */
    result = hwcrypto_cipher_encrypt(dev, data, data, block_size);

    if (result != block_size) {
        printf("Unable to encrypt: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* test decryption */
    result = hwcrypto_cipher_decrypt(dev, data, data, block_size);

    if (result != block_size) {
        printf("Unable to decrypt: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* release it */
    hwcrypto_release(dev);

    /* power it off */
    hwcrypto_poweroff(dev);
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

    /* initialize hwcrypto module */
    result = hwcrypto_init(dev);

    if (result != 0) {
        printf("Unable to init: %d\n", result);
        return;
    }

    /* turn it on */
    hwcrypto_poweron(dev);

    /* acquire hwcrypto module */
    result = hwcrypto_acquire(dev);

    if (result != 0) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release(dev);

        return;
    }

    /* initialize hash */
    result = hwcrypto_hash_init(dev, hash);

    if (result != 0) {
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

    /* power it off */
    hwcrypto_poweroff(dev);
}
#endif

int main(void)
{
    puts("HWCRYPTO peripheral driver test\n");
    puts("In this test, the supported cipher and hash functions will be "
         "tested on each hardware crypto peripheral. It will test the "
         "implementation only, not the results.\n");

    /* run a test for every hardware crypto peripheral */
    for (int i = 0; i < (int) HWCRYPTO_NUMOF; i++) {
        hwcrypto_t dev = (hwcrypto_t) i;

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

#ifdef HAVE_HWCRYPTO_SHA256
        /* SHA-256 tests */
        test_hash(dev, "SHA-256", HWCRYPTO_SHA256, 32);
#endif
    }

    puts("Testing done!\n");

    return 0;
}
