/*
 * Copyright (C) 2016 Lotte Steenbrink
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"

#include "seqnum.h"

#include "tests-gnrc_aodvv2.h"

static void test_seqnum_get(void)
{
    aodvv2_seqnum_t seqnum;

    seqnum = seqnum_get();
    TEST_ASSERT_EQUAL_INT(seqnum, 1);

    /* silence compiler */
    (void) seqnum;
}

/* Check if seqnum correctly wraps around from 65535 to 1*/
static void test_seqnum_wraparound(void)
{
    int cmp_result;
    aodvv2_seqnum_t seqnum;

    for (int i = 0; i < 65535; ++i){
        seqnum_inc();
    }

    seqnum = seqnum_get();
    cmp_result = seqnum_cmp(seqnum, (aodvv2_seqnum_t) 1);
    TEST_ASSERT_EQUAL_INT(cmp_result, 0);
}

static void setUp(void)
{
    seqnum_init();
}

Test *tests_gnrc_aodvv2_seqnum_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_seqnum_get),
        new_TestFixture(test_seqnum_wraparound),
    };

    EMB_UNIT_TESTCALLER(gnrc_aodvv2_tests, setUp, NULL, fixtures);

    return (Test *)&gnrc_aodvv2_tests;
}