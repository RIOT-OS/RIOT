/*
 * Copyright (C) 2020 Inria
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
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */
#include <sys/uio.h>

#include "leds.h"
#include "debugpins.h"
#include "sctimer.h"
#include "idmanager.h"
#include "eui64.h"

#include "byteorder.h"

#include "luid.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#include "openwsn.h"
#include "openwsn_radio.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

openwsn_radio_t openwsn_radio;

/* stores the event capture time */
static PORT_TIMER_WIDTH _txrx_event_capture_time = 0;

void _idmanager_addr_override(void)
{
    /* Initiate Id manager here and not in `openstack_init` function to
       allow overriding the short id address before additional stack
       components are initiated */
    idmanager_init();

    /* override 16b address to avoid short address collision */
    network_uint16_t short_addr;
    luid_get_short(&short_addr);
    open_addr_t id;
    id.type = ADDR_16B;
    memcpy(&id.addr_16b, short_addr.u8, IEEE802154_SHORT_ADDRESS_LEN);
    idmanager_setMyID(&id);

    /* override PANID */
    id.type = ADDR_PANID;
    network_uint16_t panid_be = byteorder_htons(OPENWSN_PANID);
    memcpy(&id.panid, &panid_be, IEEE802154_SHORT_ADDRESS_LEN);
    idmanager_setMyID(&id);

    /* recover ADDR_64B */
    eui64_t eui64;
    eui64_get(eui64.uint8);

    /* Set all IEEE addresses */
    uint16_t panid = OPENWSN_PANID;
    ieee802154_radio_set_hw_addr_filter(openwsn_radio.dev, &short_addr,
                                        &eui64, &panid);

}

static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    (void)dev;

    debugpins_isr_set();
    _txrx_event_capture_time = sctimer_readCounter();
    debugpins_isr_clr();

    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        ieee802154_radio_confirm_transmit(openwsn_radio.dev, NULL);
        ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                               IEEE802154_TRX_STATE_TRX_OFF);
        while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) == -EAGAIN) {}
        openwsn_radio.endFrame_cb(_txrx_event_capture_time);
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        openwsn_radio.endFrame_cb(_txrx_event_capture_time);
        break;
    case IEEE802154_RADIO_INDICATION_TX_START:
        openwsn_radio.startFrame_cb(_txrx_event_capture_time);
        break;
    case IEEE802154_RADIO_INDICATION_RX_START:
        openwsn_radio.startFrame_cb(_txrx_event_capture_time);
        break;
    default:
        break;
    }
}

int openwsn_radio_init(void *radio_dev)
{
    assert(radio_dev);
    ieee802154_dev_t *dev = (ieee802154_dev_t *)radio_dev;

    LOG_DEBUG("[openwsn/radio]: initialize riot-adaptation\n");
    openwsn_radio.dev = dev;

    /* override short_address and panid*/
    _idmanager_addr_override();

    if (ieee802154_radio_request_on(dev)) {
        LOG_ERROR("[openwsn/radio]: unable to initialize device\n");
        return -1;
    }

    /* Set the Event Notification */
    dev->cb = _hal_radio_cb;

    /* If the radio is still not in TRX_OFF state, spin */
    while (ieee802154_radio_confirm_on(dev) == -EAGAIN) {}

    /* Enable basic mode, no AUTOACK. no CSMA */
    ieee802154_radio_set_rx_mode(dev, IEEE802154_RX_AACK_DISABLED);
    /* MAC layer retransmissions are disabled by _set_csma_params() */
    ieee802154_radio_set_csma_params(dev, NULL, -1);

    if (IS_USED(MODULE_CC2538_RF)) {
        /* If frame filtering is enabled cc2538 will not accept beacons
        where the destination-address mode is 0 (no destination address).
        per rfc8180 4.5.1 the destination address must be set, which means
        the destination-address mode can't be 0 */
        ieee802154_radio_set_rx_mode(dev, IEEE802154_RX_PROMISC);
    }

    /* Configure PHY settings (channel, TX power) */
    ieee802154_phy_conf_t conf =
    { .channel = CONFIG_IEEE802154_DEFAULT_CHANNEL,
      .page = CONFIG_IEEE802154_DEFAULT_CHANNEL,
      .pow = CONFIG_IEEE802154_DEFAULT_TXPOWER };

    ieee802154_radio_config_phy(dev, &conf);

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
    /* TODO: this is not handled correctly since not all radios implement
       this */
    ieee802154_radio_off(openwsn_radio.dev);
    ieee802154_radio_request_on(openwsn_radio.dev);
    /* If the radio is still not in TRX_OFF state, spin */
    while (ieee802154_radio_confirm_on(openwsn_radio.dev) == -EAGAIN) {}
}

void radio_setFrequency(uint8_t frequency, radio_freq_t tx_or_rx)
{
    (void)tx_or_rx;

    ieee802154_phy_conf_t conf =
    { .channel = frequency,
      .page = CONFIG_IEEE802154_DEFAULT_CHANNEL,
      .pow = CONFIG_IEEE802154_DEFAULT_TXPOWER };

    ieee802154_radio_config_phy(openwsn_radio.dev, &conf);
}

void radio_rfOn(void)
{
    ieee802154_radio_request_on(openwsn_radio.dev);
    /* If the radio is still not in TRX_OFF state, spin */
    while (ieee802154_radio_confirm_on(openwsn_radio.dev) == -EAGAIN) {}
    /* HACK: cc2538 does not implement on() correctly, remove when it does*/
    ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                           IEEE802154_TRX_STATE_TRX_OFF);
    while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) == -EAGAIN) {}
}

void radio_rfOff(void)
{
    /* radio_rfOff is called in the middle of a slot and is not always
       followed by an `radio_rfOn`, so don't call `ieee802154_radio_off`
       and only send the radio to `TrxOFF` instead */
    int ret = ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                                     IEEE802154_TRX_STATE_TRX_OFF);

    if (ret) {
        LOG_ERROR("[openwsn/radio]: request_set_trx_state failed %s\n",
                  __FUNCTION__);
    }
    else {
        debugpins_radio_clr();
        leds_radio_off();
        while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) ==
               -EAGAIN) {}
    }

}

void radio_loadPacket(uint8_t *packet, uint16_t len)
{
    /* OpenWSN `len` accounts for the FCS field which is set by default by
       netdev, so remove from the actual packet `len` */
    iolist_t pkt = {
        .iol_base = (void *)packet,
        .iol_len = (size_t)(len - IEEE802154_FCS_LEN),
    };

    if (ieee802154_radio_write(openwsn_radio.dev, &pkt) < 0) {
        LOG_ERROR("[openwsn/radio]: couldn't load pkt\n");
    }
}

void radio_txEnable(void)
{
    int ret = ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                                     IEEE802154_TRX_STATE_TX_ON);

    if (ret) {
        LOG_ERROR("[openwsn/radio]: request_set_trx_state failed %s\n",
                  __FUNCTION__);
    }
    else {
        while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) ==
               -EAGAIN) {}
        debugpins_radio_set();
        leds_radio_on();
    }
}

void radio_txNow(void)
{
    int ret = ieee802154_radio_request_transmit(openwsn_radio.dev);

    if (ret) {
        LOG_ERROR("[openwsn/radio]: request_set_trx_state failed %s\n",
                  __FUNCTION__);
    }
    else {
        /* Trigger startFrame manually if no IEEE802154_CAP_IRQ_TX_START */
        if (!ieee802154_radio_has_irq_tx_start(openwsn_radio.dev)) {
            debugpins_isr_set();
            _txrx_event_capture_time = sctimer_readCounter();
            debugpins_isr_clr();
            openwsn_radio.startFrame_cb(_txrx_event_capture_time);
        }
    }
}

void radio_rxEnable(void)
{
    int ret = ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                                     IEEE802154_TRX_STATE_TRX_OFF);

    if (ret) {
        LOG_ERROR("[openwsn/radio]: request_set_trx_state failed %s\n",
                  __FUNCTION__);
    }
    else {
        while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) ==
               -EAGAIN) {}
        debugpins_radio_set();
        leds_radio_on();
    }
}

void radio_rxNow(void)
{
    int ret = ieee802154_radio_request_set_trx_state(openwsn_radio.dev,
                                                     IEEE802154_TRX_STATE_RX_ON);

    if (ret) {
        LOG_ERROR("[openwsn/radio]: request_set_trx_state failed %s\n",
                  __FUNCTION__);
    }
    else {
        while (ieee802154_radio_confirm_set_trx_state(openwsn_radio.dev) ==
               -EAGAIN) {}
    }
}

void radio_getReceivedFrame(uint8_t *bufRead,
                            uint8_t *lenRead,
                            uint8_t maxBufLen,
                            int8_t *rssi,
                            uint8_t *lqi,
                            bool *crc)
{
    ieee802154_rx_info_t rx_info;
    size_t size = ieee802154_radio_read(openwsn_radio.dev, bufRead,
                                        maxBufLen, &rx_info);

    /* FCS is skipped by the radio-hal in the returned length, but
       OpenWSN includes IEEE802154_FCS_LEN in its length value */
    *lenRead = size + IEEE802154_FCS_LEN;
    /* get rssi, lqi & crc */
    *rssi = rx_info.rssi;
    *lqi = rx_info.lqi;
    /* only valid crc frames are currently accepted */
    *crc = 1;
}
