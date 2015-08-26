/*
 * Copyright (C) 2015 Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests-ipv6
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "kernel_types.h"
#include "byteorder.h"

#include "net/gnrc/ipv6.h"

#include "../tests-ipv6.h"


static void set_up(void)
{
    ;
}


static void tear_down(void)
{
    ;
}


/*
 * Tests for function "gnrc_ipv6_init"
 */

/**
 * @brief Tests normal case.
 *
 * @pre No IPV6 thread has been initialized.
 * @post One single IPV6 thread has been initialized.
 *
 * ENHANCE:
 *   - Provide check for existece of thread.
 *   - Provide check if pid didn't exist before..
 */
static void test_ipv6_init__first(void)
{
    const char *errstr = "IPV6 not initialized";
    kernel_pid_t retval = KERNEL_PID_UNDEF;

    retval = gnrc_ipv6_init();

    TEST_ASSERT_MESSAGE((KERNEL_PID_FIRST <= retval) &&
                        (KERNEL_PID_LAST  >= retval),
                        errstr);
}

/**
 * @brief Tests check for error condition 2
 *
 * @pre IPV6 thread has already been initialized.
 * @post No new IPV6 thread has already been initialized.
 *
 * ENHANCE:
 *   - Provide check for uniqueness of first thread.
 */
static void test_ipv6_init__uniq(void)
{
    const char *errstr = "IPV6 initialized twice";
    kernel_pid_t retval = KERNEL_PID_UNDEF;

    /*
     * ATM the thread has been initialized in test_*__uniq.
     */
    retval = gnrc_ipv6_init();

    TEST_ASSERT_MESSAGE( -EEXIST == retval, errstr);
}


Test *ipv6_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_init__first),
                        new_TestFixture(test_ipv6_init__uniq)
    };

    EMB_UNIT_TESTCALLER(ipv6_tests, set_up, tear_down, fixtures);

    return (Test *)&ipv6_tests;
}
/** @} */
