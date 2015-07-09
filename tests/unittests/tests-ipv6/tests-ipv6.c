/*
 * Copyright (C) 2015 Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
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

#include "embUnit/embUnit.h"

#include "kernel_types.h"
#include "byteorder.h"

#include "net/ng_ipv6.h"

#include "unittests-constants.h"
#include "tests-ipv6.h"


static void set_up(void)
{
    ;
}


static void tear_down(void)
{
    ;
}


/*
 * Tests for function "ng_ipv6_init"
 */

/*
 * Tests normal case.
 *
 * Pre: No IPV6 thread has been initialized.
 * Post: One single IPV6 thread has been initialized.
 * ENHANCE:
 *   - Provide check for existece of thread.
 *   - Provide check if pid didn't exist before..
 */
static void test_ipv6_init__first(void)
{
    const char *errstr = "IPV6 not initialized";
    kernel_pid_t retval = KERNEL_PID_UNDEF;

    retval = ng_ipv6_init();

    TEST_ASSERT_MESSAGE((KERNEL_PID_FIRST <= retval) &&
                        (KERNEL_PID_LAST  >= retval),
                        errstr);
}

/* Tests check for error condition 1
 *
 * Pre: IPV6 thread has already been initialized.
 * Post: No new IPV6 thread has already been initialized.
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
    retval = ng_ipv6_init();

    TEST_ASSERT_MESSAGE(EEXIST == retval,
                        errstr);
}

/* Deferred!
 * Tests recognition for error condition 2.
 *
 * Pre: No IPV6 thread has been initialized and MAXTHREADS threads already
 *       exist.
 * Post: No IPV6 thread has been initialized.
 * TODO:
 *   - Remove thread from former test.
 *   - Controlled creation of precondition.
 */
static void test_ipv6_init__maxthr(void)
{
    const char *errstr = "!Test insufficient.!";
    kernel_pid_t retval = KERNEL_PID_UNDEF;

    retval = ng_ipv6_init();

    TEST_ASSERT_MESSAGE(-EOVERFLOW == retval,
                        errstr);
}


/*
 * Tests for function "ng_ipv6_init"
 */

/* Deferred!
 * Pre: ...
 * Post: ....
 * TODO:
 *   - Provide specification of function behaviour, pre- and postconditions.
 *   - Provide (in)valid values for function parameters.
 */
static void test_ipv6_demux__stub1(void)
{
    const char *errstr = "!Test insufficient.!";
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    ng_pktsnip_t  *pkt = NULL;
    uint8_t         nh = 0;

    iface = ng_ipv6_init();
    ng_ipv6_demux(iface, pkt, nh);

    TEST_ASSERT_MESSAGE(NULL == pkt,
                        errstr);
}


Test *tests_ipv6_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_init__first),
                        new_TestFixture(test_ipv6_init__uniq)
                        /* Deferred, See function comments.
                          new_TestFixture(test_ipv6_init__maxthr),
                          new_TestFixture(test_ipv6_demux__stub1)
                         */
    };

    EMB_UNIT_TESTCALLER(ipv6_tests, set_up, tear_down, fixtures);

    return (Test *)&ipv6_tests;
}

void tests_ipv6(void)
{
    TESTS_RUN(tests_ipv6_tests());
}
/** @} */
