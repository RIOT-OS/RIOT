/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for module suit_v4
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Koen Zandberg <koen@bergzand.net>
 */

#include <stdio.h>

#include "kernel_defines.h"
#include "log.h"

#include "suit/v4/suit.h"
#include "embUnit.h"

#include "blob/bin/manifests/manifest0.bin.h"
#include "blob/bin/manifests/manifest1.bin.h"
#include "blob/bin/manifests/manifest2.bin.h"
#include "blob/bin/manifests/manifest3.bin.h"
#include "blob/bin/manifests/manifest4.bin.h"

#define SUIT_URL_MAX            128

typedef struct {
    const unsigned char *data;
    const size_t len;
} blob_t;

const blob_t bad[] = {
    /* GCC 7.3.1 can't handle manifestx_bin_len here */
    { manifest0_bin, sizeof(manifest0_bin) },
    { manifest1_bin, sizeof(manifest1_bin) },
    { manifest2_bin, sizeof(manifest2_bin) },
    { manifest3_bin, sizeof(manifest3_bin) },
};

const size_t bad_numof = ARRAY_SIZE(bad);

const blob_t good[] = {
    { manifest4_bin, sizeof(manifest4_bin) },
};

const size_t good_numof = ARRAY_SIZE(good);

static int test_suitv4_manifest(const unsigned char *manifest_bin, size_t manifest_bin_len)
{
    char _url[SUIT_URL_MAX];
    suit_v4_manifest_t manifest;
    riotboot_flashwrite_t writer;
    memset(&writer, 0, sizeof(manifest));

    manifest.writer = &writer;
    manifest.urlbuf = _url;
    manifest.urlbuf_len = SUIT_URL_MAX;

    return suit_v4_parse(&manifest, manifest_bin, manifest_bin_len);
}

static void test_suitv4_manifest_01_bad_manifests(void)
{
    for (size_t i = 0; i < bad_numof; i++) {
        printf("\n--- testing bad manifest %u\n", (unsigned)i);
        int res = test_suitv4_manifest(bad[i].data, bad[i].len);
        TEST_ASSERT_EQUAL_INT(-1, res);
    }
}

static void test_suitv4_manifest_02_good_manifests(void)
{
    for (size_t i = 0; i < good_numof; i++) {
        printf("\n--- testing good manifest %u\n", (unsigned)i);
        int res = test_suitv4_manifest(good[i].data, good[i].len);
        TEST_ASSERT_EQUAL_INT(0, res);
    }
}

Test *tests_suitv4_manifest(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_suitv4_manifest_01_bad_manifests),
        new_TestFixture(test_suitv4_manifest_02_good_manifests),
    };

    EMB_UNIT_TESTCALLER(suitv4_manifest_tests, NULL, NULL, fixtures);

    return (Test *)&suitv4_manifest_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_suitv4_manifest());
    TESTS_END();
    return 0;
}
