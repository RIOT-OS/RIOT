/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for testing the mutex_cancel function in
 *              core_mutex
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "mutex.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

static mutex_t testlock = MUTEX_INIT;

static void cb_unlock(void *mutex)
{
    mutex_unlock(mutex);
}

static void cb_cancel(void *mc)
{
    mutex_cancel(mc);
}

int main(void)
{
    xtimer_t xt;
    puts(
        "Test Application for mutex_cancel / mutex_lock_cancelable\n"
        "=========================================================\n"
    );

    printf("%s: ", "Test without cancellation");
    mutex_cancel_t mc = mutex_cancel_init(&testlock);
    expect(mutex_lock_cancelable(&mc) == 0);
    puts("OK");

    printf("%s: ", "Test early cancellation");
    mc = mutex_cancel_init(&testlock);
    mutex_cancel(&mc);
    expect(mutex_lock_cancelable(&mc) == -ECANCELED);
    puts("OK");

    printf("%s: ", "Verify no side effects on subsequent calls");
    mc = mutex_cancel_init(&testlock);
    xt.callback = cb_unlock;
    xt.arg = &testlock;
    xtimer_set(&xt, US_PER_MS * 10);
    expect(mutex_lock_cancelable(&mc) == 0);
    puts("OK");

    printf("%s: ", "Test late cancellation");
    mc = mutex_cancel_init(&testlock);
    xt.callback = cb_cancel;
    xt.arg = &mc;
    xtimer_set(&xt, US_PER_MS * 10);
    expect(mutex_lock_cancelable(&mc) == -ECANCELED);
    puts("OK");

    puts("TEST PASSED");

    return 0;
}
