#include "ot.h"
#include "msg.h"
#include "platform.h"
#include "openthread.h"

#include "platform/alarm.h"
#include "platform/uart.h"

#include <cli/cli-uart.h>
#include <assert.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <errno.h>
#include "random.h"

#define OPENTHREAD_QUEUE_LEN      (8)
static msg_t _queue[OPENTHREAD_QUEUE_LEN];

static kernel_pid_t _pid;

void otSignalTaskletPending(void)
{
}

void *_openthread_event_loop(void *arg)
{
	_pid = thread_getpid();
    PlatformInit();
    otInit();

	msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
	netdev2_t *dev;
	msg_t msg;

#ifdef MODULE_OPENTHREAD_CLI
	serial_msg_t *ser;
	otCliUartInit();
#else
	otEnable();
	otProcessNextTasklet();
#endif

	while(1)
	{
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
#ifdef MODULE_OPENTHREAD_CLI
			case OPENTHREAD_SERIAL_MSG_TYPE_EVENT:
				ser = (serial_msg_t*) msg.content.ptr;
				otPlatUartReceived((uint8_t*) ser->buf, ser->len);
				break;
#endif
				
		}
	}

    return NULL;
}

void _event_cb(netdev2_t *dev, netdev2_event_t event)
{
    if (event == NETDEV2_EVENT_ISR) {
        assert(_pid != KERNEL_PID_UNDEF);
        msg_t msg;

        msg.type = OPENTHREAD_NETDEV2_MSG_TYPE_EVENT;
        msg.content.ptr = dev;

        if (msg_send(&msg, _pid) <= 0) {
            DEBUG("openthread_netdev2: possibly lost interrupt.\n");
        }
    }
	else
	{
		switch(event)
		{
			case NETDEV2_EVENT_RX_COMPLETE:
				DEBUG("openthread: NETDEV2_EVENT_RX_COMPLETE\n");
				recv_pkt(dev);
				break;
			case NETDEV2_EVENT_TX_COMPLETE:
				DEBUG("openthread: NETDEV2_EVENT_TX_COMPLETE\n");
				send_pkt(dev);
				break;
			case NETDEV2_EVENT_TX_NOACK:
				DEBUG("openthread: NETDEV2_EVENT_TX_NOACK\n");
				otPlatRadioTransmitDone(false, kThreadError_NoAck);
				break;
			case NETDEV2_EVENT_TX_MEDIUM_BUSY:
				DEBUG("openthread: NETDEV2_EVENT_TX_MEDIUM_BUSY\n");
				otPlatRadioTransmitDone(false, kThreadError_ChannelAccessFailure);
				break;
			default:
				break;
		}
	}
}

kernel_pid_t openthread_get_pid(void)
{
	return _pid;
}
int openthread_netdev2_init(char *stack, int stacksize, char priority,
		                        const char *name)
{
   _pid = thread_create(stack, stacksize,
                           priority, THREAD_CREATE_STACKTEST,
			                                _openthread_event_loop, NULL, name);

   if(_pid <= 0)
	   return -EINVAL;

   return _pid;
}
