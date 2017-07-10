/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * **** This file incorporates work covered by the following copyright and ****
 * **** permission notice:                                                 ****
 * Copyright (C) 2006-2018 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 *
 */
#include <stdint.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/ed25519.h>

void print_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        int p = i % 8;
        if (p == 0)
            printf("\t");
        printf("0x%02X", buf[i]);
        if (i < len - 1)
            printf(",");
        if (p == 7)
            printf("\n");
        else
            printf(" ");
    }
}

void print_key(void *key_in)
{
    uint8_t * key = key_in;
    print_buf(key, ED25519_KEY_SIZE);
}

void print_sig(void *sig_in)
{
    uint8_t * sig = sig_in;
    print_buf(sig, ED25519_SIG_SIZE);
}


int main(void)
{
    uint8_t priv[ED25519_KEY_SIZE], pub[ED25519_KEY_SIZE];
    uint8_t sig[ED25519_SIG_SIZE];
    uint32_t outlen;
    RNG rng;
    ed25519_key key;

    wc_ed25519_init(&key);
    wc_InitRng(&rng);
    if (wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key) != 0) {
        printf("Failed to create ED25519 key!\n");
        exit(1);
    }

    outlen = ED25519_KEY_SIZE;
    wc_ed25519_export_private_only(&key, priv, &outlen);
    printf("const uint8_t ed_private_key[ED25519_KEY_SIZE] = {\n");
    print_key(priv);
    printf("};\n\n");

    outlen = ED25519_KEY_SIZE;
    wc_ed25519_export_public(&key, pub, &outlen);
    printf("const uint8_t ed_public_key[ED25519_KEY_SIZE] = {\n");
    print_key(pub);
    printf("};\n\n");

    printf("const uint8_t msg[] = \"Hello World!\";\n\n");
    printf("const uint8_t msg_signature[ED25519_SIG_SIZE] = {\n");
    outlen = ED25519_SIG_SIZE;
    wc_ed25519_sign_msg("Hello World!", 12, sig, &outlen, &key);
    print_sig(sig);
    printf("};\n\n");
    exit(0);
}
