/*
 * Copyright (C) 2019 Daniele Lacamera
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */
#include <stdint.h>
#include <stdio.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/ed25519.h>

#include "od.h"

void print_key(void *key_in)
{
    uint8_t * key = key_in;
    od_hex_dump(key, ED25519_KEY_SIZE, 8);
}

void print_sig(void *sig_in)
{
    uint8_t * sig = sig_in;
    od_hex_dump(sig, ED25519_SIG_SIZE, 8);
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
        LOG(LOG_ERROR, "Failed to create ED25519 key!\n");
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
