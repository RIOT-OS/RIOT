/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <stdint.h>
#include <sys/uio.h>

#include "msg.h"
#include "net/netdev2.h"

#include "evproc.h"
#include "emb6.h"
#include "linkaddr.h"
#include "packetbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

extern uip_lladdr_t uip_lladdr;

static netdev2_t *_dev = NULL;
static s_nsLowMac_t *_lowmac = NULL;
static int8_t _rssi_base_value = -100;
static uint8_t _last_rssi;

static int8_t _netdev2_init(s_ns_t *p_ns);
static int8_t _netdev2_send(const void *pr_payload, uint8_t c_len);
static int8_t _netdev2_on(void);
static int8_t _netdev2_off(void);
static void _netdev2_set_txpower(int8_t power);
static int8_t _netdev2_get_txpower(void);
static void _netdev2_set_sensitivity(int8_t sens);
static int8_t _netdev2_get_sensitivity(void);
static int8_t _netdev2_get_rssi(void);
static void _netdev2_set_promisc(uint8_t c_on_off);

const s_nsIf_t emb6_netdev2_driver = {
    .name               = "netdev2",
    .init               = &_netdev2_init,
    .send               = &_netdev2_send,
    .on                 = &_netdev2_on,
    .off                = &_netdev2_off,
    .set_txpower        = &_netdev2_set_txpower,
    .get_txpower        = &_netdev2_get_txpower,
    .set_sensitivity    = &_netdev2_set_sensitivity,
    .get_sensitivity    = &_netdev2_get_sensitivity,
    .get_rssi           = &_netdev2_get_rssi,
    .ant_div            = NULL,
    .ant_rf_switch      = NULL,
    .set_promisc        = &_netdev2_set_promisc,
};

static void _get_recv_pkt(void)
{
    char *dataptr;
    struct netdev2_radio_rx_info rx_info;
    int8_t len;

    packetbuf_clear();

    dataptr = packetbuf_dataptr();
    len = _dev->driver->recv(_dev, dataptr, PACKETBUF_SIZE, &rx_info);
    _last_rssi = rx_info.rssi;

    if ((len > 0) && (_lowmac != NULL)) {
        packetbuf_set_datalen(len);
        _lowmac->input();
    }
}

static void _event_cb(netdev2_t *dev, netdev2_event_t event, void *arg)
{
    (void)arg;
    if (event == NETDEV2_EVENT_ISR) {
        /* EVENT_TYPE_PCK_LL is supposed to be used by drivers, so use it
         * (though NETDEV2_EVENT_ISR technically doesn't only signify
         * incoming packets) */
        evproc_putEvent(E_EVPROC_HEAD, EVENT_TYPE_PCK_LL, NULL);
    }
    else {
        switch (event) {
            case NETDEV2_EVENT_RX_COMPLETE: {
                _get_recv_pkt();
            }
            break;
            default:
                break;
        }
    }
}

static void _emb6_netdev2_callback(c_event_t c_event, p_data_t p_data)
{
    (void)p_data;
    if (c_event == EVENT_TYPE_PCK_LL) {
        _dev->driver->isr(_dev);
    }
}

int emb6_netdev2_setup(netdev2_t *dev)
{
    if (_dev == NULL) {
        _dev = dev;
        return 0;
    }
    return -1;
}

static int8_t _netdev2_init(s_ns_t *p_ns)
{
    if ((_dev != NULL) && (p_ns != NULL) && (p_ns->lmac != NULL)) {
        _dev->event_callback = _event_cb;
        _dev->driver->get(_dev, NETOPT_ADDRESS_LONG, &mac_phy_config.mac_address,
                          sizeof(mac_phy_config.mac_address));
        memcpy(&uip_lladdr, mac_phy_config.mac_address,
               sizeof(mac_phy_config.mac_address));
        _dev->driver->get(_dev, NETOPT_NID, &mac_phy_config.pan_id,
                          sizeof(mac_phy_config.pan_id));
        linkaddr_set_node_addr((linkaddr_t *)&uip_lladdr);
        _lowmac = p_ns->lmac;
        evproc_regCallback(EVENT_TYPE_PCK_LL, _emb6_netdev2_callback);
        return 1;
    }
    return 0;
}

static int8_t _netdev2_send(const void *pr_payload, uint8_t c_len)
{
    if (_dev != NULL) {
        const struct iovec vector = {
            .iov_base = (void *)pr_payload,
            .iov_len = c_len
        };
        if (_dev->driver->send(_dev, &vector, 1) < 0) {
            DEBUG("Error on send\n");
            return RADIO_TX_ERR;
        }
        DEBUG("Packet of length %u was transmitted\n", (unsigned)c_len);
        return RADIO_TX_OK;
    }
    DEBUG("Device was not initialized\n");
    return RADIO_TX_ERR;
}

static int8_t _netdev2_on(void)
{
    /* TODO: turn netdev2 on */
    return 1;
}

static int8_t _netdev2_off(void)
{
    /* TODO: turn netdev2 off */
    return 1;
}

static void _netdev2_set_txpower(int8_t power)
{
    int16_t pwr = power;

    _dev->driver->set(_dev, NETOPT_TX_POWER, &pwr, sizeof(pwr));
}

static int8_t _netdev2_get_txpower(void)
{
    int16_t power = 0;

    _dev->driver->get(_dev, NETOPT_TX_POWER, &power, sizeof(power));
    return (int8_t)power;
}

static void _netdev2_set_sensitivity(int8_t sens)
{
    /* TODO: set sensitivity */
}

static int8_t _netdev2_get_sensitivity(void)
{
    /* TODO: get sensitivity */
    return 0;
}

static int8_t _netdev2_get_rssi(void)
{
    return (int8_t)(_rssi_base_value + 1.03 * _last_rssi);
}

static void _netdev2_set_promisc(uint8_t c_on_off)
{
    netopt_enable_t en = (c_on_off) ? NETOPT_ENABLE : NETOPT_DISABLE;

    _dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
}

/** @} */
