/*
 * Copyright (C) 2022 HAW Hamburg
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
#include <stdlib.h>
#include <string.h>

#include "bhp/msg.h"
#include "embUnit.h"

#include "unittests-constants.h"
#include "tests-bhp_msg.h"

static bhp_msg_t bhp_msg;
static int canary;
static int *ctx;

static char _stack[THREAD_STACKSIZE_DEFAULT];

static void bhp_handler(void *arg)
{
    TEST_ASSERT(arg == ctx);
    canary = TEST_INT;
}

static void set_up(void)
{
    memset(&bhp_msg, '\0', sizeof(bhp_msg));
    canary = 0;
    bhp_msg_init(&bhp_msg, bhp_handler, ctx);
}

static void test_bhp_msg__init(void)
{
    TEST_ASSERT(bhp_msg.pid == KERNEL_PID_UNDEF);
    TEST_ASSERT(bhp_msg.msg.type == BHP_MSG_BH_REQUEST);
    TEST_ASSERT(bhp_msg.bhp.irq_handler == bhp_handler);
    TEST_ASSERT(bhp_msg.bhp.ctx == ctx);
}

static void *_event_loop(void *arg)
{
    (void) arg;
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == BHP_MSG_BH_REQUEST) {
            bhp_msg_handler(&msg);
        }
    }

    return NULL;
}

static void test_bhp_msg__claim(void)
{
    TEST_ASSERT_EQUAL_INT(0, bhp_msg.pid);
    bhp_msg_claim_thread(&bhp_msg, TEST_UINT8);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, bhp_msg.pid);
}

static void test_bhp_msg__cb(void)
{
    kernel_pid_t pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN - 1,
                             0, _event_loop, NULL,
                             "bhp_msg");

    bhp_msg_claim_thread(&bhp_msg, pid);
    TEST_ASSERT_EQUAL_INT(0, canary);
    bhp_msg_isr_cb(&bhp_msg);
    TEST_ASSERT_EQUAL_INT(TEST_INT, canary);
}

Test *tests_bhp_msg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bhp_msg__init),
        new_TestFixture(test_bhp_msg__claim),
        new_TestFixture(test_bhp_msg__cb),
    };

    EMB_UNIT_TESTCALLER(bhp_msg_tests, set_up, NULL, fixtures);

    return (Test *)&bhp_msg_tests;
}

void tests_bhp_msg(void)
{
    TESTS_RUN(tests_bhp_msg_tests());
}
/** @} */
