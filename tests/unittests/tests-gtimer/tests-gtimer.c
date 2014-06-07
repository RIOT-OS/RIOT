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

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "tests-gtimer.h"

#define EPSILON (1000) // epsilon should be high enough for platforms without a FPU
#define OFFSET (100000)
#define RATE (50)

timex_t gtimer_init_now;

static void set_up(void) {

}

static void test_local_now_nearly_equal_vtimer(void)
{
	timex_t now_vtimer;
	timex_t now_gtimer;
	uint64_t time_vtimer;
	uint64_t time_gtimer;

	vtimer_now(&now_vtimer);
	gtimer_now(&now_gtimer);

	time_vtimer = timex_uint64(now_vtimer);
	time_gtimer = timex_uint64(now_gtimer);
	DEBUG("time_vtimer %"PRIu64 " time_gtimer %"PRIu64 "\n", time_vtimer, time_gtimer);
	// check if gtimer is within EPSILON us of vtimer
	TEST_ASSERT((time_gtimer - time_vtimer) < EPSILON);
}

static void test_gtimer_sync_set_global_offset(void)
{
	timex_t now_gtimer;
	uint64_t time_gtimer;
	uint64_t time_vtimer;

	gtimer_sync_set_global_offset(OFFSET);
	gtimer_now(&now_gtimer);

	time_vtimer = timex_uint64(gtimer_init_now);
	time_gtimer = timex_uint64(now_gtimer);
	// check if gtimer + OFFSET is within EPSILON us of vtimer
	TEST_ASSERT((time_gtimer - (time_vtimer + OFFSET)) < EPSILON);
}

static void test_gtimer_sync_set_relative_rate(void)
{
	timex_t now_gtimer;
	uint64_t time_gtimer;
	uint64_t time_vtimer;

	gtimer_sync_set_relative_rate(RATE);
	vtimer_usleep(OFFSET);
	gtimer_now(&now_gtimer);

	time_vtimer = timex_uint64(gtimer_init_now);
	time_gtimer = timex_uint64(now_gtimer);
	DEBUG("time_gtimer: %"PRIu64 " timer_vtimer %"PRIu64 " GTIMER_CLOCKRATE_MULTIPLY(OFFSET,RATE)) %" PRIu32 " OFFSET %"PRIu32 "\n"
			, time_gtimer, time_vtimer, (GTIMER_CLOCKRATE_MULTIPLY(OFFSET,RATE)), OFFSET);
	// check if gtimer + OFFSET + OFFSET * RATE is within EPSILON us of vtimer
	TEST_ASSERT((time_gtimer - (time_vtimer + OFFSET + OFFSET + (GTIMER_CLOCKRATE_MULTIPLY(OFFSET,RATE)))) < EPSILON);
}

Test *tests_gtimer_tests(void)
{
	vtimer_now(&gtimer_init_now);
	gtimer_init();

	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture(test_local_now_nearly_equal_vtimer),
		new_TestFixture(test_gtimer_sync_set_global_offset),
		new_TestFixture(test_gtimer_sync_set_relative_rate),
};

	EMB_UNIT_TESTCALLER(gtimer_tests, set_up, NULL, fixtures);

	return (Test *)&gtimer_tests;
}

void tests_gtimer(void) {
	TESTS_RUN(tests_gtimer_tests());
}
