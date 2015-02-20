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
#include <assert.h>
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
    assert(core_init() == STS_OK);
}

static void tearDown(void)
{
    /* Finalize RELIC */
    core_clean();
}

int tests_relic_ecdh(void)
{
    /*  The following is an example for doing an elliptic-curve Diffie-Hellman 
        key exchange. 
    */

    /* Select an elliptic curve configuration */
    if (ec_param_set_any() == STS_OK) {
#if (TEST_RELIC_SHOW_OUTPUT == 1)
        ec_param_print();
#endif

        bn_t privateA;
        ec_t publicA;
        uint8_t sharedKeyA[MD_LEN];

        bn_t privateB;
        ec_t publicB;
        uint8_t sharedKeyB[MD_LEN];

        bn_null(privateA);
        ec_null(publicA);

        bn_new(privateA);
        ec_new(publicA);

        bn_null(privateB);
        ec_null(publicB);

        bn_new(privateB);
        ec_new(publicB);

        /* User A generates private/public key pair */
        TEST_ASSERT_EQUAL_INT(STS_OK, cp_ecdh_gen(privateA, publicA));

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
        TEST_ASSERT_EQUAL_INT(STS_OK, cp_ecdh_gen(privateB, publicB));

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
        assert(cp_ecdh_key(sharedKeyA, MD_LEN, privateA, publicB) == STS_OK);

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nshared key computed by user A: ");
        print_mem(sharedKeyA, MD_LEN);
#endif

        /* User B calculates shared secret */
        assert(cp_ecdh_key(sharedKeyB, MD_LEN, privateB, publicA) == STS_OK);

#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nshared key computed by user B: ");
        print_mem(sharedKeyB, MD_LEN);
#endif

        /* The secrets should be the same now */
        TEST_ASSERT_EQUAL_INT(CMP_EQ, util_cmp_const(sharedKeyA, sharedKeyB, MD_LEN));

        bn_free(privateA);
        ec_free(publicA);

        bn_free(privateB);
        ec_free(publicB);
#if (TEST_RELIC_SHOW_OUTPUT == 1)
        printf("\nRELIC EC-DH test successful\n");
#endif
    }

    return 0;
}

TestRef tests_relic_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_relic_ecdh)
    };

    EMB_UNIT_TESTCALLER(RELICTest, setUp, tearDown, fixtures);
    return (TestRef)&RELICTest;
}

void tests_relic(void)
{
    TESTS_RUN(tests_relic_all());
}
