/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include "embUnit/embUnit.h"

#include "gtimer.h"
#include "hwtimer.h"
#include "vtimer.h"

#include "tests-gtimer.h"

static void set_up(void) {
	gtimer_init();
}

static void test_local_now_equal_vtimer(void)
{
	timex_t now_vtimer;
	timex_t now_gtimer;
	uint64_t time_vtimer;
	uint64_t time_gtimer;

	vtimer_now(&now);
	gtimer_now(&now_gtimer);

	time_vtimer = timex_uint64(&now_vtimer);
	time_gtimer = timex_uint64(&now_gtimer);
	// check if gtimer is within 1 ms of vtimer
	TEST_ASSERT((time_gtimer - time_vtimer) < 1000);
}

Test *tests_gtimer_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture(test_local_now_equal_vtimer),
};

	EMB_UNIT_TESTCALLER(gtimer_tests, set_up, NULL, fixtures);

	return (Test *)&gtimer_tests;
}

void tests_gtimer(void) {
	TESTS_RUN(tests_gtimer_tests());
}
