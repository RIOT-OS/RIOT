/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       tweetnacl NaCl crypto library example
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "ps.h"
#include "tweetnacl.h"

static unsigned char alice_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char alice_sk[crypto_box_SECRETKEYBYTES];

static unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char bob_sk[crypto_box_SECRETKEYBYTES];

static const char message[] = "0123456789abcdef";

/* 17 == strlen(message) + 1 for null byte */
#define MLEN (17U + crypto_box_ZEROBYTES)
static unsigned char m[MLEN];
static unsigned char c[MLEN];
static const unsigned char n[crypto_box_NONCEBYTES];

static unsigned char result[MLEN];

int main(void)
{
    int res;

    printf("Creating keypair ALICE...\n");
    crypto_box_keypair(alice_pk, alice_sk);
    printf("Done.\n");

    printf("Creating keypair BOB...\n");
    crypto_box_keypair(bob_pk, bob_sk);
    printf("Done.\n");

    memset(m, 0, crypto_box_ZEROBYTES);
    memcpy(m + crypto_box_ZEROBYTES, message, MLEN - crypto_box_ZEROBYTES);

    printf("Encrypting using pk_bob...\n");
    crypto_box(c, m, MLEN, n, bob_pk, alice_sk);

    memset(result, '\0', sizeof(result));

    printf("Decrypting...\n");
    res = crypto_box_open(result, c, MLEN, n, alice_pk, bob_sk);
    if (res) {
        printf("Decryption failed.\n");
    }
    else {
        printf("Decryption successful. content: \"%s\"\n", result + crypto_box_ZEROBYTES);
    }

    ps();

    return 0;
}
