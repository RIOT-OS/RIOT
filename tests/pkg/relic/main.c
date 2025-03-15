/*
 * Copyright (C) 2014 Tobias Markmann <tm@ayena.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#define TEST_RELIC_SHOW_OUTPUT (0) /**< set if encoded/decoded string is displayed */

#if (TEST_RELIC_SHOW_OUTPUT == 1)
#include <stdio.h>
#endif
#include <stdlib.h>

#include "relic.h"
#include "embUnit.h"

void print_mem(void *mem, int len) {
  int i;
  unsigned char *p = (unsigned char *)mem;
  for (i=0;i<len;i++) {
    printf("0x%02x ", p[i]);
  }
  printf("\n");
}

static void setUp(void)
{
    /* Initialize RELIC */
    TEST_ASSERT_EQUAL_INT(RLC_OK, core_init());

    pc_param_set_any();
}

static void tearDown(void)
{
    /* Finalize RELIC */
    core_clean();
}

static void tests_relic_ecdh(void)
{
    /*  The following is an example for doing an elliptic-curve Diffie-Hellman
        key exchange.
    */

    /* Select an elliptic curve configuration */
    if (ec_param_set_any() == RLC_OK) {
#if (TEST_RELIC_SHOW_OUTPUT == 1)
        ec_param_print();
#endif

        bn_t privateA;
        ec_t publicA;
        uint8_t sharedKeyA[RLC_MD_LEN];

        bn_t privateB;
        ec_t publicB;
        uint8_t sharedKeyB[RLC_MD_LEN];

        bn_null(privateA);
        ec_null(publicA);

        bn_new(privateA);
        ec_new(publicA);

        bn_null(privateB);
        ec_null(publicB);

        bn_new(privateB);
        ec_new(publicB);

        /* User A generates private/public key pair */
        TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ecdh_gen(privateA, publicA));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("User A\n");
        printf("======\n");
        printf("private key: ");
        bn_print(privateA);
        printf("\npublic key:  ");
        ec_print(publicA);
        printf("\n");
#endif

        /* User B generates private/public key pair */
        TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ecdh_gen(privateB, publicB));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("User B\n");
        printf("======\n");
        printf("private key: ");
        bn_print(privateB);
        printf("\npublic key:  ");
        ec_print(publicB);
        printf("\n");
#endif

        /* In a protocol you would exchange the public keys now */

        /* User A calculates shared secret */
        TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ecdh_key(sharedKeyA, RLC_MD_LEN, privateA, publicB));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nshared key computed by user A: ");
        print_mem(sharedKeyA, RLC_MD_LEN);
#endif

        /* User B calculates shared secret */
        TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ecdh_key(sharedKeyB, RLC_MD_LEN, privateB, publicA));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nshared key computed by user B: ");
        print_mem(sharedKeyB, RLC_MD_LEN);
#endif

        /* The secrets should be the same now */
        TEST_ASSERT_EQUAL_INT(RLC_EQ, util_cmp_sec(sharedKeyA, sharedKeyB, RLC_MD_LEN));

        bn_free(privateA);
        ec_free(publicA);

        bn_free(privateB);
        ec_free(publicB);
#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nRELIC EC-DH test successful\n");
#endif
    }

}

static void tests_relic_paillier_encryption(void)
{
    /*  The following is an example for doing Paillier key pair generation
        and simple data encryption.
    */
    bn_t priv, pub, plain, enc, dec;

    bn_null(priv);
    bn_new(priv);

    bn_null(pub);
    bn_new(pub);

    bn_null(plain);
    bn_new(plain);

    bn_null(enc);
    bn_new(enc);

    bn_null(dec);
    bn_new(dec);

    TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ghpe_gen(pub, priv, 512));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
    printf("RELIC Paillier - key pair generation successeful\n");
    printf("private key: ");
    bn_print(priv);
    printf("\n");
    printf("public key:  ");
    bn_print(pub);
    printf("\n");
#endif

    bn_rand(plain, RLC_POS, 510);

    TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ghpe_enc(enc, plain, pub, 1));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
    printf("RELIC Paillier -  data encryption successful\n");
    printf("plain text: ");
    bn_print(plain);
    printf("\n");
    printf("encrypted data:  ");
    bn_print(enc);
    printf("\n");
#endif

    TEST_ASSERT_EQUAL_INT(RLC_OK, cp_ghpe_dec(dec, enc, pub, priv, 1));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
    printf("decrypted data: ");
    bn_print(dec);
    printf("\n");
#endif

    TEST_ASSERT_EQUAL_INT(RLC_EQ, bn_cmp(plain, dec));

#if (TEST_RELIC_SHOW_OUTPUT == 1)
    printf("\nRELIC Paillier encryption test successful\n");
#endif

    bn_free(pub);
    bn_free(priv);
    bn_free(plain);
    bn_free(enc);
    bn_free(dec);
}

TestRef tests_relic(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_relic_ecdh),
        new_TestFixture(tests_relic_paillier_encryption)
    };

    EMB_UNIT_TESTCALLER(RELICTest, setUp, tearDown, fixtures);
    return (TestRef)&RELICTest;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_relic());
    TESTS_END();

    return 0;
}
