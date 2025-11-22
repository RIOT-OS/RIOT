/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for suit manifest parser module
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>

#include "kernel_defines.h"
#include "log.h"

#include "suit.h"
#include "suit/storage.h"
#include "suit/transport/mock.h"
#include "embUnit.h"

#define TEST_MANIFEST_INCLUDE(file) <blob/bin/BOARD_NAME_UNQ/manifests/file>

/* cppcheck-suppress preprocessorErrorDirective
 * (reason: board-dependent include paths) */
#include TEST_MANIFEST_INCLUDE(manifest0.bin.h)
#include TEST_MANIFEST_INCLUDE(manifest1.bin.h)
#include TEST_MANIFEST_INCLUDE(manifest2.bin.h)
#include TEST_MANIFEST_INCLUDE(manifest3.bin.h)
#include TEST_MANIFEST_INCLUDE(manifest4.bin.h)

#include TEST_MANIFEST_INCLUDE(file1.bin.h)
#include TEST_MANIFEST_INCLUDE(file2.bin.h)

typedef struct {
    const unsigned char *data;
    size_t len;
    int expected;
} manifest_blob_t;

const manifest_blob_t manifest_blobs[] = {
    /* Older GCC can't handle manifestx_bin_len here */
    { manifest0_bin, sizeof(manifest0_bin), SUIT_ERR_SIGNATURE },
    { manifest1_bin, sizeof(manifest1_bin), SUIT_ERR_SEQUENCE_NUMBER },
    { manifest2_bin, sizeof(manifest2_bin), SUIT_ERR_COND },
    { manifest3_bin, sizeof(manifest3_bin), SUIT_OK },
    { manifest4_bin, sizeof(manifest4_bin), SUIT_OK },
};

const unsigned manifest_blobs_numof = ARRAY_SIZE(manifest_blobs);

const suit_transport_mock_payload_t payloads[] = {
    {
        .buf = file1_bin,
        .len = sizeof(file1_bin),
    },
    {
        .buf = file2_bin,
        .len = sizeof(file2_bin),
    }
};

const size_t num_payloads = ARRAY_SIZE(payloads);

static int test_suit_manifest(const unsigned char *manifest_bin,
                                size_t manifest_bin_len)
{
    char _url[CONFIG_SOCK_URLPATH_MAXLEN];
    suit_manifest_t manifest;

    memset(&manifest, 0, sizeof(manifest));

    manifest.urlbuf = _url;
    manifest.urlbuf_len = CONFIG_SOCK_URLPATH_MAXLEN;

    int res;
    if ((res =
             suit_parse(&manifest, manifest_bin,
                           manifest_bin_len)) != SUIT_OK) {
        return res;
    }

    return res;
}

static void test_suit_manifest_01_manifests(void)
{
    suit_storage_set_seq_no_all(1);
    for (unsigned i = 0; i < manifest_blobs_numof; i++) {
        printf("\n--- testing manifest %u\n", i);
        int res = \
            test_suit_manifest(manifest_blobs[i].data, manifest_blobs[i].len);
        printf("---- res=%i (expected=%i)\n", res, manifest_blobs[i].expected);
        TEST_ASSERT_EQUAL_INT(manifest_blobs[i].expected, res);
    }
}

Test *tests_suit_manifest(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_suit_manifest_01_manifests),
    };

    EMB_UNIT_TESTCALLER(suit_manifest_tests, NULL, NULL, fixtures);

    return (Test *)&suit_manifest_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_suit_manifest());
    TESTS_END();
    return 0;
}
