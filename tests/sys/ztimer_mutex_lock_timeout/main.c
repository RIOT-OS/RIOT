/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for testing ztimer_mutex_lock_timeout_timeout
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "mutex.h"
#include "test_utils/expect.h"
#include "timex.h"
#include "ztimer.h"

#ifndef TEST_CLOCK
#define TEST_CLOCK      ZTIMER_USEC
#endif

#ifndef TIMEOUT_SMALL
#define TIMEOUT_SMALL   (100U)
#endif

#ifndef TIMEOUT_LARGE
#define TIMEOUT_LARGE   (US_PER_SEC / 2)
#endif

static mutex_t testlock = MUTEX_INIT;

int main(void)
{
    ztimer_now_t pre, post;
    puts(
        "Test Application for ztimer_mutex_lock_timeout_timeout()\n"
        "================================================\n"
    );

    printf("%s: ", "Test on unlocked mutex with zero timeout");
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, 0) == 0);
    puts("OK");

    printf("%s: ", "Test on unlocked mutex with small timeout");
    mutex_unlock(&testlock);
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, TIMEOUT_SMALL) == 0);
    puts("OK");

    printf("%s: ", "Test on unlocked mutex with large timeout");
    mutex_unlock(&testlock);
    pre = ztimer_now(TEST_CLOCK);
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, US_PER_SEC / 2) == 0);
    post = ztimer_now(TEST_CLOCK);
    /* Call shouldn't block on unlocked mutex. So the duration spent on the
    * function call should be short. Let's take a very generous definition of
    * "short duration" here to not get false failures on slow boards */
    expect(post - pre < TIMEOUT_LARGE);
    puts("OK");

    printf("%s: ", "Test on locked mutex with zero timeout");
    pre = ztimer_now(TEST_CLOCK);
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, 0) == -ECANCELED);
    post = ztimer_now(TEST_CLOCK);
    expect(post - pre < TIMEOUT_LARGE);
    puts("OK");

    printf("%s: ", "Test on locked mutex with small timeout");
    pre = ztimer_now(TEST_CLOCK);
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, TIMEOUT_SMALL) == -ECANCELED);
    post = ztimer_now(TEST_CLOCK);
    expect((post - pre > TIMEOUT_SMALL) && (post - pre < TIMEOUT_LARGE));
    puts("OK");

    printf("%s: ", "Test on locked mutex with large timeout");
    pre = ztimer_now(TEST_CLOCK);
    expect(ztimer_mutex_lock_timeout(TEST_CLOCK, &testlock, TIMEOUT_LARGE) == -ECANCELED);
    post = ztimer_now(TEST_CLOCK);
    expect(post - pre > TIMEOUT_LARGE);
    puts("OK");

    puts("TEST PASSED");

    return 0;
}
