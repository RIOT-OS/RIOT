/*
 * Copyright (C) 2022 K F
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittest entry point for the zt test group
 *
 * @author      kfessel
 */
#include <stdint.h>
#include <ztimer.h>
#include <stdio.h>
#include "embUnit/embUnit.h"

#include "tests-zt.h"

void zt_sleep(void){
    uint32_t start = ztimer_now(ZTIMER_MSEC);
    puts( "will sleep");
    ztimer_sleep(ZTIMER_MSEC,2000);
    puts( "done sleep");
    uint32_t end = ztimer_now(ZTIMER_MSEC);
    TEST_ASSERT_EQUAL_INT(2000, end - start);

}

Test * tests_zt_test(void){

    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(zt_sleep)
    };

    EMB_UNIT_TESTCALLER(zt_tests, NULL, NULL, fixtures);

    return (Test *)&zt_tests;
}

void tests_zt(void)
{
    TESTS_RUN(tests_zt_test());

}
/** @} */
