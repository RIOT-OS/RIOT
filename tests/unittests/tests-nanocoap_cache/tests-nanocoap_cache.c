/*
 * Copyright (c) 2020 Cenk Gündoğan
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "embUnit.h"

#include "net/nanocoap/cache.h"
#include "ztimer.h"
#include "hashes/sha256.h"

#include "unittests-constants.h"
#include "tests-nanocoap_cache.h"
#define _BUF_SIZE (128U)

static void test_nanocoap_cache__cachekey(void)
{
    uint8_t digest1[SHA256_DIGEST_LENGTH];
    uint8_t digest2[SHA256_DIGEST_LENGTH];
    uint8_t buf1[_BUF_SIZE];
    uint8_t buf2[_BUF_SIZE];
    coap_pkt_t pkt1;
    coap_pkt_t pkt2;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/time";
    char path2[] = "/time2";
    size_t len;

    /* 1. packet */
    len = coap_build_hdr((coap_hdr_t *)&buf1[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&pkt1, &buf1[0], sizeof(buf1), len);
    coap_opt_add_string(&pkt1, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_finish(&pkt1, COAP_OPT_FINISH_NONE);

    /* 2. packet */
    len = coap_build_hdr((coap_hdr_t *)&buf2[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&pkt2, &buf2[0], sizeof(buf2), len);
    coap_opt_add_string(&pkt2, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_finish(&pkt2, COAP_OPT_FINISH_NONE);

    nanocoap_cache_key_generate((const coap_pkt_t *) &pkt1, digest1);
    nanocoap_cache_key_generate((const coap_pkt_t *) &pkt2, digest2);

    /* compare 1. and 2. packet */
    TEST_ASSERT_EQUAL_INT(0, nanocoap_cache_key_compare(digest1, digest2));

    /* 3. packet */
    len = coap_build_hdr((coap_hdr_t *)&buf2[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&pkt2, &buf2[0], sizeof(buf2), len);
    coap_opt_add_string(&pkt2, COAP_OPT_URI_PATH, &path2[0], '/');
    coap_opt_finish(&pkt2, COAP_OPT_FINISH_NONE);

    nanocoap_cache_key_generate((const coap_pkt_t *) &pkt2, digest2);

    /* compare 1. and 3. packet */
    TEST_ASSERT(nanocoap_cache_key_compare(digest1, digest2) < 0);
    /* compare 3. and 1. packet */
    TEST_ASSERT(nanocoap_cache_key_compare(digest2, digest1) > 0);
}

static void test_nanocoap_cache__cachekey_blockwise(void)
{
    uint8_t digest1[SHA256_DIGEST_LENGTH];
    uint8_t digest2[SHA256_DIGEST_LENGTH];
    uint8_t buf1[_BUF_SIZE];
    uint8_t buf2[_BUF_SIZE];
    coap_pkt_t pkt1;
    coap_pkt_t pkt2;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/time";
    size_t len;
    coap_block1_t blockopt = {
        .offset = 0,
        .blknum = 0,
        .szx = 2,
        .more = 1,
    };

    /* 1. packet */
    len = coap_build_hdr((coap_hdr_t *)&buf1[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&pkt1, &buf1[0], sizeof(buf1), len);
    coap_opt_add_string(&pkt1, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_add_block1_control(&pkt1, &blockopt);
    coap_opt_finish(&pkt1, COAP_OPT_FINISH_NONE);

    blockopt.offset = 128;
    blockopt.blknum = 2;

    /* 2. packet */
    len = coap_build_hdr((coap_hdr_t *)&buf2[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&pkt2, &buf2[0], sizeof(buf2), len);
    coap_opt_add_string(&pkt2, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_add_block1_control(&pkt1, &blockopt);
    coap_opt_finish(&pkt2, COAP_OPT_FINISH_NONE);

    nanocoap_cache_key_blockreq_options_generate((const coap_pkt_t *) &pkt1, digest1);
    nanocoap_cache_key_blockreq_options_generate((const coap_pkt_t *) &pkt2, digest2);

    /* compare 1. and 2. packet. Should be equal except for blockwise */
    TEST_ASSERT_EQUAL_INT(0, nanocoap_cache_key_compare(digest1, digest2));

    /* Now with the blockwise option in the digest */
    nanocoap_cache_key_options_generate((const coap_pkt_t *) &pkt1, digest1);
    nanocoap_cache_key_options_generate((const coap_pkt_t *) &pkt2, digest2);

    /* compare 1. and 2. packet. Should no longer be equal */
    TEST_ASSERT(nanocoap_cache_key_compare(digest2, digest1) != 0);
}

static void test_nanocoap_cache__add(void)
{
    uint8_t buf[_BUF_SIZE];
    uint8_t rbuf[_BUF_SIZE];
    coap_pkt_t req, resp;

    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[16];
    nanocoap_cache_entry_t *c = NULL;
    size_t len;
    uint8_t temp_cache_key[CONFIG_NANOCOAP_CACHE_KEY_LENGTH];

    /* initialize the nanocoap cache */
    nanocoap_cache_init();

    /* add more entries to test LRU replacement */
    for (unsigned i = 0; i < CONFIG_NANOCOAP_CACHE_ENTRIES + 4; i++) {
        if (i < CONFIG_NANOCOAP_CACHE_ENTRIES) {
            TEST_ASSERT_EQUAL_INT(CONFIG_NANOCOAP_CACHE_ENTRIES - i,
                                  nanocoap_cache_free_count());
            TEST_ASSERT_EQUAL_INT(i, nanocoap_cache_used_count());
        }
        else {
            TEST_ASSERT_EQUAL_INT(0, nanocoap_cache_free_count());
            TEST_ASSERT_EQUAL_INT(CONFIG_NANOCOAP_CACHE_ENTRIES,
                                  nanocoap_cache_used_count());
        }

        snprintf(path, sizeof(path), "/path_%u", i);

        /* request */
        len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                             &token[0], 2, COAP_METHOD_GET, msgid);
        coap_pkt_init(&req, &buf[0], sizeof(buf), len);
        coap_opt_add_string(&req, COAP_OPT_URI_PATH, &path[0], '/');
        coap_opt_finish(&req, COAP_OPT_FINISH_NONE);

        /* response */
        len = coap_build_hdr((coap_hdr_t *)&rbuf[0], COAP_TYPE_NON,
                             &token[0], 2, COAP_CODE_205, msgid);
        coap_pkt_init(&resp, &rbuf[0], sizeof(rbuf), len);
        coap_opt_finish(&resp, COAP_OPT_FINISH_NONE);

        /* add a fake response buffer with fake response length */
        c = nanocoap_cache_add_by_req((const coap_pkt_t *)&req,
                                      (const coap_pkt_t *)&req,
                                      CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE);

        TEST_ASSERT_NOT_NULL(c);

        /* keep a pointer to the first cache entry for testing the LRU
           replacement */
        if (i == 0) {
            memcpy(temp_cache_key, c->cache_key,
                   CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
        }

        if (i == CONFIG_NANOCOAP_CACHE_ENTRIES-1) {
            /* lookup the first entry to update its list position */
            nanocoap_cache_key_lookup(temp_cache_key);
        }
    }

    /* the first entry should NOT be cached out, because we updated
     * its list position */
    c = nanocoap_cache_key_lookup(temp_cache_key);
    TEST_ASSERT_NOT_NULL(c);
}

static void test_nanocoap_cache__del(void)
{
    uint8_t buf[_BUF_SIZE];
    uint8_t rbuf[_BUF_SIZE];
    coap_pkt_t req, resp;

    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char *path = "/path";
    nanocoap_cache_entry_t *c = NULL;
    size_t len;
    int res;

    /* initialize the nanocoap cache */
    nanocoap_cache_init();

    TEST_ASSERT_EQUAL_INT(CONFIG_NANOCOAP_CACHE_ENTRIES,
                          nanocoap_cache_free_count());
    TEST_ASSERT_EQUAL_INT(0, nanocoap_cache_used_count());

    /* request */
    len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&req, &buf[0], sizeof(buf), len);
    coap_opt_add_string(&req, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_finish(&req, COAP_OPT_FINISH_NONE);

    /* response */
    len = coap_build_hdr((coap_hdr_t *)&rbuf[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_CODE_205, msgid);
    coap_pkt_init(&resp, &rbuf[0], sizeof(rbuf), len);
    coap_opt_finish(&resp, COAP_OPT_FINISH_NONE);

    c = nanocoap_cache_add_by_req((const coap_pkt_t *)&req,
                                  (const coap_pkt_t *)&resp,
                                  CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE);
    TEST_ASSERT_NOT_NULL(c);

    TEST_ASSERT_EQUAL_INT(CONFIG_NANOCOAP_CACHE_ENTRIES - 1,
                          nanocoap_cache_free_count());
    TEST_ASSERT_EQUAL_INT(1, nanocoap_cache_used_count());

    /* delete previously added cache entry */
    res = nanocoap_cache_del(c);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(CONFIG_NANOCOAP_CACHE_ENTRIES,
                          nanocoap_cache_free_count());
    TEST_ASSERT_EQUAL_INT(0, nanocoap_cache_used_count());
}

static void test_nanocoap_cache__max_age(void)
{
    uint8_t buf[_BUF_SIZE];
    uint8_t rbuf[_BUF_SIZE];
    coap_pkt_t req, resp;

    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char *path = "/path";
    nanocoap_cache_entry_t *c = NULL;
    size_t len;
    uint32_t now;

    /* initialize the nanocoap cache */
    nanocoap_cache_init();

    /* request */
    len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_METHOD_GET, msgid);
    coap_pkt_init(&req, &buf[0], sizeof(buf), len);
    coap_opt_add_string(&req, COAP_OPT_URI_PATH, &path[0], '/');
    coap_opt_finish(&req, COAP_OPT_FINISH_NONE);

    /* response with max-age 30 sec */
    len = coap_build_hdr((coap_hdr_t *)&rbuf[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_CODE_205, msgid);
    coap_pkt_init(&resp, &rbuf[0], sizeof(rbuf), len);
    coap_opt_add_uint(&resp, COAP_OPT_MAX_AGE, 30);
    coap_opt_finish(&resp, COAP_OPT_FINISH_NONE);

    c = nanocoap_cache_add_by_req((const coap_pkt_t *)&req,
                                  (const coap_pkt_t *)&resp,
                                  CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE);

    /* the absolute time of max-age should be at approx. now + 30 sec
       (1 sec buffer) */
    now = ztimer_now(ZTIMER_SEC);
    TEST_ASSERT(nanocoap_cache_entry_is_stale(c, now + 31));

    /* delete previously added cache entry */
    nanocoap_cache_del(c);

    /* response with max-age 60 sec (default, if option is missing) */
    len = coap_build_hdr((coap_hdr_t *)&rbuf[0], COAP_TYPE_NON,
                         &token[0], 2, COAP_CODE_205, msgid);
    coap_pkt_init(&resp, &rbuf[0], sizeof(rbuf), len);
    coap_opt_finish(&resp, COAP_OPT_FINISH_NONE);

    now = ztimer_now(ZTIMER_SEC);

    c = nanocoap_cache_add_by_req((const coap_pkt_t *)&req,
                                  (const coap_pkt_t *)&resp,
                                  CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE);

    /* the absolute time of max-age should be at approx. now + 60 sec
       (1 sec buffer) */
    now = ztimer_now(ZTIMER_SEC);
    TEST_ASSERT(nanocoap_cache_entry_is_stale(c, now + 61));
    /* check overflow cases */
    c->max_age = UINT32_MAX - 40;
    TEST_ASSERT(nanocoap_cache_entry_is_stale(c, 20));
}

Test *tests_nanocoap_cache_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocoap_cache__add),
        new_TestFixture(test_nanocoap_cache__del),
        new_TestFixture(test_nanocoap_cache__cachekey),
        new_TestFixture(test_nanocoap_cache__cachekey_blockwise),
        new_TestFixture(test_nanocoap_cache__max_age),
    };

    EMB_UNIT_TESTCALLER(nanocoap_cache_entry_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_cache_entry_tests;
}

void tests_nanocoap_cache(void)
{
    TESTS_RUN(tests_nanocoap_cache_tests());
}
/** @} */
