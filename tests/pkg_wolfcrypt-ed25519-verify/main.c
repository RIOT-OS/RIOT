/*
 * Copyright (C) 2019 Daniele Lacamera
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       ed25519 signature verification application
 *
 * @author      Daniele Lacamera <daniele@wolfssl.com>
 *
 * @}
 */

#include <stdint.h>
#include <wolfssl/wolfcrypt/ed25519.h>
#include "log.h"

const uint8_t ed_public_key[ED25519_KEY_SIZE] = {
    0x9A, 0xBC, 0xA2, 0xE2, 0x4D, 0x19, 0x9E, 0x02,
    0x72, 0x69, 0x19, 0xE1, 0x05, 0x39, 0x3A, 0x08,
    0xC9, 0x9C, 0xDD, 0xA9, 0x28, 0x45, 0x8B, 0x37,
    0x9E, 0xCA, 0x20, 0x02, 0xBC, 0x7A, 0x2E, 0xC2
};

const uint8_t msg[] = "Hello World!";

const uint8_t msg_signature[ED25519_SIG_SIZE] = {
    0x8A, 0xD3, 0xEA, 0x8F, 0x08, 0x8B, 0x8C, 0x41,
    0x61, 0x36, 0x51, 0xF4, 0x49, 0x6E, 0xD2, 0xCB,
    0xD6, 0xAC, 0x7B, 0x61, 0xD8, 0x3B, 0xE0, 0x1C,
    0xA7, 0xED, 0x72, 0x33, 0xA3, 0xB4, 0xE7, 0xBA,
    0x90, 0x47, 0xC9, 0xAE, 0x48, 0x48, 0x4A, 0x78,
    0x46, 0x63, 0x25, 0x44, 0xA5, 0x82, 0x0F, 0x4A,
    0x9A, 0x5A, 0x0E, 0x3F, 0xF5, 0xB1, 0x97, 0xAD,
    0xB5, 0x48, 0x26, 0x45, 0xD6, 0xC7, 0x00, 0x06
};


int main(void)
{
    int stat;
    int ret;
    ed25519_key key;
    LOG(LOG_INFO, "You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    LOG(LOG_INFO, "This board features a(n) %s MCU.\n", RIOT_MCU);
    wc_ed25519_init(&key);
    LOG(LOG_INFO, "Starting ed25519 test.");

    ret = wc_ed25519_import_public(ed_public_key, ED25519_KEY_SIZE, &key);
    if (ret != 0) {
        LOG(LOG_ERROR, "Error importing public key for signature verification (%d)\n", ret);
        return 1;
    }
    if ((wc_ed25519_verify_msg(msg_signature, ED25519_SIG_SIZE, msg, 12, &stat, &key) < 0) || (stat == 0)) {
        LOG(LOG_WARNING, "The signature is not valid!\n");
    } else {
        LOG(LOG_INFO, "The signature is valid!\n");
    }

    return 0;
}
