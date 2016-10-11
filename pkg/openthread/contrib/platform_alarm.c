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

/* starts an aDt millisecs alarm when current time is aT0 millisecs */
void otPlatAlarmStartAt(uint32_t aT0, uint32_t aDt)
{
	DEBUG("openthread: otPlatAlarmStartAt: aT0: %i, aDT: %i\n", (int) aT0, (int) aDt);
	ot_alarm_msg.type = OPENTHREAD_XTIMER_MSG_TYPE_EVENT;

	int dt = aDt*1000;
	if(dt == 0)
	{
		msg_send(&ot_alarm_msg, thread_getpid());
	}
	else
	{
		xtimer_set_msg(&ot_timer, dt, &ot_alarm_msg, thread_getpid());
	}
}

/* stop alarms */
void otPlatAlarmStop(void)
{
	DEBUG("openthread: otPlatAlarmStop\n");
	xtimer_remove(&ot_timer);
}

/* return running time in millisecs */
uint32_t otPlatAlarmGetNow(void)
{
	uint32_t now = xtimer_now()/1000;
	DEBUG("openthread: otPlatAlarmGetNow: %i\n", (int) now);
	return now;
}
