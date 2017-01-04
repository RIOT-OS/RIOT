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
 * @brief       tweetnacl NaCl crypto library tests
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>S
 *
 * @}
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "embUnit.h"
#include "tests-tweetnacl.h"

#include "tweetnacl.h"

void randombytes(uint8_t *target, uint64_t n)
{
    uint32_t random;
    uint8_t *random_pos = 0;
    int _n = 0;
    while (n--) {
        if (_n++ & 0x3) {
            random = random_uint32();

            random_pos = (uint8_t*) &random;
        }
        if (random_pos) {
            uint8_t r_pos = *random_pos;
            *target++ = r_pos++;
        }
        else {
            *target++ = (uint8_t)random_uint32();
        }
    }
}

#define TEST_BUF_SIZE (20)
static char _buf[crypto_box_ZEROBYTES + TEST_BUF_SIZE];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
}

static void tearDown(void)
{
    /* Finalize */
}

const unsigned char* bytes_to_message(const char* bytes, unsigned long long len)
{
    memset(_buf,'\0', crypto_box_ZEROBYTES);
    memcpy(_buf + crypto_box_ZEROBYTES, bytes, len);

    return (const unsigned char*) _buf;
}

struct identity {
    unsigned char pk[crypto_box_PUBLICKEYBYTES];
    unsigned char sk[crypto_box_SECRETKEYBYTES];
};

void identity_create(struct identity *id)
{
    crypto_box_keypair(id->pk,id->sk);
}

static void test_tweetnacl_01(void)
{
    int res;
    const char message[] = "Test String";
    struct identity id_alice;

    /* Creating keypair ALICE... */
    identity_create(&id_alice);

    unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char bob_sk[crypto_box_SECRETKEYBYTES];

    /* Creating keypair BOB... */
    crypto_box_keypair(bob_pk,bob_sk);

    const unsigned char *m = bytes_to_message(message,strlen(message)+1);
    unsigned long long mlen = strlen(message)+crypto_box_ZEROBYTES+1;

    const unsigned char n[crypto_box_NONCEBYTES];
    unsigned char c[mlen];

    /* Encrypting using pk_bob... */
    crypto_box(c, m, mlen, n, bob_pk, id_alice.sk);

    unsigned char result[mlen];
    memset(result, '\0', sizeof(result));

    /* Decrypting... */
    res = crypto_box_open(result,c,mlen,n,id_alice.pk,bob_sk);
    TEST_ASSERT_EQUAL_INT(0, res);
    
    if (res) {
        puts("Decryption failed.\n");
    } else {
        printf("Decryption successful. content: \"%s\"\n", result+crypto_box_ZEROBYTES);
    }
}

Test *tests_tweetnacl_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tweetnacl_01)
    };

    EMB_UNIT_TESTCALLER(tweetnacl_tests, setUp, tearDown, fixtures);
    return (Test*)&tweetnacl_tests;
}

void tests_tweetnacl(void)
{
    TESTS_RUN(tests_tweetnacl_all());
}
