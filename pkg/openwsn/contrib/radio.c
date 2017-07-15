/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
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
 *
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */
#include <sys/uio.h>

#include "radio.h"
#include "leds.h"
#include "debugpins.h"
#include "sctimer.h"

#include "openwsn.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"

#define ENABLE_DEBUG                (0)
#include "debug.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
static at86rf2xx_t at86rf2xx_dev;
#endif

typedef struct {
    radio_capture_cbt startFrame_cb;
    radio_capture_cbt endFrame_cb;
    radio_state_t state;
    netdev_t                 *dev;
} radio_vars_t;

radio_vars_t radio_vars;

static void _event_cb(netdev_t *dev, netdev_event_t event);

void radio_init(void)
{

    DEBUG("OW initialize riot-adaptation\n");

    uint16_t dev_type;
    memset(&radio_vars, 0, sizeof(radio_vars_t));
    radio_vars.state = RADIOSTATE_STOPPED;

#ifdef MODULE_AT86RF2XX
    radio_vars.dev = (netdev_t *)&at86rf2xx_dev.netdev.netdev;
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
#endif

    DEBUG("OW initialize RIOT radio (netdev)\n");
    radio_vars.dev->driver->init(radio_vars.dev);
    radio_vars.dev->event_callback = _event_cb;
    if (radio_vars.dev->driver->get(radio_vars.dev, NETOPT_DEVICE_TYPE, &dev_type,
                                    sizeof(dev_type)) < 0) {
        DEBUG("OW couldn't get device type\n");
    }

    netopt_state_t state = NETOPT_STATE_STANDBY;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));

    DEBUG("OW RIOT radio (netdev) set default options\n");
    netopt_enable_t enable;
    /* Enable needed IRQs */
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RX_START_IRQ, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RX_END_IRQ, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_TX_END_IRQ, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_DISABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_AUTOACK, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_DISABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_CSMA, &(enable), sizeof(netopt_enable_t));
    /* Enable TX with preloading */
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_PRELOADING, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RAWMODE, &(enable), sizeof(netopt_enable_t));
    uint8_t retrans = 0;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RETRANS, &(retrans), sizeof(uint8_t));
#ifdef PANID_DEFINED
    uint16_t pan_default = PANID_DEFINED;
#else
    uint16_t pan_default = 0xcafe;
#endif
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_NID, &(pan_default), sizeof(uint16_t));

    radio_vars.state = RADIOSTATE_RFOFF;
}

void radio_setStartFrameCb(radio_capture_cbt cb)
{
    radio_vars.startFrame_cb = cb;
}

void radio_setEndFrameCb(radio_capture_cbt cb)
{
    radio_vars.endFrame_cb = cb;
}

void radio_reset(void)
{
    netopt_state_t state = NETOPT_STATE_RESET;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));

    state = NETOPT_STATE_STANDBY;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
}

void radio_setFrequency(uint8_t frequency)
{
    radio_vars.state = RADIOSTATE_SETTING_FREQUENCY;

    uint16_t chan = frequency;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_CHANNEL, &(chan), sizeof(chan));

    radio_vars.state = RADIOSTATE_FREQUENCY_SET;
}

void radio_rfOn(void)
{
    netopt_state_t state = NETOPT_STATE_STANDBY;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
}

void radio_rfOff(void)
{
    netopt_state_t state = NETOPT_STATE_STANDBY;

    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
    leds_radio_off();

    radio_vars.state = RADIOSTATE_RFOFF;
}

void radio_loadPacket(uint8_t *packet, uint16_t len)
{
    DEBUG("OW radio_loadPacket\n");
    /* NETOPT_PRELOADING w as enabled in the init function so this
       simply loads data to the device buffer */
    iolist_t pkt = {
        .iol_base = (void *)packet,
        .iol_len  = (size_t)(len - 2),   /* FCS is written by driver */
    };
    if (radio_vars.dev->driver->send(radio_vars.dev, &pkt) < 0) {
        DEBUG("OW couldn't load pkt\n");
    }

    radio_vars.state = RADIOSTATE_PACKET_LOADED;
}

void radio_txEnable(void)
{
    DEBUG("radio_txEnable\n");

    radio_vars.state = RADIOSTATE_ENABLING_TX;

    leds_radio_on();

    radio_vars.state = RADIOSTATE_TX_ENABLED;
}
void radio_txNow(void)
{

    PORT_TIMER_WIDTH val;

    netopt_state_t state = NETOPT_STATE_TX;

    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &state, sizeof(netopt_state_t));

    if (radio_vars.startFrame_cb != NULL) {
        val = sctimer_readCounter();
        radio_vars.startFrame_cb(val);
    }
}

void radio_rxEnable(void)
{
    DEBUG("radio_rxEnable\n");

    radio_vars.state = RADIOSTATE_ENABLING_RX;

    leds_radio_on();

    netopt_state_t state = NETOPT_STATE_IDLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));

    radio_vars.state = RADIOSTATE_LISTENING;
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
    netdev_ieee802154_rx_info_t rx_info;

    int bytes_expected = radio_vars.dev->driver->recv(radio_vars.dev, bufRead, maxBufLen, &rx_info);

    if (bytes_expected) {
        netdev_ieee802154_rx_info_t rx_info;
        DEBUG("ow_netdev: received frame of size %i\n", bytes_expected);
        int nread = radio_vars.dev->driver->recv(radio_vars.dev, bufRead, bytes_expected, &rx_info);
        *crc = true;
        if (nread <= 2) {
            *crc = false;
            return;
        }
        *lenRead = nread + 2;

        *lqi = 0;
        *rssi = 0;

        /* TODO: check CRC */
        *crc = true;
    }
    else {
        *crc = false;
    }
}

PORT_TIMER_WIDTH capturedTime = 0u;
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        /* capture the time */
        debugpins_isr_set();
        capturedTime = sctimer_readCounter();
        radio_vars.dev->driver->isr(radio_vars.dev);
        debugpins_isr_clr();
    }
    else {
        DEBUG("ow_netdev: event triggered -> %i\n", event);
        assert( capturedTime != 0u);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED:
                radio_vars.startFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_RX_STARTED\n");
                break;
            case NETDEV_EVENT_RX_COMPLETE:
                radio_vars.endFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_RX_COMPLETE\n");
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                radio_vars.endFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_TX_COMPLETE\n");
                break;
            default:
                break;
        }
    }
}
