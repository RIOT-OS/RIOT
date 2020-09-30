/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "radio" bsp module
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>
#include <sys/uio.h>

#include "leds.h"
#include "debugpins.h"
#include "sctimer.h"
#include "idmanager.h"

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"

#include "openwsn.h"
#include "openwsn_radio.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#define LOG_LEVEL LOG_NONE
#include "log.h"

openwsn_radio_t openwsn_radio;

static void _event_cb(netdev_t *dev, netdev_event_t event);

/* stores the NETDEV_EVENT_ISR capture time to tag the following NETDEV_EVENT */
static PORT_TIMER_WIDTH _txrx_event_capture_time = 0;

static void _set_addr(void)
{
    netdev_t* dev = openwsn_radio.dev;
    /* Initiate Id manager here and not in `openstack_init` function to allow
       overriding the short id address before additional stack components are
       initiated */
    idmanager_init();

    /* override 16b address to avoid short address collision */
    uint8_t addr[IEEE802154_SHORT_ADDRESS_LEN];
    dev->driver->get(dev, NETOPT_ADDRESS, addr, IEEE802154_SHORT_ADDRESS_LEN);
    open_addr_t id;
    id.type = ADDR_16B;
    memcpy(&id.addr_16b, addr, IEEE802154_SHORT_ADDRESS_LEN);
    idmanager_setMyID(&id);
    /* override PANID */
    id.type = ADDR_PANID;
    uint16_t panid = OPENWSN_PANID;
    memcpy(&id.addr_16b, &panid, IEEE802154_SHORT_ADDRESS_LEN);
    idmanager_setMyID(&id);
}

int openwsn_radio_init(void *radio_dev)
{
    assert(radio_dev);
    netdev_t *netdev = (netdev_t *)radio_dev;

    LOG_DEBUG("[openwsn/radio]: initialize riot-adaptation\n");
    openwsn_radio.dev = netdev;

    if (netdev->driver->init(netdev)) {
        LOG_ERROR("[openwsn/radio]: unable to initialize device\n");
        return -1;
    }
    netdev->event_callback = _event_cb;

    LOG_DEBUG("[openwsn/radio]: put radio in standby\n");
    netopt_state_t state = NETOPT_STATE_STANDBY;
    netdev->driver->set(netdev, NETOPT_STATE, &(state), sizeof(state));

    LOG_DEBUG("[openwsn/radio]: set needed netdev options\n");
    netopt_enable_t enable;
    /* Enable needed IRQs */
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_TX_START_IRQ, &(enable), sizeof(enable));
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_RX_START_IRQ, &(enable), sizeof(enable));
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &(enable), sizeof(enable));
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_TX_END_IRQ, &(enable), sizeof(enable));
    enable = NETOPT_DISABLE;
    /* Enable basic mode, no AUTOACK. no CSMA , no frame filtering */
    netdev->driver->set(netdev, NETOPT_AUTOACK, &(enable), sizeof(enable));
    enable = NETOPT_DISABLE;
    netdev->driver->set(netdev, NETOPT_CSMA, &(enable), sizeof(enable));
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_RAWMODE, &(enable), sizeof(enable));
    uint8_t retrans = 0;
    /* MAC layer will handle retransmissions */
    netdev->driver->set(netdev, NETOPT_RETRANS, &(retrans), sizeof(uint8_t));
    /* Enable TX with preloading */
    enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_PRELOADING, &(enable), sizeof(enable));
    /* Set default PANID */
    uint16_t panid = OPENWSN_PANID;
    netdev->driver->set(netdev, NETOPT_NID, &(panid), sizeof(uint16_t));

    _set_addr();

    return 0;
}

void radio_setStartFrameCb(radio_capture_cbt cb)
{
    openwsn_radio.startFrame_cb = cb;
}

void radio_setEndFrameCb(radio_capture_cbt cb)
{
    openwsn_radio.endFrame_cb = cb;
}

void radio_reset(void)
{
    netopt_state_t state = NETOPT_STATE_RESET;

    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &(state),
                                   sizeof(netopt_state_t));

    state = NETOPT_STATE_STANDBY;
    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &(state),
                                   sizeof(netopt_state_t));
}

void radio_setFrequency(uint8_t frequency, radio_freq_t tx_or_rx)
{
    (void)tx_or_rx;

    uint16_t chan = frequency;
    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_CHANNEL, &(chan),
                                   sizeof(chan));
}

void radio_rfOn(void)
{
    netopt_state_t state = NETOPT_STATE_IDLE;

    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &(state),
                                   sizeof(netopt_state_t));
}

void radio_rfOff(void)
{
    netopt_state_t state = NETOPT_STATE_STANDBY;

    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &(state),
                                   sizeof(netopt_state_t));

    debugpins_radio_clr();
    leds_radio_off();
}

void radio_loadPacket(uint8_t *packet, uint16_t len)
{
    /* NETOPT_PRELOADING is enabled in radio_init so this will only load the
       packet */
    /* OpenWSN `len` accounts for the FCS field which is set by default by
       netdev, so remove from the actual packet `len` */
    iolist_t pkt = {
        .iol_base = (void *)packet,
        .iol_len = (size_t)(len - IEEE802154_FCS_LEN),
    };

    if (openwsn_radio.dev->driver->send(openwsn_radio.dev, &pkt) < 0) {
        LOG_DEBUG("[openwsn/radio]: couldn't load pkt\n");
    }
    LOG_DEBUG("[openwsn/radio]: loaded radio packet\n");
}

void radio_txEnable(void)
{
    debugpins_radio_set();
    leds_radio_on();
}

void radio_txNow(void)
{
    netopt_state_t state = NETOPT_STATE_TX;

    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &state,
                                   sizeof(netopt_state_t));
}

void radio_rxEnable(void)
{
    debugpins_radio_set();
    leds_radio_on();
    netopt_state_t state = NETOPT_STATE_IDLE;
    openwsn_radio.dev->driver->set(openwsn_radio.dev, NETOPT_STATE, &(state),
                                   sizeof(state));
}

void radio_rxNow(void)
{
    /* nothing to do */
}

void radio_getReceivedFrame(uint8_t *bufRead,
                            uint8_t *lenRead,
                            uint8_t maxBufLen,
                            int8_t *rssi,
                            uint8_t *lqi,
                            bool *crc)
{
    /* OpenWSN packets are 130 bytes to hold all required data for an SPI
       transaction since in some implementations it's used directly in the SPI
       shift register:
            - 1B spi address, 1B length, 125B data, 2B CRC, 1B LQI
       In RIOT we don't do this so maxBufLen is irrelevant, packet size will
       always be enough to hold IEEE802154_FRAME_LEN_MAX, but in practice only
       125B of data are copied into bufRead.
     */
    (void)maxBufLen;
    netdev_ieee802154_rx_info_t rx_info;

    int bytes_expected = openwsn_radio.dev->driver->recv(openwsn_radio.dev,
                                                         NULL, 0,
                                                         NULL);

    if (bytes_expected < (int)(IEEE802154_ACK_FRAME_LEN - IEEE802154_FCS_LEN)) {
        /* drop invalid packet */
        openwsn_radio.dev->driver->recv(openwsn_radio.dev, NULL, bytes_expected,
                                        NULL);
        radio_rxEnable();
        return;
    }

    int nread = openwsn_radio.dev->driver->recv(openwsn_radio.dev, bufRead,
                                                bytes_expected, &rx_info);

    /* FCS is skipped by netdev in the returned length, but OpenWSN includes
       IEEE802154_FCS_LEN in its length value */
    *lenRead = nread + IEEE802154_FCS_LEN;

    /* get rssi, lqi & crc */
    *rssi = rx_info.rssi;
    *lqi = rx_info.lqi;
    /* only valid crc frames are currently accepted */
    *crc = 1;

    radio_rxEnable();
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    (void)dev;

    if (event == NETDEV_EVENT_ISR) {
        /* capture the time */
        debugpins_isr_set();
        _txrx_event_capture_time = sctimer_readCounter();
        openwsn_radio.dev->driver->isr(openwsn_radio.dev);
        debugpins_isr_clr();
    }
    else {
        LOG_DEBUG("[openwsn/radio]: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED:
                openwsn_radio.startFrame_cb(_txrx_event_capture_time);
                LOG_DEBUG("[openwsn/radio]: NETDEV_EVENT_RX_STARTED\n");
                break;
            case NETDEV_EVENT_TX_STARTED:
                openwsn_radio.startFrame_cb(_txrx_event_capture_time);
                LOG_DEBUG("[openwsn/radio]: NETDEV_EVENT_TX_STARTED\n");
                break;
            case NETDEV_EVENT_RX_COMPLETE:
                openwsn_radio.endFrame_cb(_txrx_event_capture_time);
                LOG_DEBUG("[openwsn/radio]: NETDEV_EVENT_RX_COMPLETE\n");
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                openwsn_radio.endFrame_cb(_txrx_event_capture_time);
                LOG_DEBUG("[openwsn/radio]: NETDEV_EVENT_TX_COMPLETE\n");
                break;
            default:
                break;
        }
    }
}
