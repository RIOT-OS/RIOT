#include "net/netdev/ieee802154_submac.h"
#include "event/thread.h"

static const ieee802154_submac_cb_t _cb;

static const netdev_driver_t netdev_submac_driver;

static void _ack_timeout(void *arg)
{
    (void) arg;
    netdev_ieee802154_submac_t *netdev_submac = arg;
    netdev_t *netdev = arg;
    netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT;

    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt,
                                         value, max_len);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t*) netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;

    int res;
    int16_t tx_power;

    switch (opt) {
        case NETOPT_ADDRESS:
           ieee802154_set_short_addr(submac, (void*) value);
           break;
        case NETOPT_ADDRESS_LONG:
           ieee802154_set_ext_addr(submac, (void*) value);
           break;
        case NETOPT_NID:
           ieee802154_set_panid(submac, *((uint16_t*) value));
           break;
        case NETOPT_CHANNEL:
           ieee802154_set_channel_number(submac, *((uint16_t*) value));
           break;
        case NETOPT_TX_POWER:
           tx_power = *((int16_t*) value);
           res = ieee802154_set_tx_power(submac, tx_power);
           if (res >= 0) {
               netdev_submac->dev.txpower = tx_power;
           }
           return res;
        default:
           break;
    }

    return netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt,
                                     value, value_len);
}

void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac, uint16_t us)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac, netdev_ieee802154_submac_t, submac);
    xtimer_set(&netdev_submac->ack_timer, us);
}

void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac, netdev_ieee802154_submac_t, submac);
    xtimer_remove(&netdev_submac->ack_timer);
}

static int _send(netdev_t *netdev, const iolist_t *pkt)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t*) netdev;
    return ieee802154_send(&netdev_submac->submac, pkt);
}

static void _isr(netdev_t *netdev)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t*) netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;
    int flags;

    do {
        irq_disable();
        flags = netdev_submac->isr_flags;
        netdev_submac->isr_flags = 0;
        irq_enable();

        if (flags & NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT) {
            ieee802154_submac_ack_timeout_fired(&netdev_submac->submac);
        }

        if (flags & NETDEV_SUBMAC_FLAGS_TX_DONE) {
            ieee802154_submac_tx_done_cb(&netdev_submac->submac);
        }

        if (flags & NETDEV_SUBMAC_FLAGS_RX_DONE) {
            ieee802154_submac_rx_done_cb(submac);
        }
    }
    while(netdev_submac->isr_flags != 0);
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t*) netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;
    ieee802154_rx_info_t rx_info;

    if (buf == NULL && len == 0) {
        return ieee802154_get_frame_length(submac);
    }

    int res = ieee802154_read_frame(submac, buf, len, &rx_info);

    if (info) {
        netdev_ieee802154_rx_info_t *netdev_rx_info = info;
        netdev_rx_info->rssi = rx_info.rssi;
        netdev_rx_info->lqi = rx_info.lqi;
    }

    return res;
}

static void submac_tx_done(ieee802154_submac_t *submac, int status, ieee802154_tx_info_t *info)
{
    (void) status;
    (void) info;
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac, netdev_ieee802154_submac_t, submac);
    netdev_t *netdev = (netdev_t*) netdev_submac;

    switch(status) {
        case TX_STATUS_SUCCESS:
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
            break;
        case TX_STATUS_FRAME_PENDING:
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE_DATA_PENDING);
            break;
        case TX_STATUS_MEDIUM_BUSY:
            netdev->event_callback(netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
            break;
        case TX_STATUS_NO_ACK:
            netdev->event_callback(netdev, NETDEV_EVENT_TX_NOACK);
            break;
        default:
            break;
    }
}

static void submac_rx_done(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac, netdev_ieee802154_submac_t, submac);
    netdev_t *netdev = (netdev_t*) netdev_submac;

    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static const ieee802154_submac_cb_t _cb= {
    .rx_done = submac_rx_done,
    .tx_done = submac_tx_done,
};
            
/* Event Notification callback */
static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    ieee802154_submac_t *submac = dev->ctx;
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac, netdev_ieee802154_submac_t, submac);
    netdev_t *netdev = (netdev_t*) netdev_submac;
    switch(status) {
        case IEEE802154_RADIO_CONFIRM_TX_DONE:
            netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_TX_DONE;
            break;
        case IEEE802154_RADIO_INDICATION_RX_DONE:
            netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_RX_DONE;
        default:
           break;
    }
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static int _init(netdev_t *netdev)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t*) netdev;
    /* Call the init function of the device (this will be handled by
     * `auto_init`) */

    ieee802154_submac_t *submac = &netdev_submac->submac;
    ieee802154_submac_init(submac);

    netdev_ieee802154_t *netdev_ieee802154 = (netdev_ieee802154_t*) netdev;

    /* This function already sets the PAN ID to the default one */
    netdev_ieee802154_reset(netdev_ieee802154);

    uint16_t chan = CONFIG_IEEE802154_DEFAULT_CHANNEL;
    int16_t tx_power = CONFIG_IEEE802154_DEFAULT_TXPOWER;

    /* Initialise netdev_ieee802154_t struct */
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_CHANNEL,
                          &chan, sizeof(chan));
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_ADDRESS,
                          &submac->short_addr, sizeof(submac->short_addr));
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_ADDRESS_LONG,
                          &submac->ext_addr, sizeof(submac->ext_addr));

    netdev_submac->dev.txpower = tx_power;

    return 0;
}

int netdev_ieee802154_submac_init(netdev_ieee802154_submac_t *netdev_submac, ieee802154_dev_t *dev)
{
    netdev_t *netdev = (netdev_t*) netdev_submac;
    netdev->driver = &netdev_submac_driver;
    ieee802154_submac_t *submac = &netdev_submac->submac;
    submac->dev = dev;
    submac->cb = &_cb;
    submac->dev->ctx = submac;

    /* Set the Event Notification */
    submac->dev->cb = _hal_radio_cb;

    netdev_submac->ack_timer.callback = _ack_timeout;
    netdev_submac->ack_timer.arg = netdev_submac;
    
    return 0;
}

static const netdev_driver_t netdev_submac_driver = {
    .get  = _get,
    .set  = _set,
    .send = _send,
    .recv = _recv,
    .isr  = _isr,
    .init = _init,
};

