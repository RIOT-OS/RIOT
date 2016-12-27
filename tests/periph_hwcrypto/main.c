/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for GPIO peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/hwcrypto.h"

/**
 * @brief   Cipher/hash context.
 */
static uint8_t context[256] __attribute__((aligned));

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
 * @brief   Dummy padding.
 */
//static uint8_t padding[32] __attribute__((aligned));

/**
 * @brief   Dummy counter vector (don't care about actual initialization
 *          vector).
 */
static uint8_t counter[32] __attribute__((aligned));

#if defined(HAVE_HWCRYPTO_AES128) || defined(HAVE_HWCRYPTO_AES256)
static void test_cipher(char* name, hwcrypto_cipher_t cipher, hwcrypto_mode_t mode, uint8_t key_size, uint8_t block_size)
{
    int result;

    printf("Testing %s\n", name);

    /* initialize hwcrypto module */
    result = hwcrypto_init();

    if (result != 0) {
        printf("Unable to init: %d\n", result);
        return;
    }

    /* turn it on */
    hwcrypto_poweron();

    /* acquire hwcrypto module */
    result = hwcrypto_acquire();

    if (result != 0) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release();
        
        return;
    }

    /* initialize cipher */
    result = hwcrypto_cipher_init((hwcrypto_cipher_context_t*) context, cipher, mode);

    if (result != 0) {
        printf("Unable to init cipher: %d\n", result);
        hwcrypto_release();


        return;
    }

    result = hwcrypto_cipher_set((hwcrypto_cipher_context_t*) context, HWCRYPTO_OPT_KEY, key, key_size);

    if (result != 0) {
        printf("Unable to set key: %d\n", result);
        hwcrypto_release();

        return;
    }

    if (mode == HWCRYPTO_MODE_CBC || mode == HWCRYPTO_MODE_CFB || mode == HWCRYPTO_MODE_OFB) {
        result = hwcrypto_cipher_set((hwcrypto_cipher_context_t*) context, HWCRYPTO_OPT_IV, iv, 16);

        if (result != 0) {
            printf("Unable to set iv: %d\n", result);
            hwcrypto_release();

            return;
        }
    }

    if (mode == HWCRYPTO_MODE_CTR) {
        result = hwcrypto_cipher_set((hwcrypto_cipher_context_t*) context, HWCRYPTO_OPT_COUNTER, counter, 16);

        if (result != 0) {
            printf("Unable to set counter: %d\n", result);
            hwcrypto_release();

            return;
        }
    }

    /* test encryption */
    result = hwcrypto_cipher_encrypt((hwcrypto_cipher_context_t*) context, data, data, block_size);

    if (result != block_size) {
        printf("Unable to encrypt: %d\n", result);
        hwcrypto_release();

        return;
    }

    /* test decryption */
    result = hwcrypto_cipher_decrypt((hwcrypto_cipher_context_t*) context, data, data, block_size);

    if (result != block_size) {
        printf("Unable to decrypt: %d\n", result);
        hwcrypto_release();

        return;
    }

    /* release it */
    hwcrypto_release();

    /* power it off */
    hwcrypto_poweroff();
}
#endif

#if defined(HAVE_HWCRYPTO_SHA1) || defined(HAVE_HWCRYPTO_SHA256)
static void test_hash(char* name, hwcrypto_hash_t hash, uint8_t block_size)
{
    int result;

    printf("Testing %s\n", name);

    /* initialize hwcrypto module */
    result = hwcrypto_init();

    if (result != 0) {
        printf("Unable to init: %d\n", result);
        return;
    }

    /* turn it on */
    hwcrypto_poweron();

    /* acquire hwcrypto module */
    result = hwcrypto_acquire();

    if (result != 0) {
        printf("Unable to acquire: %d\n", result);
        hwcrypto_release();

        return;
    }

    /* initialize hash */
    result = hwcrypto_hash_init((hwcrypto_hash_context_t*) context, hash);

    if (result != 0) {
        printf("Unable to init hash: %d\n", result);
        hwcrypto_release();

        return;
    }

    /* test hashing */
    result = hwcrypto_hash_update((hwcrypto_hash_context_t*) context, data, block_size);

    if (result != block_size) {
        printf("Unable to hash: %d\n", result);
        hwcrypto_release();

        return;
    }

    result = hwcrypto_hash_final((hwcrypto_hash_context_t*) context, data, block_size);

    if (result != block_size) {
        printf("Unable to finalize hash: %d\n", result);
        hwcrypto_release();

        return;
    }

    /* release it */
    hwcrypto_release();

    /* power it off */
    hwcrypto_poweroff();
}
#endif

int main(void)
{
    puts("HWCRYPTO peripheral driver test\n");
    puts("In this test, the supported cipher and hash functions will be\n"
         "tested. It will test the implementation only, not the results.");

    /* AES-128 tests */
#ifdef HAVE_HWCRYPTO_AES128
    test_cipher("AES-128 ECB", HWCRYPTO_AES128, HWCRYPTO_MODE_ECB, 16, 16);
    test_cipher("AES-128 CBC", HWCRYPTO_AES128, HWCRYPTO_MODE_CBC, 16, 16);
    test_cipher("AES-128 CFB", HWCRYPTO_AES128, HWCRYPTO_MODE_CFB, 16, 16);
    test_cipher("AES-128 OFB", HWCRYPTO_AES128, HWCRYPTO_MODE_OFB, 16, 16);
    test_cipher("AES-128 CTR", HWCRYPTO_AES128, HWCRYPTO_MODE_CTR, 16, 16);
#endif

    /* AES-256 tests */
#ifdef HAVE_HWCRYPTO_AES256
    test_cipher("AES-256 ECB", HWCRYPTO_AES256, HWCRYPTO_MODE_ECB, 32, 16);
    test_cipher("AES-256 CBC", HWCRYPTO_AES256, HWCRYPTO_MODE_CBC, 32, 16);
    test_cipher("AES-256 CFB", HWCRYPTO_AES256, HWCRYPTO_MODE_CFB, 32, 16);
    test_cipher("AES-256 OFB", HWCRYPTO_AES256, HWCRYPTO_MODE_OFB, 32, 16);
    test_cipher("AES-256 CTR", HWCRYPTO_AES256, HWCRYPTO_MODE_CTR, 32, 16);
#endif

    /* SHA-1 tests */
#ifdef HAVE_HWCRYPTO_SHA1
    test_hash("SHA-1", HWCRYPTO_SHA1, 20);
#endif

    /* SHA-256 tests */
#ifdef HAVE_HWCRYPTO_SHA256
    test_hash("SHA-256", HWCRYPTO_SHA256, 32);
#endif

    puts("Testing done!\n");

    return 0;
}
