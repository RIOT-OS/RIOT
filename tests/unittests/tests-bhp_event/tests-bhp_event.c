/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdlib.h>

#include "bhp/event.h"
#include "event.h"
#include "event/thread.h"
#include "embUnit.h"

#include "unittests-constants.h"
#include "tests-bhp_event.h"

extern void auto_init_event_thread(void);

static bhp_event_t bhp_event;
static int canary;
static int *ctx;

static void bhp_handler(void *arg)
{
    TEST_ASSERT(arg == ctx);
    canary = TEST_INT;
}

static void set_up(void)
{
    memset(&bhp_event, '\0', sizeof(bhp_event));
    canary = 0;
    bhp_event_init(&bhp_event, EVENT_PRIO_HIGHEST, bhp_handler, ctx);
}

static void test_bhp_event__init(void)
{
    TEST_ASSERT(bhp_event.evq == EVENT_PRIO_HIGHEST);
    TEST_ASSERT(bhp_event.bhp.irq_handler == bhp_handler);
    TEST_ASSERT(bhp_event.bhp.ctx == ctx);
}

static void test_bhp_event__cb(void)
{
    auto_init_event_thread();
    TEST_ASSERT_EQUAL_INT(0, canary);
    bhp_event_isr_cb(&bhp_event);
    TEST_ASSERT_EQUAL_INT(TEST_INT, canary);
}

Test *tests_bhp_event_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bhp_event__init),
        new_TestFixture(test_bhp_event__cb),
    };

    EMB_UNIT_TESTCALLER(bhp_event_tests, set_up, NULL, fixtures);

    return (Test *)&bhp_event_tests;
}

void tests_bhp_event(void)
{
    TESTS_RUN(tests_bhp_event_tests());
}
/** @} */
