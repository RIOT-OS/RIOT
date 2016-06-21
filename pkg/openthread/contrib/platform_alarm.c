#include <stdint.h>
#include "xtimer.h"
#include "msg.h"
#include "thread.h"

#include <platform/alarm.h>
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static xtimer_t ot_timer;
static msg_t ot_alarm_msg;

void otPlatAlarmStartAt(uint32_t aT0, uint32_t aDt)
{
	DEBUG("openthread: otPlatAlarmStartAt: aT0: %i, aDT: %i\n", aT0, aDt);
	//xtimer_remove(&ot_timer);
	ot_alarm_msg.type = OPENTHREAD_XTIMER_MSG_TYPE_EVENT;
	int dt = aT0+aDt-xtimer_now();
	DEBUG("Dt is %i\n", dt);
	if(dt <= 0)
		msg_send(&ot_alarm_msg, thread_getpid());
	else
		xtimer_set_msg(&ot_timer, dt, &ot_alarm_msg, thread_getpid());
}

void otPlatAlarmStop(void)
{
	DEBUG("openthread: otPlatAlarmStop\n");
	xtimer_remove(&ot_timer);
}

uint32_t otPlatAlarmGetNow(void)
{
	uint32_t now = xtimer_now();
	DEBUG("openthread: otPlatAlarmGetNow: %i\n", now);
	return now;
}
