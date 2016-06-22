
#include "thread.h"
#include <assert.h>

#ifdef MODULE_NETDEV2_TAP
#include "netdev2_tap.h"
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#include "openthread.h"
#include "ot.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define OPENTHREAD_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif


#ifdef MODULE_NETDEV2_TAP
#define LWIP_NETIF_NUMOF        (1)
#endif


#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif



#define OPENTHREAD_NETDEV2_BUFLEN (ETHERNET_MAX_LEN)

#ifdef MODULE_NETDEV2_TAP
extern netdev2_tap_t netdev2_tap;
#endif

void otSignalTaskletPending(void)
{
	//printf("Tasklet pending\n");
}


static uint8_t _tmp_buf[OPENTHREAD_NETDEV2_BUFLEN];
static uint8_t transmit_buf[OPENTHREAD_NETDEV2_BUFLEN];

static kernel_pid_t _pid;

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
				recv_pkt(dev, _tmp_buf);
				break;
			case NETDEV2_EVENT_TX_COMPLETE:
				send_pkt(dev);
				break;
			case NETDEV2_EVENT_TX_NOACK:
				otPlatRadioTransmitDone(false, kThreadError_NoAck);
				break;
			case NETDEV2_EVENT_TX_MEDIUM_BUSY:
				otPlatRadioTransmitDone(false, kThreadError_ChannelAccessFailure);
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
#endif

#ifdef MODULE_NETDEV2_TAP
	netdev2_t *netdev = (netdev2_t*) &netdev2_tap;
#endif
		netdev->driver->init(netdev);
		netdev->event_callback = _event_cb;
		radio_init(transmit_buf);
		set_netdev(netdev);
}

