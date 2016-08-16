/*
 * Copyright (C) 2016 Lotte Steenbrink
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include "embUnit.h"

#include "routingtable.h"

#include "tests-gnrc_aodvv2.h"

static void test_table_test(void)
{
    printf("xoxo");
}

static void setUp(void)
{
    routingtable_init();
}

Test *tests_gnrc_aodvv2_table_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_table_test),
    };

    EMB_UNIT_TESTCALLER(gnrc_aodvv2_tests, setUp, NULL, fixtures);

    return (Test *)&gnrc_aodvv2_tests;
}