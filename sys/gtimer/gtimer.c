/**
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys
 * @{
 * @file    gtimer.c
 * @brief	Global clock implementation for clock-sync protocols
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
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

gtimer_timeval_t gtimer_last;
mutex_t gtimer_mutex;

static void _gtimer_refresh_last_local(void);

static void _gtimer_now(gtimer_timeval_t *out);

void gtimer_now(timex_t *out) {
	gtimer_timeval_t now;
	gtimer_sync_now(&now);
	timex_t result = timex_from_uint64(now.global);
	*out = result;
}

void gtimer_init(void) {
    DEBUG("gtimer_init()\n");
	timex_t now;
	vtimer_now(&now);
	mutex_init(&gtimer_mutex);
	gtimer_last.local = timex_uint64(now);
	gtimer_last.global = timex_uint64(now);
	gtimer_last.rate = 0.0;
}

void gtimer_sync_now(gtimer_timeval_t *out) {
	mutex_lock(&gtimer_mutex);
	_gtimer_now(out);
	mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_global_offset(uint64_t global_offset) {
	mutex_lock(&gtimer_mutex);
	_gtimer_refresh_last_local();
	gtimer_last.global += global_offset;
	mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_relative_rate(float rate) {
	mutex_lock(&gtimer_mutex);
	_gtimer_refresh_last_local();
	gtimer_last.rate = rate;
	mutex_unlock(&gtimer_mutex);
}

float gtimer_sync_get_relative_rate(void) {
	return gtimer_last.rate;
}

static void _gtimer_now(gtimer_timeval_t *out) {
	uint64_t now;
	timex_t temp;
	vtimer_now(&temp);
	now = timex_uint64(temp);
	DEBUG("clockrate multiply: %"PRIu64 "\n", ((now - gtimer_last.local) * gtimer_last.rate));
	out->global = gtimer_last.global
			+ (((now - gtimer_last.local) * gtimer_last.rate) + (now - gtimer_last.local));
	out->local = now;
}

static void _gtimer_refresh_last_local(void) {
	gtimer_timeval_t now;
	_gtimer_now(&now);
	gtimer_last = now;
}
