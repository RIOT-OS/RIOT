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

#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netif/lorawan.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/lorawan.h"
#include "net/netdev.h"
#include "net/lora.h"
#include "net/loramac.h"
#include "net/gnrc/lorawan/region.h"
#include "net/gnrc/netreg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define MSG_TYPE_MLME_BACKOFF_EXPIRE (0x3458)           /**< Backoff timer expiration message type */

static uint8_t _appskey[LORAMAC_APPSKEY_LEN];
static uint8_t _appkey[LORAMAC_APPKEY_LEN];
static uint8_t _snwksintkey[LORAMAC_SNWKSINTKEY_LEN];
static uint8_t _nwksenckey[LORAMAC_NWKSENCKEY_LEN];
static uint8_t _nwkkey[LORAMAC_NWKKEY_LEN];
static uint8_t _joineui[LORAMAC_JOINEUI_LEN];
static uint8_t _fnwksintkey[LORAMAC_FNWKSINTKEY_LEN];
static uint8_t _deveui[LORAMAC_DEVEUI_LEN];
static uint8_t _devaddr[LORAMAC_DEVADDR_LEN];

static msg_t timeout_msg = { .type = MSG_TYPE_TIMEOUT };
static msg_t backoff_msg = { .type = MSG_TYPE_MLME_BACKOFF_EXPIRE };

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);
static void _msg_handler(gnrc_netif_t *netif, msg_t *msg);
static int _get(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt);
static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt);
static int _init(gnrc_netif_t *netif);

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
            gnrc_netif_lorawan_t *lw_netif = container_of(mac, gnrc_netif_lorawan_t, mac);
            gnrc_netif_t *netif = container_of(lw_netif, gnrc_netif_t, lorawan);

            DEBUG("gnrc_lorawan: join succeeded %d\n", netif->pid);
            if (netif->dev) {
                netif->flags |= GNRC_NETIF_FLAGS_HAS_L2ADDR;
                netif->dev->event_callback(netif->dev, NETDEV_EVENT_LINK_UP);
            }
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

    ztimer_set_msg(ZTIMER_MSEC, &lw_netif->timer, us / 1000, &timeout_msg, thread_getpid());
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
    gnrc_netif_t *netif = container_of(mac, gnrc_netif_t, lorawan.mac);
    gnrc_nettype_t nettype = GNRC_NETTYPE_UNDEF;
    uint32_t demux = GNRC_NETREG_DEMUX_CTX_ALL;

    assert(ind->data.port >= LORAMAC_PORT_MIN && ind->data.port <= LORAMAC_PORT_MAX);

    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, ind->data.pkt->iol_base,
                                          ind->data.pkt->iol_len,
                                          nettype);

    if (!pkt) {
        DEBUG("gnrc_lorawan: mcps_indication: couldn't allocate pktbuf\n");
        return;
    }

    gnrc_pktsnip_t *netif_snip = gnrc_netif_hdr_build(NULL, 0,
                                                      &ind->data.port,
                                                      sizeof(ind->data.port));
    if (netif_snip == NULL) {
        DEBUG("gnrc_lorawan_netif: no space left in packet buffer\n");
        goto release;
    }

    gnrc_netif_hdr_t *hdr = netif_snip->data;
    gnrc_netif_hdr_set_netif(hdr, netif);
    pkt = gnrc_pkt_append(pkt, netif_snip);

    if (!gnrc_netapi_dispatch_receive(nettype, demux, pkt)) {
        DEBUG("gnrc_lorawan_netif: unable to forward packet\n");
        goto release;
    }

    return;

release:
    gnrc_pktbuf_release(pkt);
}

void gnrc_lorawan_mlme_indication(gnrc_lorawan_t *mac, mlme_indication_t *ind)
{
    (void)mac;
    (void)ind;
}

__attribute__((weak))
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
        gnrc_lorawan_radio_rx_error_cb(mac);
        return;
    }
    nread = dev->driver->recv(dev, pkt->data, bytes_expected, &rx_info);
    if (nread <= 0) {
        gnrc_pktbuf_release(pkt);
        gnrc_lorawan_radio_rx_error_cb(mac);
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
        event_post(&netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW], &netif->event_isr);
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
        case NETDEV_EVENT_LINK_UP: {
            if (IS_USED(MODULE_GNRC_IPV6)) {
                msg_t msg = { .type = GNRC_IPV6_NIB_IFACE_UP, .content = { .ptr = netif } };

                msg_send(&msg, gnrc_ipv6_pid);
            }
            break;
        }
        case NETDEV_EVENT_LINK_DOWN: {
            if (IS_USED(MODULE_GNRC_IPV6)) {
                msg_t msg = { .type = GNRC_IPV6_NIB_IFACE_DOWN, .content = { .ptr = netif } };

                msg_send(&msg, gnrc_ipv6_pid);
            }
            break;
        }
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

static int _init(gnrc_netif_t *netif)
{
    DEBUG("netif init ! \n");
    int res = gnrc_netif_default_init(netif);

    if (res < 0) {
        return res;
    }

    netif->dev->event_callback = _driver_cb;
    _reset(netif);

    /* Convert default keys, address and EUIs to hex */
    fmt_hex_bytes(_appskey, CONFIG_LORAMAC_APP_SKEY_DEFAULT);
    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        fmt_hex_bytes(_joineui, CONFIG_LORAMAC_JOIN_EUI_DEFAULT);
        fmt_hex_bytes(_appkey, CONFIG_LORAMAC_APP_KEY_DEFAULT);
        fmt_hex_bytes(_nwkkey, CONFIG_LORAMAC_NWK_KEY_DEFAULT);
        fmt_hex_bytes(_fnwksintkey, CONFIG_LORAMAC_FNWKSINT_KEY_DEFAULT);
        fmt_hex_bytes(_snwksintkey, CONFIG_LORAMAC_SNWKSINT_KEY_DEFAULT);
        fmt_hex_bytes(_nwksenckey, CONFIG_LORAMAC_NWKSENC_KEY_DEFAULT);
    }
    else {
        fmt_hex_bytes(_joineui, CONFIG_LORAMAC_APP_EUI_DEFAULT);
        fmt_hex_bytes(_nwkkey, CONFIG_LORAMAC_APP_KEY_DEFAULT);
        fmt_hex_bytes(_fnwksintkey, CONFIG_LORAMAC_NWK_SKEY_DEFAULT);
    }

    fmt_hex_bytes(_deveui, CONFIG_LORAMAC_DEV_EUI_DEFAULT);
    fmt_hex_bytes(_devaddr, CONFIG_LORAMAC_DEV_ADDR_DEFAULT);

    /* Initialize default keys, address and EUIs */
    memcpy(netif->lorawan.appskey, _appskey, sizeof(_appskey));
    _memcpy_reversed(netif->lorawan.deveui, _deveui, sizeof(_deveui));
    _memcpy_reversed(netif->lorawan.joineui, _joineui, sizeof(_joineui));
    memcpy(netif->lorawan.nwkkey, _nwkkey, sizeof(_nwkkey));
    memcpy(netif->lorawan.fnwksintkey, _fnwksintkey, sizeof(_fnwksintkey));

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        gnrc_netif_lorawan_set_appkey(&netif->lorawan, _appkey, sizeof(_appkey));
        gnrc_netif_lorawan_set_snwksintkey(&netif->lorawan, _snwksintkey, sizeof(_snwksintkey));
        gnrc_netif_lorawan_set_nwksenckey(&netif->lorawan, _nwksenckey, sizeof(_nwksenckey));
    }

    _set_be_addr(&netif->lorawan.mac, _devaddr);

    const gnrc_lorawan_key_ctx_t ctx = {
        .appskey = netif->lorawan.appskey,
        .fnwksintkey = netif->lorawan.fnwksintkey,
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
        .snwksintkey = netif->lorawan.snwksintkey,
        .nwksenckey = netif->lorawan.nwksenckey,
        .jsintkey = netif->lorawan.jsintkey,
        .jsenckey = netif->lorawan.jsenckey
#else
        .snwksintkey = netif->lorawan.fnwksintkey,
        .nwksenckey = netif->lorawan.fnwksintkey
#endif
    };

    gnrc_lorawan_init(&netif->lorawan.mac, netif->lorawan.joineui, &ctx);

    ztimer_set_msg(ZTIMER_MSEC, &netif->lorawan.backoff_timer,
                   GNRC_LORAWAN_BACKOFF_WINDOW_TICK / 1000,
                   &backoff_msg, thread_getpid());

    return res;
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
    gnrc_netif_hdr_t *netif_hdr;
    const uint8_t *dst;
    mlme_request_t mlme_request;
    mlme_confirm_t mlme_confirm;

    uint8_t port;
    int res = -EINVAL;

    assert(payload);

    netif_hdr = payload->data;
    dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);

    assert(payload->type == GNRC_NETTYPE_NETIF);
    port = dst[0];

    if (netif_hdr->dst_l2addr_len != sizeof(port)) {
        goto end;
    }

    /* Remove the netif hdr snip and point to the MSDU */
    payload = gnrc_pktbuf_remove_snip(payload, payload);

    if (netif->lorawan.flags & GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK) {
        mlme_request.type = MLME_LINK_CHECK;
        gnrc_lorawan_mlme_request(&netif->lorawan.mac, &mlme_request,
                                  &mlme_confirm);
    }

    mcps_request_t req =
    { .type = netif->lorawan.ack_req ? MCPS_CONFIRMED : MCPS_UNCONFIRMED,
      .data =
      { .pkt = (iolist_t *)payload, .port = port,
        .dr = netif->lorawan.datarate } };
    mcps_confirm_t conf;

    gnrc_lorawan_mcps_request(&netif->lorawan.mac, &req, &conf);
    res = conf.status;

    if (res < 0) {
        DEBUG("gnrc_netif: unable to send (%s)\n", strerror(-res));
        gnrc_pktbuf_release_error(payload, res);
    }

end:
    return res;
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
    case NETOPT_ACK_REQ:
        assert(opt->data_len == sizeof(netopt_enable_t));
        netif->lorawan.ack_req = *((netopt_enable_t *)opt->data);
        break;
    case NETOPT_ADDRESS_LONG:
        assert(opt->data_len == LORAMAC_DEVEUI_LEN);
        _memcpy_reversed(netif->lorawan.deveui, opt->data,
                         LORAMAC_DEVEUI_LEN);
        break;
    case NETOPT_LORAWAN_APPEUI:
    case NETOPT_LORAWAN_JOINEUI:
        assert(opt->data_len == LORAMAC_JOINEUI_LEN);
        _memcpy_reversed(netif->lorawan.joineui, opt->data,
                         LORAMAC_JOINEUI_LEN);
        break;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    case NETOPT_LORAWAN_NWKKEY:
        assert(opt->data_len == LORAMAC_NWKKEY_LEN);
        memcpy(netif->lorawan.nwkkey, opt->data, LORAMAC_NWKKEY_LEN);
        break;
    case NETOPT_LORAWAN_APPKEY:
        assert(opt->data_len == LORAMAC_APPKEY_LEN);
        memcpy(netif->lorawan.appkey, opt->data, LORAMAC_APPKEY_LEN);
        break;
    case NETOPT_LORAWAN_FNWKSINTKEY:
        assert(opt->data_len == LORAMAC_FNWKSINTKEY_LEN);
        _memcpy_reversed(netif->lorawan.fnwksintkey, opt->data,
                         LORAMAC_FNWKSINTKEY_LEN);
        break;
    case NETOPT_LORAWAN_SNWKSINTKEY:
        assert(opt->data_len == LORAMAC_SNWKSINTKEY_LEN);
        _memcpy_reversed(netif->lorawan.snwksintkey, opt->data,
                         LORAMAC_SNWKSINTKEY_LEN);
        break;
    case NETOPT_LORAWAN_NWKSENCKEY:
        assert(opt->data_len == LORAMAC_NWKSENCKEY_LEN);
        _memcpy_reversed(netif->lorawan.nwksenckey, opt->data,
                         LORAMAC_NWKSKEY_LEN);
        break;
#else
    case NETOPT_LORAWAN_APPKEY:
        assert(opt->data_len == LORAMAC_NWKKEY_LEN);
        memcpy(netif->lorawan.nwkkey, opt->data, LORAMAC_NWKKEY_LEN);
        break;
    case NETOPT_LORAWAN_NWKSKEY:
    case NETOPT_LORAWAN_FNWKSINTKEY:
    case NETOPT_LORAWAN_SNWKSINTKEY:
    case NETOPT_LORAWAN_NWKSENCKEY:
        assert(opt->data_len == LORAMAC_FNWKSINTKEY_LEN);
        memcpy(netif->lorawan.fnwksintkey, opt->data,
                         LORAMAC_FNWKSINTKEY_LEN);
        break;
#endif
    case NETOPT_OTAA:
        assert(opt->data_len == sizeof(netopt_enable_t));
        netif->lorawan.otaa = *((netopt_enable_t *)opt->data);
        break;
    case NETOPT_LORAWAN_APPSKEY:
        assert(opt->data_len >= LORAMAC_APPSKEY_LEN);
        memcpy(netif->lorawan.appskey, opt->data, LORAMAC_APPSKEY_LEN);
        break;
    case NETOPT_LINK:
    {
        netopt_enable_t en = *((netopt_enable_t *)opt->data);
        if (en) {
            if (netif->lorawan.otaa) {
                mlme_request.type = MLME_JOIN;
                mlme_request.join.deveui = netif->lorawan.deveui;
                mlme_request.join.joineui = netif->lorawan.joineui;
                mlme_request.join.nwkkey = netif->lorawan.nwkkey;

                if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
                    gnrc_lorawan_mlme_join_set_appkey(&mlme_request.join, gnrc_netif_lorawan_get_appkey(
                                                          &netif->lorawan));
                }

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
                netif->flags &= ~GNRC_NETIF_FLAGS_HAS_L2ADDR;
                netif->dev->event_callback(netif->dev, NETDEV_EVENT_LINK_DOWN);
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
