/*
 * Copyright (C) 2015 Tu Nguyen Anh <tu_nguyen@gmx.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stddef.h>

#include "embUnit/embUnit.h"
#include "unittests-constants.h"

#include "tcb.h"

#include "tests-core.h"

#define TCP_T_P ((tcb_t *)0)

static void test_tcb_sizeof(void)
{	
	TEST_ASSERT(sizeof(TCP_T_P->sp) == sizeof(char *));
	TEST_ASSERT(sizeof(TCP_T_P->status) == sizeof(uint16_t));

	TEST_ASSERT(sizeof(TCP_T_P->pid) == sizeof(kernel_pid_t));
	TEST_ASSERT(sizeof(TCP_T_P->priority) == sizeof(uint16_t));

	TEST_ASSERT(sizeof(TCP_T_P->rq_entry) == sizeof(clist_node_t));

	TEST_ASSERT(sizeof(TCP_T_P->wait_data) == sizeof(void *));
	TEST_ASSERT(sizeof(TCP_T_P->msg_waiters) == sizeof(priority_queue_t));

	TEST_ASSERT(sizeof(TCP_T_P->msg_queue) == sizeof(cib_t));
	TEST_ASSERT(sizeof(TCP_T_P->msg_array) == sizeof(msg_t *));
	

#if defined(DEVELHELP) || defined(SCHED_TEST_STACK)
	TEST_ASSERT(sizeof(TCP_T_P->stack_start) == sizeof(char *));
#endif
#ifdef DEVELHELP
	TEST_ASSERT(sizeof(TCP_T_P->name) == sizeof(cib_t));
	TEST_ASSERT(sizeof(TCP_T_P->stack_size) == sizeof(int));
#endif
}

Test *tests_core_tcb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tcb_sizeof),
	 };

    EMB_UNIT_TESTCALLER(core_tcb_tests, NULL, NULL, fixtures);

    return (Test *)&core_tcb_tests;
}
