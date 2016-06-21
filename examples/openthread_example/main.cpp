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
				otPlatAlarmFired();
				break;
			case OPENTHREAD_NETDEV2_MSG_TYPE_EVENT:
				dev = (netdev2_t*) msg.content.ptr;
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
kernel_pid_t pid = thread_create(ot_thread_stack, sizeof(ot_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
			                                ot_thread, NULL, "ot_thread");
	(void) pid;
	uint32_t last_wakeup = xtimer_now();
	msg_t msg;
	uint8_t *buf = (uint8_t*) "help\n";
	serial_msg_t sm;
	sm.buf = buf;
	sm.len = sizeof(buf)+1;
	msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
	msg.content.ptr = &sm;
	while(1)
	{
		msg_send(&msg, pid);
	}
	return 0;
}
