
#include "thread.h"
#include <assert.h>

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#include "openthread.h"
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "platform.h"
#include "platform/alarm.h"
#include "platform/serial.h"

#define OPENTHREAD_QUEUE_LEN      (8)

#include <cli/cli-serial.h>
#include "xtimer.h"

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define OPENTHREAD_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif



#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

#define OPENTHREAD_NETDEV2_BUFLEN (ETHERNET_MAX_LEN)

void otSignalTaskletPending(void)
{
}


static uint8_t _tmp_buf[OPENTHREAD_NETDEV2_BUFLEN];
static uint8_t transmit_buf[OPENTHREAD_NETDEV2_BUFLEN];
static msg_t _queue[OPENTHREAD_QUEUE_LEN];

static kernel_pid_t _pid;

void *ot_thread(void *arg)
{
	(void) arg;
    PlatformInit();
    openthread_init();
    otInit();
	otCliSerialInit();

	msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
	netdev2_t *dev;
	serial_msg_t *ser;
	msg_t msg;

	while(1)
	{
		otProcessNextTasklet();
		msg_receive(&msg);
		switch(msg.type)
		{
			case OPENTHREAD_XTIMER_MSG_TYPE_EVENT:
				//DEBUG("openthread: otPlatAlarmFired at %i\n", (int) (xtimer_now()/1000));
				otPlatAlarmFired();
				break;
			case OPENTHREAD_NETDEV2_MSG_TYPE_EVENT:
				dev = (netdev2_t*) msg.content.ptr;
				//DEBUG("openthread: Called driver isr\n");
				dev->driver->isr(dev);
				break;
			case OPENTHREAD_SERIAL_MSG_TYPE_EVENT:
				ser = (serial_msg_t*) msg.content.ptr;
				otPlatSerialReceived((uint8_t*) ser->buf, ser->len);
				break;
				
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
				DEBUG("openthread: Receiving pkt\n");
				recv_pkt(dev);
				break;
			case NETDEV2_EVENT_TX_COMPLETE:
				DEBUG("openthread: Sending a pkt\n");
				send_pkt(dev);
				break;
			case NETDEV2_EVENT_TX_NOACK:
				DEBUG("NETDEV2_EVENT_TX_NOACK\n");
				//otPlatRadioTransmitDone(false, kThreadError_NoAck);
				break;
			case NETDEV2_EVENT_TX_MEDIUM_BUSY:
				DEBUG("NETDEV2_EVENT_TX_MEDIUM_BUSY\n");
				//otPlatRadioTransmitDone(false, kThreadError_ChannelAccessFailure);
				break;
			default:
				break;
		}
	}
}

void openthread_init(void)
{
	_pid = thread_getpid();
#ifdef MODULE_AT86RF2XX
        at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
		netdev2_t *netdev = (netdev2_t*) &at86rf2xx_dev;
		radio_init(transmit_buf, _tmp_buf);
		netdev->driver->init(netdev);
		netdev->event_callback = _event_cb;
		set_netdev(netdev);
#endif
}

