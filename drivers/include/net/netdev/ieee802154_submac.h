#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154/radio.h"
#include "xtimer.h"

#include "od.h"
#include "event/thread.h"
#include "event/callback.h"
#include "xtimer.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif


#define NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT (1<<0)
#define NETDEV_SUBMAC_FLAGS_TX_DONE (1<<1)
#define NETDEV_SUBMAC_FLAGS_RX_DONE (1<<2)

typedef struct {
    netdev_ieee802154_t dev;
    ieee802154_submac_t submac;
    void *buf;
    size_t size;
    netdev_ieee802154_rx_info_t info;
    xtimer_t ack_timer;
    int isr_flags;
} netdev_ieee802154_submac_t;

int netdev_ieee802154_submac_init(netdev_ieee802154_submac_t *netdev_submac,
                                  ieee802154_dev_t *dev);
