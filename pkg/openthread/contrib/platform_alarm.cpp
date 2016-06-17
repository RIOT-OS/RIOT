#include <stdint.h>
#include "xtimer.h"
#include "msg.h"
#include "thread.h"

#include <platform/alarm.h>
#include "ot.h"

static xtimer_t ot_timer;
static msg_t ot_alarm_msg;

void otPlatAlarmStartAt(uint32_t aT0, uint32_t aDt)
{
	xtimer_remove(&ot_timer);
	ot_alarm_msg.type = OPENTHREAD_XTIMER_MSG_TYPE_EVENT;
	xtimer_set_msg(&ot_timer, aT0+aDt-xtimer_now(), &ot_alarm_msg, thread_getpid());
}

void otPlatAlarmStop(void)
{
	xtimer_remove(&ot_timer);
}

uint32_t otPlatAlarmGetNow(void)
{
	return xtimer_now();
}
