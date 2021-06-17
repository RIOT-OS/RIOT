/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include "fmt.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/lorawan.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/lorawan.h"
#include "net/netdev.h"
#include "net/lora.h"
#include "net/loramac.h"
#include "net/gnrc/lorawan/region.h"
#include "net/gnrc/netreg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MSG_TYPE_MLME_BACKOFF_EXPIRE (0x3458)           /**< Backoff timer expiration message type */

static uint8_t _nwkskey[LORAMAC_NWKSKEY_LEN];
static uint8_t _appskey[LORAMAC_APPSKEY_LEN];
static uint8_t _appkey[LORAMAC_APPKEY_LEN];
static uint8_t _deveui[LORAMAC_DEVEUI_LEN];
static uint8_t _appeui[LORAMAC_APPEUI_LEN];
static uint8_t _devaddr[LORAMAC_DEVADDR_LEN];

static msg_t timeout_msg = {.type = MSG_TYPE_TIMEOUT};
static msg_t backoff_msg = {.type = MSG_TYPE_MLME_BACKOFF_EXPIRE};

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);
static void _msg_handler(gnrc_netif_t *netif, msg_t *msg);
static int _get(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt);
static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt);
static void _init(gnrc_netif_t *netif);

static const gnrc_netif_ops_t lorawan_ops = {
    .init = _init,
    .send = _send,
    .recv = _recv,
    .get = _get,
    .set = _set,
    .msg_handler = _msg_handler
};

void gnrc_lorawan_mlme_confirm(gnrc_lorawan_t *mac, mlme_confirm_t *confirm)
{
    gnrc_netif_lorawan_t *lw_netif =
        container_of(mac, gnrc_netif_lorawan_t, mac);

    if (confirm->type == MLME_JOIN) {
        if (confirm->status == 0) {
            DEBUG("gnrc_lorawan: join succeeded\n");
        }
        else {
            DEBUG("gnrc_lorawan: join failed\n");
        }
    }
    else if (confirm->type == MLME_LINK_CHECK) {
        lw_netif->flags &= ~GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK;
        lw_netif->demod_margin = confirm->link_req.margin;
        lw_netif->num_gateways = confirm->link_req.num_gateways;
    }
}

void gnrc_lorawan_set_timer(gnrc_lorawan_t *mac, uint32_t us)
{
    gnrc_netif_lorawan_t *lw_netif = container_of(mac, gnrc_netif_lorawan_t, mac);
    ztimer_set_msg(ZTIMER_MSEC, &lw_netif->timer, us/1000, &timeout_msg, thread_getpid());
}

void gnrc_lorawan_remove_timer(gnrc_lorawan_t *mac)
{
    gnrc_netif_lorawan_t *lw_netif = container_of(mac, gnrc_netif_lorawan_t, mac);
    ztimer_remove(ZTIMER_MSEC, &lw_netif->timer);
}

static inline void _set_be_addr(gnrc_lorawan_t *mac, uint8_t *be_addr)
{
    uint32_t tmp = byteorder_bebuftohl(be_addr);
    le_uint32_t dev_addr = byteorder_btoll(byteorder_htonl(tmp));

    mlme_request_t mlme_request;
    mlme_confirm_t mlme_confirm;

    mlme_request.type = MLME_SET;
    mlme_request.mib.type = MIB_DEV_ADDR;
    mlme_request.mib.dev_addr = &dev_addr;

    gnrc_lorawan_mlme_request(mac, &mlme_request, &mlme_confirm);
}

void gnrc_lorawan_mcps_indication(gnrc_lorawan_t *mac, mcps_indication_t *ind)
{
    (void)mac;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, ind->data.pkt->iol_base,
                                          ind->data.pkt->iol_len,
                                          GNRC_NETTYPE_LORAWAN);
    if (!pkt) {
        DEBUG("gnrc_lorawan: mcps_indication: couldn't allocate pktbuf\n");
        return;
    }

    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_LORAWAN, ind->data.port,
                                      pkt)) {
        gnrc_pktbuf_release(pkt);
    }
}

void gnrc_lorawan_mlme_indication(gnrc_lorawan_t *mac, mlme_indication_t *ind)
{
    (void)mac;
    (void)ind;
}

void gnrc_lorawan_mcps_confirm(gnrc_lorawan_t *mac, mcps_confirm_t *confirm)
{
    (void)mac;

    gnrc_pktbuf_release_error((gnrc_pktsnip_t *)confirm->msdu, confirm->status);

    DEBUG("gnrc_lorawan: transmission finished with status %i\n",
          confirm->status);
}

static void _rx_done(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    int bytes_expected = dev->driver->recv(dev, NULL, 0, 0);
    int nread;
    struct netdev_radio_rx_info rx_info;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected,
                                          GNRC_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("_recv_lorawan: cannot allocate pktsnip.\n");
        /* Discard packet on netdev device */
        dev->driver->recv(dev, NULL, bytes_expected, NULL);
        gnrc_lorawan_radio_rx_done_cb(mac, NULL, 0);
        return;
    }
    nread = dev->driver->recv(dev, pkt->data, bytes_expected, &rx_info);
    if (nread <= 0) {
        gnrc_pktbuf_release(pkt);
        gnrc_lorawan_radio_rx_done_cb(mac, NULL, 0);
        return;
    }

    gnrc_lorawan_radio_rx_done_cb(mac, pkt->data, pkt->size);
    gnrc_pktbuf_release(pkt);
}

static void _driver_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = dev->context;
    gnrc_lorawan_t *mac = &netif->lorawan.mac;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg = { .type = NETDEV_MSG_TYPE_EVENT,
                      .content = { .ptr = netif } };

        if (msg_send(&msg, netif->pid) <= 0) {
            DEBUG("gnrc_netif: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("gnrc_netif: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
                _rx_done(mac);
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                gnrc_lorawan_radio_tx_done_cb(mac);
                break;
            case NETDEV_EVENT_RX_TIMEOUT:
                gnrc_lorawan_radio_rx_timeout_cb(mac);
                break;
            default:
                DEBUG("gnrc_netif: warning: unhandled event %u.\n", event);
                break;
        }
    }
}

static void _reset(gnrc_netif_t *netif)
{
    netif->lorawan.otaa = CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE ==
                          LORAMAC_JOIN_OTAA ? NETOPT_ENABLE : NETOPT_DISABLE;
    netif->lorawan.datarate = CONFIG_LORAMAC_DEFAULT_DR;
    netif->lorawan.demod_margin = 0;
    netif->lorawan.num_gateways = 0;
    netif->lorawan.port = CONFIG_LORAMAC_DEFAULT_TX_PORT;
    netif->lorawan.ack_req = CONFIG_LORAMAC_DEFAULT_TX_MODE == LORAMAC_TX_CNF;
    netif->lorawan.flags = 0;
}

static void _memcpy_reversed(uint8_t *dst, uint8_t *src, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        dst[size - i - 1] = src[i];
    }
}

netdev_t *gnrc_lorawan_get_netdev(gnrc_lorawan_t *mac)
{
    gnrc_netif_t *netif = container_of(mac, gnrc_netif_t, lorawan.mac);

    return netif->dev;
}

static void _init(gnrc_netif_t *netif)
{
    gnrc_netif_default_init(netif);
    netif->dev->event_callback = _driver_cb;
    _reset(netif);

    /* Convert default keys, address and EUIs to hex */
    fmt_hex_bytes(_nwkskey, CONFIG_LORAMAC_NWK_SKEY_DEFAULT);
    fmt_hex_bytes(_appskey, CONFIG_LORAMAC_APP_SKEY_DEFAULT);
    fmt_hex_bytes(_appkey, CONFIG_LORAMAC_APP_KEY_DEFAULT);
    fmt_hex_bytes(_deveui, CONFIG_LORAMAC_DEV_EUI_DEFAULT);
    fmt_hex_bytes(_appeui, CONFIG_LORAMAC_APP_EUI_DEFAULT);
    fmt_hex_bytes(_devaddr, CONFIG_LORAMAC_DEV_ADDR_DEFAULT);

    /* Initialize default keys, address and EUIs */
    memcpy(netif->lorawan.nwkskey, _nwkskey, sizeof(_nwkskey));
    memcpy(netif->lorawan.appskey, _appskey, sizeof(_appskey));
    _memcpy_reversed(netif->lorawan.deveui, _deveui, sizeof(_deveui));
    memcpy(netif->lorawan.appkey, _appkey, sizeof(_appkey));
    _memcpy_reversed(netif->lorawan.appeui, _appeui, sizeof(_appeui));

    _set_be_addr(&netif->lorawan.mac, _devaddr);
    gnrc_lorawan_init(&netif->lorawan.mac, netif->lorawan.nwkskey, netif->lorawan.appskey);

    ztimer_set_msg(ZTIMER_MSEC, &netif->lorawan.backoff_timer,
                   GNRC_LORAWAN_BACKOFF_WINDOW_TICK / 1000,
                   &backoff_msg, thread_getpid());
}

int gnrc_netif_lorawan_create(gnrc_netif_t *netif, char *stack, int stacksize,
                              char priority, char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &lorawan_ops);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    (void)netif;
    /* Unused */
    return 0;
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *payload)
{
    mlme_request_t mlme_request;
    mlme_confirm_t mlme_confirm;

    if (netif->lorawan.flags & GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK) {
        mlme_request.type = MLME_LINK_CHECK;
        gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                  &mlme_confirm);
    }
    mcps_request_t req =
    { .type = netif->lorawan.ack_req ? MCPS_CONFIRMED : MCPS_UNCONFIRMED,
      .data =
      { .pkt = (iolist_t *)payload, .port = netif->lorawan.port,
          .dr = netif->lorawan.datarate } };
    mcps_confirm_t conf;

    gnrc_lorawan_mcps_request(&netif->lorawan.mac, &req, &conf);
    if (conf.status < 0) {
        gnrc_pktbuf_release_error(payload, conf.status);
    }
    return conf.status;
}

static void _msg_handler(gnrc_netif_t *netif, msg_t *msg)
{
    (void)netif;
    (void)msg;
    switch (msg->type) {
        case MSG_TYPE_TIMEOUT:
            gnrc_lorawan_timeout_cb(&netif->lorawan.mac);
            break;
        case MSG_TYPE_MLME_BACKOFF_EXPIRE:
            gnrc_lorawan_mlme_backoff_expire_cb(&netif->lorawan.mac);
            ztimer_set_msg(ZTIMER_MSEC, &netif->lorawan.backoff_timer,
                           GNRC_LORAWAN_BACKOFF_WINDOW_TICK / 1000,
                           &backoff_msg, thread_getpid());
        default:
            break;
    }
}

static int _get(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt)
{
    int res = 0;
    uint32_t tmp;

    mlme_confirm_t mlme_confirm;
    mlme_request_t mlme_request;

    switch (opt->opt) {
        case NETOPT_OTAA:
            assert(opt->data_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = netif->lorawan.otaa;
            break;
        case NETOPT_LINK:
            mlme_request.type = MLME_GET;
            mlme_request.mib.type = MIB_ACTIVATION_METHOD;
            gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                      &mlme_confirm);
            *((netopt_enable_t *)opt->data) = mlme_confirm.mib.activation !=
                                              MLME_ACTIVATION_NONE;
            break;
        case NETOPT_LINK_CHECK:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) =
                (netif->lorawan.flags & GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK) ?
                NETOPT_ENABLE : NETOPT_DISABLE;
            break;
        case NETOPT_NUM_GATEWAYS:
            assert(opt->data_len == sizeof(uint8_t));
            *((uint8_t *)opt->data) = netif->lorawan.num_gateways;
            break;
        case NETOPT_DEMOD_MARGIN:
            assert(opt->data_len == sizeof(uint8_t));
            *((uint8_t *)opt->data) = netif->lorawan.demod_margin;
            break;
        case NETOPT_ADDRESS:
            mlme_request.type = MLME_GET;
            mlme_request.mib.type = MIB_DEV_ADDR;

            gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                      &mlme_confirm);
            tmp = *((uint32_t *)mlme_confirm.mib.dev_addr);
            tmp = byteorder_swapl(tmp);
            memcpy(opt->data, &tmp, sizeof(uint32_t));
            res = sizeof(uint32_t);
            break;
        default:
            res = netif->dev->driver->get(netif->dev, opt->opt, opt->data,
                                          opt->data_len);
            break;
    }
    return res;
}

static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt)
{
    int res = 0;
    mlme_confirm_t mlme_confirm;
    mlme_request_t mlme_request;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_LORAWAN_DR:
            assert(opt->data_len == sizeof(uint8_t));
            if (!gnrc_lorawan_validate_dr(*((uint8_t *)opt->data))) {
                DEBUG("gnrc_netif_lorawan: Invalid datarate\n");
                res = -EINVAL;
                break;
            }
            netif->lorawan.datarate = *((uint8_t *)opt->data);
            break;
        case NETOPT_LORAWAN_TX_PORT:
            assert(opt->data_len == sizeof(uint8_t));
            netif->lorawan.port = *((uint8_t *)opt->data);
            break;
        case NETOPT_ACK_REQ:
            assert(opt->data_len == sizeof(netopt_enable_t));
            netif->lorawan.ack_req = *((netopt_enable_t *)opt->data);
            break;
        case NETOPT_LORAWAN_APPKEY:
            assert(opt->data_len == LORAMAC_APPKEY_LEN);
            memcpy(netif->lorawan.appkey, opt->data, LORAMAC_APPKEY_LEN);
            break;
        case NETOPT_ADDRESS_LONG:
            assert(opt->data_len == LORAMAC_DEVEUI_LEN);
            _memcpy_reversed(netif->lorawan.deveui, opt->data,
                             LORAMAC_DEVEUI_LEN);
            break;
        case NETOPT_LORAWAN_APPEUI:
            assert(opt->data_len == LORAMAC_APPEUI_LEN);
            _memcpy_reversed(netif->lorawan.appeui, opt->data,
                             LORAMAC_APPEUI_LEN);
            break;
        case NETOPT_OTAA:
            assert(opt->data_len == sizeof(netopt_enable_t));
            netif->lorawan.otaa = *((netopt_enable_t *)opt->data);
            break;
        case NETOPT_LORAWAN_APPSKEY:
            assert(opt->data_len >= LORAMAC_APPSKEY_LEN);
            memcpy(netif->lorawan.appskey, opt->data, LORAMAC_APPSKEY_LEN);
            break;
        case NETOPT_LORAWAN_NWKSKEY:
            assert(opt->data_len >= LORAMAC_NWKSKEY_LEN);
            memcpy(netif->lorawan.nwkskey, opt->data, LORAMAC_NWKSKEY_LEN);
            break;
        case NETOPT_LINK:
        {
            netopt_enable_t en = *((netopt_enable_t *)opt->data);
            if (en) {
                if (netif->lorawan.otaa) {
                    mlme_request.type = MLME_JOIN;
                    mlme_request.join.deveui = netif->lorawan.deveui;
                    mlme_request.join.appeui = netif->lorawan.appeui;
                    mlme_request.join.appkey = netif->lorawan.appkey;
                    mlme_request.join.dr = netif->lorawan.datarate;
                    gnrc_lorawan_mlme_request(&netif->lorawan.mac,
                                              &mlme_request, &mlme_confirm);
                }
                else {
                    mlme_request.type = MLME_SET;
                    mlme_request.mib.type = MIB_ACTIVATION_METHOD;
                    mlme_request.mib.activation = MLME_ACTIVATION_ABP;
                    gnrc_lorawan_mlme_request(&netif->lorawan.mac,
                                              &mlme_request, &mlme_confirm);
                }
            }
            else {
                mlme_request.type = MLME_RESET;
                gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                          &mlme_confirm);
                res = mlme_confirm.status;
                if (mlme_confirm.status == 0) {
                    /* reset netif as well */
                    _reset(netif);
                }
            }
            break;
        }
        case NETOPT_ADDRESS:
            assert(opt->data_len == sizeof(uint32_t));
            _set_be_addr(&netif->lorawan.mac, opt->data);
            break;
        case NETOPT_LINK_CHECK:
            netif->lorawan.flags |= GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK;
            break;
        case NETOPT_LORAWAN_RX2_DR:
            assert(opt->data_len == sizeof(uint8_t));
            mlme_request.type = MLME_SET;
            mlme_request.mib.type = MIB_RX2_DR;
            mlme_request.mib.rx2_dr = *((uint8_t *)opt->data);
            gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                      &mlme_confirm);
            break;
        default:
            res = netif->dev->driver->set(netif->dev, opt->opt, opt->data,
                                          opt->data_len);
            break;
    }
    gnrc_netif_release(netif);
    return res;
}
/** @} */
