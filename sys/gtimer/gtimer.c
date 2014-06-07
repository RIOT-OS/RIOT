/**
 * Declarations for the virtual clock
 *
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys
 * @{
 * @file    gtimer.h
 * @brief	Declarations for the virtual clock
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @}
 */
#include <stdint.h>
#include <stdio.h>
#include "timex.h"
#include "msg.h"
#include "gtimer.h"
#include "mutex.h"
#include "vtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

gtimer_timeval_t last;
mutex_t gtimer_mutex;

#ifdef GTIMER_TEST
timex_t manual_now;

void gtimer_sync_set_local(timex_t new_now) {
	manual_now = new_now;
}
#define LOCAL_NOW(TARGET) TARGET = manual_now
#else
#define LOCAL_NOW(TARGET) vtimer_now(& TARGET)
#endif

static void _gtimer_refresh_last_local(void);

static void _gtimer_now(gtimer_timeval_t *out);

/**
 * @brief Returns the current global time. 
 */
void gtimer_now(timex_t *out) {
	gtimer_timeval_t now;
	gtimer_sync_now(&now);
	timex_t result = timex_from_uint64(now.global);
	*out = result;
}

/**
 * @brief Starts a new global clock based on a real hw clock.
 */
void gtimer_init(void) {
	timex_t now;
	LOCAL_NOW(now);
	mutex_init(&gtimer_mutex);
	last.local = timex_uint64(now);
	last.global = timex_uint64(now);
	last.rate = 0;
	puts("gtimer initialized\n");
}

void gtimer_sync_now(gtimer_timeval_t *out) {
	mutex_lock(&gtimer_mutex);
	_gtimer_now(out);
	mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_global_offset(uint64_t global_offset) {
	mutex_lock(&gtimer_mutex);
	_gtimer_refresh_last_local();
	last.global += global_offset;
	mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_relative_rate(uint32_t rate) {
	mutex_lock(&gtimer_mutex);
	_gtimer_refresh_last_local();
	last.rate = rate;
	mutex_unlock(&gtimer_mutex);
}

float gtimer_sync_get_relative_rate(void) {
	return last.rate;
}

static void _gtimer_now(gtimer_timeval_t *out) {
	uint64_t now;
	timex_t temp;
	LOCAL_NOW(temp);
	now = timex_uint64(temp);
	DEBUG("clockrate multiply: %"PRIu64 "\n", GTIMER_CLOCKRATE_MULTIPLY((now - last.local), last.rate));
	out->global = last.global
			+ (GTIMER_CLOCKRATE_MULTIPLY((now - last.local), last.rate) + (now - last.local));
	out->local = now;
}

static void _gtimer_refresh_last_local(void) {
	gtimer_timeval_t now;
	_gtimer_now(&now);
	last = now;
}
