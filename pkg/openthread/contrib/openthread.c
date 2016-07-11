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


#include <cli/cli-serial.h>
#include "xtimer.h"
#include "random.h"
#include "periph/cpuid.h"

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define OPENTHREAD_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif



#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

#define OPENTHREAD_NETDEV2_BUFLEN (ETHERNET_MAX_LEN)


static uint8_t rx_buf[OPENTHREAD_NETDEV2_BUFLEN];
static uint8_t tx_buf[OPENTHREAD_NETDEV2_BUFLEN];

static char ot_thread_stack[3*THREAD_STACKSIZE_MAIN];

void openthread_bootstrap(void)
{
#ifdef CPUID_LEN
    char cpu_id[CPUID_LEN];
    cpuid_get(cpu_id);
    int seed = 0;
    for(int i=0;i<(int) CPUID_LEN;i++)
    {
        seed += cpu_id[i];
    }
    random_init(seed);
#else
    random_init(0);
#endif

#ifdef MODULE_AT86RF2XX
	at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
	netdev2_t *netdev = (netdev2_t*) &at86rf2xx_dev;
#endif

	netdev->driver->init(netdev);
	netdev->event_callback = _event_cb;

	openthread_radio_init(netdev, tx_buf, rx_buf);
    openthread_netdev2_init(ot_thread_stack, sizeof(ot_thread_stack), THREAD_PRIORITY_MAIN - 1, "ot_thread");
}

