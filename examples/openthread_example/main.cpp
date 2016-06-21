#include "msg.h"
#include <stdio.h>
#include <openthread.h>
#include <cli/cli-serial.h>
#include "platform.h"
#include "platform/alarm.h"
#include "platform/serial.h"
#include "ot.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"
#include "random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define OPENTHREAD_QUEUE_LEN      (8)
static msg_t _queue[OPENTHREAD_QUEUE_LEN];
static uint8_t buf[100];
char ot_thread_stack[THREAD_STACKSIZE_MAIN];
#define INTERVAL (1000000U)

void *ot_thread(void *arg)
{
    (void) arg;
    PlatformInit();
    openthread_init();
    otInit();
	 otCliSerialInit();
	msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
	uint8_t *b = buf;
(void) b;
	netdev2_t *dev;
	while(1)
	{
		msg_t msg;
		otProcessNextTasklet();
		msg_receive(&msg);
		switch(msg.type)
		{
			case OPENTHREAD_XTIMER_MSG_TYPE_EVENT:
				DEBUG("openthread: otPlatAlarmFired\n");
				otPlatAlarmFired();
				break;
			case OPENTHREAD_NETDEV2_MSG_TYPE_EVENT:
				dev = (netdev2_t*) msg.content.ptr;
				DEBUG("openthread: Called driver isr\n");
				    dev->driver->isr(dev);
				break;
			case OPENTHREAD_SERIAL_MSG_TYPE_EVENT:
				serial_msg_t *ser = (serial_msg_t*) msg.content.ptr;
				otPlatSerialReceived((uint8_t*) ser->buf, ser->len);
				break;
				
		}
	}

    return NULL;
}

int main(void)
{
	xtimer_init();
	msg_t msg;
	serial_msg_t sm;
	sm.len = sizeof(buf)+1;
	msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
	msg.content.ptr = &sm;

    char buf[30];
    char *p=buf;
	sm.buf = (uint8_t*) buf;
    sm.len = 0;
	int now = xtimer_now();
	DEBUG("Waiting for seed. Please press enter\n");
	while(1)
	{
		if(getchar() == '\r' || getchar() == '\n')
		{
			now = xtimer_now()-now;
			DEBUG("Seed is %i\n", now);
			break;
		}
	}
	random_init(now);
kernel_pid_t pid = thread_create(ot_thread_stack, sizeof(ot_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
			                                ot_thread, NULL, "ot_thread");
	while(1)
	{
        *p = getchar();
        if(*p == '\r' || *p == '\n')
        {
            sm.len = p-buf+1; 
            msg_send(&msg, pid);
            p=buf;
        }
        else
        {
            p++;
        }
	}
	return 0;
}
