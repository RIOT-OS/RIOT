/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for at86rf2xx security module (AES)
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "at86rf2xx_aes.h"

#ifndef KEY_1
#define KEY_1       { 0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b,   \
                      0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06 }
#endif
#ifndef IV_1
#define IV_1        { 0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30,   \
                      0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41 }
#endif
#ifndef PLAIN_1
#define PLAIN_1     { 'S', 'i', 'n', 'g', 'l', 'e', ' ', 'b',           \
                      'l', 'o', 'c', 'k', ' ', 'm', 's', 'g' }
#endif
#ifndef KEY_2
#define KEY_2       { 0xc2, 0x86, 0x69, 0x6d, 0x88, 0x7c, 0x9a, 0xa0,   \
                      0x61, 0x1b, 0xbb, 0x3e, 0x20, 0x25, 0xa4, 0x5a }
#endif
#ifndef IV_2
#define IV_2        { 0x56, 0x2e, 0x17, 0x99, 0x6d, 0x09, 0x3d, 0x28,   \
                      0xdd, 0xb3, 0xba, 0x69, 0x5a, 0x2e, 0x6f, 0x58 }
#endif
#ifndef PLAIN_2
#define PLAIN_2     { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,   \
                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,   \
                      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,   \
                      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f }
#endif
#ifndef KEY_3
#define KEY_3       { 0x6c, 0x3e, 0xa0, 0x47, 0x76, 0x30, 0xce, 0x21,   \
                      0xa2, 0xce, 0x33, 0x4a, 0xa7, 0x46, 0xc2, 0xcd }
#endif
#ifndef IV_3
#define IV_3        { 0xc7, 0x82, 0xdc, 0x4c, 0x09, 0x8c, 0x66, 0xcb,   \
                      0xd9, 0xcd, 0x27, 0xd8, 0x25, 0x68, 0x2c, 0x81 }
#endif
#ifndef PLAIN_3
#define PLAIN_3     { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ',           \
                      'a', ' ', '4', '8', '-', 'b', 'y', 't',           \
                      'e', ' ', 'm', 'e', 's', 's', 'a', 'g',           \
                      'e', ' ', '(', 'e', 'x', 'a', 'c', 't',           \
                      'l', 'y', ' ', '3', ' ', 'A', 'E', 'S',           \
                      ' ', 'b', 'l', 'o', 'c', 'k', 's', ')' }
#endif
#ifndef KEY_4
#define KEY_4       { 0x56, 0xe4, 0x7a, 0x38, 0xc5, 0x59, 0x89, 0x74,   \
                      0xbc, 0x46, 0x90, 0x3d, 0xba, 0x29, 0x03, 0x49 }
#endif
#ifndef IV_4
#define IV_4        { 0x8c, 0xe8, 0x2e, 0xef, 0xbe, 0xa0, 0xda, 0x3c,   \
                      0x44, 0x69, 0x9e, 0xd7, 0xdb, 0x51, 0xb7, 0xd9 }
#endif
#ifndef PLAIN_4
#define PLAIN_4     { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,   \
                      0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,   \
                      0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,   \
                      0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,   \
                      0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,   \
                      0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,   \
                      0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,   \
                      0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf }
#endif
#ifndef KEY_5
#define KEY_5       { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,   \
                      0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf }
#endif
#ifndef IV_5
#define IV_5        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   \
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#endif
#ifndef PLAIN_5
#define PLAIN_5     { 0x59, 0xac, 0xde, 0x48, 0x00, 0x00, 0x00, 0x00,   \
                      0x01, 0x00, 0x00, 0x00, 0x05, 0x06, 0x00, 0x01,   \
                      0x00, 0x1d, 0x2b, 0xdc, 0x84, 0x21, 0x43, 0x02,   \
                      0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac, 0xff,   \
                      0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde,   \
                      0xac, 0x06, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00,   \
                      0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   \
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#endif

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    /* Ignore interrupts */
    (void)dev;
    (void)event;
}

static void _ecb(at86rf2xx_t *dev,
                 aes_block_t *plain,
                 uint8_t key[AT86RF2XX_AES_KEY_LENGTH],
                 aes_block_t *cipher,
                 uint8_t nblocks)
{
    at86rf2xx_aes_key_write_encrypt(dev, key);
    at86rf2xx_aes_ecb_encrypt(dev, cipher, key, (void*)plain, nblocks);
    memset(plain, 0, AT86RF2XX_AES_BLOCK_SIZE * nblocks);
    at86rf2xx_aes_key_write_decrypt(dev, key);
    at86rf2xx_aes_ecb_decrypt(dev, plain, key, (void*)cipher, nblocks);
}

static void _cbc(at86rf2xx_t *dev,
                 aes_block_t *plain,
                 uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                 uint8_t iv[AT86RF2XX_AES_BLOCK_SIZE],
                 aes_block_t *cipher,
                 uint8_t nblocks)
{
    at86rf2xx_aes_key_write_encrypt(dev, key);
    at86rf2xx_aes_cbc_encrypt(dev, cipher, key, iv, (void*)plain, nblocks);
    memset(plain, 0, AT86RF2XX_AES_BLOCK_SIZE * nblocks);
    at86rf2xx_aes_key_write_decrypt(dev, key);
    at86rf2xx_aes_cbc_decrypt(dev, plain, key, iv, (void*)cipher, nblocks);
}

int main(void)
{
    at86rf2xx_t dev;
    bool success = true;
    at86rf2xx_setup(&dev, &at86rf2xx_params[0], 0);
    dev.netdev.netdev.event_callback = _event_cb;
    if (dev.netdev.netdev.driver->init(&dev.netdev.netdev) != 0) {
        return EXIT_FAILURE;
    }

    puts("START");
    {
        uint8_t k[] = KEY_1;
        uint8_t p[] = PLAIN_1;
        uint8_t c[sizeof(p)];
        uint8_t n = sizeof(p) / AT86RF2XX_AES_BLOCK_SIZE;

        _ecb(&dev, (aes_block_t *)p, k, (aes_block_t *)c, n);

        if (!memcmp(p, (uint8_t[])PLAIN_1, sizeof(p))) {
            puts("ECB test 1 done");
        }
        else {
            success = false;
            puts("ECB test 1 failed");
        }
    }
    {
        uint8_t k[] = KEY_2;
        uint8_t p[] = PLAIN_2;
        uint8_t c[sizeof(p)];
        uint8_t n = sizeof(p) / AT86RF2XX_AES_BLOCK_SIZE;

         _ecb(&dev, (aes_block_t *)p, k, (aes_block_t *)c, n);

        if (!memcmp(p, (uint8_t[])PLAIN_2, sizeof(p))) {
            puts("ECB test 2 done");
        }
        else {
            success = false;
            puts("ECB test 2 failed");
        }
    }
    {
        uint8_t k[] = KEY_3;
        uint8_t v[] = IV_3;
        uint8_t p[] = PLAIN_3;
        uint8_t c[sizeof(p)];
        uint8_t n = sizeof(p) / AT86RF2XX_AES_BLOCK_SIZE;

        _cbc(&dev, (aes_block_t *)p, k, v, (aes_block_t *)c, n);

        if (!memcmp(p, (uint8_t[])PLAIN_3, sizeof(p))) {
            puts("CBC test 1 done");
        }
        else {
            success = false;
            puts("CBC test 1 failed");
        }
    }
    {
        uint8_t k[] = KEY_4;
        uint8_t v[] = IV_4;
        uint8_t p[] = PLAIN_4;
        uint8_t c[sizeof(p)];
        uint8_t n = sizeof(p) / AT86RF2XX_AES_BLOCK_SIZE;

        _cbc(&dev, (aes_block_t *)p, k, v, (aes_block_t *)c, n);

        if (!memcmp(p, (uint8_t[])PLAIN_4, sizeof(p))) {
            puts("CBC test 2 done");
        }
        else {
            success = false;
            puts("CBC test 2 failed");
        }
    }
    {
        uint8_t k[] = KEY_5;
        uint8_t v[] = IV_5;
        uint8_t p[] = PLAIN_5;
        uint8_t c[sizeof(p)];
        uint8_t n = sizeof(p) / AT86RF2XX_AES_BLOCK_SIZE;

        _cbc(&dev, (aes_block_t *)p, k, v, (aes_block_t *)c, n);

        if (!memcmp(p, (uint8_t[])PLAIN_5, sizeof(p))) {
            puts("CBC test 3 done");
        }
        else {
            success = false;
            puts("CBC test 3 failed");
        }
    }
    if (success) {
        puts("SUCCESS");
    }
    return EXIT_SUCCESS;
}
