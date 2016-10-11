#include "thread.h"
#include <assert.h>

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_KW2XRF
#include "kw2xrf.h"
#include "kw2xrf_params.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "platform/alarm.h"
#include "platform/uart.h"


#include <cli/cli-uart.h>
#include "xtimer.h"
#include "random.h"

#include "openthread.h"
#include "mutex.h"
#include "ot.h"


#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define OPENTHREAD_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif

#ifdef MODULE_KW2XRF
#define OPENTHREAD_NETIF_NUMOF        (sizeof(kw2xrf_params) / sizeof(kw2xrf_params[0]))
#endif

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

#ifdef MODULE_KW2XRF
static kw2xrf_t kw2xrf_dev;
#endif

#define OPENTHREAD_NETDEV2_BUFLEN (ETHERNET_MAX_LEN)

static uint8_t rx_buf[OPENTHREAD_NETDEV2_BUFLEN];
static uint8_t tx_buf[OPENTHREAD_NETDEV2_BUFLEN];
static char ot_thread_stack[2*THREAD_STACKSIZE_MAIN];


static mutex_t mtx = MUTEX_INIT;

/* call this when calling an OpenThread function */
void begin_mutex(void)
{
	mutex_lock(&mtx);
}

/* call this after an OpenThread function is called */
void end_mutex(void)
{
	mutex_unlock(&mtx);
}

void openthread_bootstrap(void)
{
	/* init random */
	ot_random_init();

	/* setup netdev modules */
#ifdef MODULE_AT86RF2XX
	at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
	netdev2_t *netdev = (netdev2_t*) &at86rf2xx_dev;
#endif

#ifdef MODULE_KW2XRF
	kw2xrf_setup(&kw2xrf_dev, &kw2xrf_params[0]);
	netdev2_t *netdev = (netdev2_t*) &kw2xrf_dev;
#endif

	openthread_radio_init(netdev, tx_buf, rx_buf);
    openthread_netdev2_init(ot_thread_stack, sizeof(ot_thread_stack), THREAD_PRIORITY_MAIN - 1, "ot_thread", netdev);
}

