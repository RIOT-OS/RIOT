/*
 * Copyright (C) 2018-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_netif
 * @{
 *
 * @file
 * @brief       GNRC netif wrapper for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <limits.h>
#include <errno.h>

#include "assert.h"
#include "thread.h"
#include "thread_flags.h"

#include "net/ble.h"
#include "net/bluetil/addr.h"
#include "net/gnrc/netapi/notify.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"

#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "nimble_riot.h"
#include "host/ble_gap.h"
#include "host/util/util.h"
#include "mem/mem.h"
#include <string.h>

#define ENABLE_DEBUG            0
#include "debug.h"

#ifdef MODULE_GNRC_SIXLOWPAN
#define NETTYPE                 GNRC_NETTYPE_SIXLOWPAN
#elif defined(MODULE_GNRC_IPV6)
#define NETTYPE                 GNRC_NETTYPE_IPV6
#else
#define NETTYPE                 GNRC_NETTYPE_UNDEF
#endif

#ifndef NIMBLE_NETIF_PRIO
#define NIMBLE_NETIF_PRIO       GNRC_NETIF_PRIO
#endif

#ifndef CONFIG_NIMBLE_NETIF_ADV_INSTANCE
#define CONFIG_NIMBLE_NETIF_ADV_INSTANCE            0
#endif

/* thread flag used for signaling transmit readiness */
#define FLAG_TX_UNSTALLED       (1u << 13)
#define FLAG_TX_NOTCONN         (1u << 12)
#define FLAG_ALL                (FLAG_TX_UNSTALLED | FLAG_TX_NOTCONN)

/* allocate a stack for the netif device */
static char _stack[THREAD_STACKSIZE_DEFAULT];
static thread_t *_netif_thread;

/* keep the actual device state */
static gnrc_netif_t _netif;
static gnrc_nettype_t _nettype = NETTYPE;

/* keep a reference to the event callback */
static nimble_netif_eventcb_t _eventcb;

/* notify the user about state changes for a connection context */
static void _notify(int handle, nimble_netif_event_t event, uint8_t *addr)
{
    if (_eventcb) {
        _eventcb(handle, event, addr);
    }
}

static int _netif_init(gnrc_netif_t *netif)
{
    (void)netif;

    int res = gnrc_netif_default_init(netif);
    if (res < 0) {
        return res;
    }

    /* save the threads context pointer, so we can set its flags */
    _netif_thread = thread_get_active();

#if IS_USED(MODULE_GNRC_NETIF_6LO)
    /* we disable fragmentation for this device, as the L2CAP layer takes care
     * of this */
    _netif.sixlo.max_frag_size = 0;
#endif  /* IS_USED(MODULE_GNRC_NETIF_6LO) */
    res = 0;

    return res;
}

static int _send_pkt(nimble_netif_conn_t *conn, gnrc_pktsnip_t *pkt)
{
    int res;
    int num_bytes = 0;

    if (conn == NULL || conn->coc == NULL) {
        return -ENOTCONN;
    }

    /* copy the data into a newly allocated mbuf */
    struct os_mbuf *sdu = os_msys_get_pkthdr(gnrc_pkt_len(pkt), 0);
    if (sdu == NULL) {
        return -ENOBUFS;
    }
    while (pkt) {
        res = os_mbuf_append(sdu, pkt->data, pkt->size);
        if (res != 0) {
            os_mbuf_free_chain(sdu);
            return -ENOBUFS;
        }
        num_bytes += (int)pkt->size;
        pkt = pkt->next;
    }

    /* send packet via the given L2CAP COC */
    do {
        res = ble_l2cap_send(conn->coc, sdu);
        if (res == BLE_HS_EBUSY) {
            thread_flags_t state = thread_flags_wait_any(FLAG_ALL);
            /* abort if the active connection was lost in the mean time */
            if ((state & FLAG_TX_NOTCONN) && !nimble_netif_conn_is_open(conn)) {
                return -ECONNRESET;
            }
        }
    } while (res == BLE_HS_EBUSY);

    if ((res != 0) && (res != BLE_HS_ESTALLED)) {
        os_mbuf_free_chain(sdu);
        return -ECANCELED;
    }

    return num_bytes;
}

static int _netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(pkt->type == GNRC_NETTYPE_NETIF);

    (void)netif;
    int res;

    gnrc_netif_hdr_t *hdr = (gnrc_netif_hdr_t *)pkt->data;
    /* if packet is bcast or mcast, we send it to every connected node */
    if (hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        int handle = nimble_netif_conn_get_next(NIMBLE_NETIF_CONN_INVALID,
                                                NIMBLE_NETIF_L2CAP_CONNECTED);
        while (handle != NIMBLE_NETIF_CONN_INVALID) {
            res = _send_pkt(nimble_netif_conn_get(handle), pkt->next);
            handle = nimble_netif_conn_get_next(handle, NIMBLE_NETIF_L2CAP_CONNECTED);
        }
    }
    /* send unicast */
    else {
        int handle = nimble_netif_conn_get_by_addr(
            gnrc_netif_hdr_get_dst_addr(hdr));
        nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
        res = _send_pkt(conn, pkt->next);
    }

    /* release the packet in GNRC's packet buffer */
    gnrc_pktbuf_release(pkt);
    return res;
}

/* not used, we pass incoming data to GNRC directly from the NimBLE thread */
static gnrc_pktsnip_t *_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    return NULL;
}

static const gnrc_netif_ops_t _nimble_netif_ops = {
    .init = _netif_init,
    .send = _netif_send,
    .recv = _netif_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = NULL,
};

static inline int _netdev_init(netdev_t *dev)
{
    (void)dev;

    /* get our own address from the controller */
    uint8_t tmp[6];
    int res = ble_hs_id_copy_addr(nimble_riot_own_addr_type, tmp, NULL);
    assert(res == 0);
    (void)res;

    bluetil_addr_swapped_cp(tmp, _netif.l2addr);

    /* signal link UP */
    dev->event_callback(dev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static inline int _netdev_get(netdev_t *dev, netopt_t opt,
                              void *value, size_t max_len)
{
    (void)dev;
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= BLE_ADDR_LEN);
            memcpy(value, _netif.l2addr, BLE_ADDR_LEN);
            res = BLE_ADDR_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = BLE_ADDR_LEN;
            res = sizeof(uint16_t);
            break;
        case NETOPT_MAX_PDU_SIZE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)value) = NIMBLE_NETIF_MTU;
            res = sizeof(uint16_t);
            break;
        case NETOPT_PROTO:
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = _nettype;
            res = sizeof(gnrc_nettype_t);
            break;
        case NETOPT_DEVICE_TYPE:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_BLE;
            res = sizeof(uint16_t);
            break;
        default:
            break;
    }

    return res;
}

static inline int _netdev_set(netdev_t *dev, netopt_t opt,
                              const void *value, size_t val_len)
{
    (void)dev;
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_PROTO:
            assert(val_len == sizeof(_nettype));
            memcpy(&_nettype, value, sizeof(_nettype));
            res = sizeof(_nettype);
            break;
        default:
            break;
    }

    return res;
}

static const netdev_driver_t _nimble_netdev_driver = {
    .send = NULL,
    .recv = NULL,
    .init = _netdev_init,
    .isr  =  NULL,
    .get  = _netdev_get,
    .set  = _netdev_set,
};

static netdev_t _nimble_netdev_dummy = {
    .driver = &_nimble_netdev_driver,
};

static void _on_data(nimble_netif_conn_t *conn, struct ble_l2cap_event *event)
{
    struct os_mbuf *rxb = event->receive.sdu_rx;
    size_t rx_len = (size_t)OS_MBUF_PKTLEN(rxb);

    /* allocate netif header */
    gnrc_pktsnip_t *if_snip = gnrc_netif_hdr_build(conn->addr, BLE_ADDR_LEN,
                                                   _netif.l2addr,
                                                   BLE_ADDR_LEN);
    if (if_snip == NULL) {
        goto end;
    }

    /* we need to add the device PID to the netif header */
    gnrc_netif_hdr_set_netif(if_snip->data, &_netif);

    /* allocate space in the pktbuf to store the packet */
    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(if_snip, NULL, rx_len, _nettype);
    if (payload == NULL) {
        gnrc_pktbuf_release(if_snip);
        goto end;
    }

    /* copy payload from mbuf into pktbuffer */
    int res = os_mbuf_copydata(rxb, 0, rx_len, payload->data);
    if (res != 0) {
        gnrc_pktbuf_release(payload);
        goto end;
    }

    /* finally dispatch the receive packet to GNRC */
    if (!gnrc_netapi_dispatch_receive(payload->type, GNRC_NETREG_DEMUX_CTX_ALL,
                                      payload)) {
        gnrc_pktbuf_release(payload);
    }

end:
    /* free the mbuf and allocate a new one for receiving new data */
    os_mbuf_free_chain(rxb);
    rxb = os_msys_get_pkthdr(MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
    /* due to buffer provisioning, there should always be enough space */
    assert(rxb != NULL);
    ble_l2cap_recv_ready(event->receive.chan, rxb);
}

/**
 * @brief   Sends a netapi notification for a connection event.
 *
 * @param[in] notify    The type of notification event.
 * @param[in] addr      BLE address of the node that (dis-)connected.
 */
static inline void _dispatch_connection_event(netapi_notify_t notify, const void *addr)
{
    if (!IS_USED(MODULE_GNRC_NETAPI_NOTIFY)) {
        return;
    }

    netapi_notify_l2_connection_t event = {
        .l2addr_len = BLE_ADDR_LEN,
        .if_pid = _netif.pid,
    };

    memcpy(event.l2addr, addr, BLE_ADDR_LEN);

    gnrc_netapi_notify(GNRC_NETTYPE_L2_DISCOVERY, GNRC_NETREG_DEMUX_CTX_ALL,
                       notify, &event, sizeof(netapi_notify_l2_connection_t));
}

static int _on_l2cap_client_evt(struct ble_l2cap_event *event, void *arg)
{
    int handle = (int)arg;
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    assert(conn && (conn->state & NIMBLE_NETIF_GAP_MASTER));

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED:
            if (event->connect.status != 0) {
                /* in the unlikely event the L2CAP connection establishment
                 * fails, we close the GAP connection */
                ble_gap_terminate(conn->gaphandle, BLE_ERR_REM_USER_CONN_TERM);
                break;
            }
            conn->coc = event->connect.chan;
            conn->state |= NIMBLE_NETIF_L2CAP_CLIENT;
            conn->state &= ~NIMBLE_NETIF_CONNECTING;
            _notify(handle, NIMBLE_NETIF_CONNECTED_MASTER, conn->addr);
            _dispatch_connection_event(NETAPI_NOTIFY_L2_NEIGH_CONNECTED, conn->addr);
            break;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            assert(conn->state & NIMBLE_NETIF_L2CAP_CLIENT);
            conn->state &= ~NIMBLE_NETIF_L2CAP_CONNECTED;
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT:
            /* this event should never be triggered for the L2CAP client */
            assert(0);
            break;
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            _on_data(conn, event);
            break;
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
            thread_flags_set(_netif_thread, FLAG_TX_UNSTALLED);
            break;
        default:
            break;
    }

    return 0;
}

static int _on_l2cap_server_evt(struct ble_l2cap_event *event, void *arg)
{
    (void)arg;
    int handle;
    nimble_netif_conn_t *conn;

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED:
            handle = nimble_netif_conn_get_by_gaphandle(event->connect.conn_handle);
            conn = nimble_netif_conn_get(handle);
            assert(conn);

            /* in the unlikely event the L2CAP connection establishment fails,
             * we close the GAP connection */
            if (event->connect.status != 0) {
                ble_gap_terminate(conn->gaphandle, BLE_ERR_REM_USER_CONN_TERM);
                break;
            }

            /* we need to update the state to keep everything in sync */
            conn->coc = event->connect.chan;
            conn->state |= NIMBLE_NETIF_L2CAP_SERVER;
            conn->state &= ~(NIMBLE_NETIF_ADV | NIMBLE_NETIF_CONNECTING);

            /* in case conn itvl spacing is enabled, make sure that the conn
             * itvl of the new connection is sufficiently spaced */
            if ((NIMBLE_NETIF_CONN_ITVL_SPACING > 0) &&
                nimble_netif_conn_itvl_used(conn->itvl, handle)) {
                ble_gap_terminate(conn->gaphandle, BLE_ERR_REM_USER_CONN_TERM);
                break;
            }

            _notify(handle, NIMBLE_NETIF_CONNECTED_SLAVE, conn->addr);
            _dispatch_connection_event(NETAPI_NOTIFY_L2_NEIGH_CONNECTED, conn->addr);
            break;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            conn = nimble_netif_conn_from_gaphandle(event->disconnect.conn_handle);
            assert(conn && (conn->state & NIMBLE_NETIF_L2CAP_SERVER));
            conn->coc = NULL;
            conn->state &= ~NIMBLE_NETIF_L2CAP_CONNECTED;
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT: {
            struct os_mbuf *sdu_rx = os_msys_get_pkthdr(
                                            MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
            /* there should always be enough buffer space */
            assert(sdu_rx != NULL);
            ble_l2cap_recv_ready(event->accept.chan, sdu_rx);
            break;
        }
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            conn = nimble_netif_conn_from_gaphandle(event->receive.conn_handle);
            assert(conn);
            _on_data(conn, event);
            break;
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
            thread_flags_set(_netif_thread, FLAG_TX_UNSTALLED);
            break;
        default:
            break;
    }

    return 0;
}

static void _on_gap_connected(nimble_netif_conn_t *conn, uint16_t conn_handle)
{
    struct ble_gap_conn_desc desc;
    int res = ble_gap_conn_find(conn_handle, &desc);
    assert(res == 0);
    (void)res;

    conn->gaphandle = conn_handle;
    conn->itvl = desc.conn_itvl;
    bluetil_addr_swapped_cp(desc.peer_id_addr.val, conn->addr);
}

static void _on_gap_param_update(int handle, nimble_netif_conn_t *conn)
{
    struct ble_gap_conn_desc desc;
    int res = ble_gap_conn_find(conn->gaphandle, &desc);
    assert(res == 0) ;
    (void)res;

    conn->itvl = desc.conn_itvl;
    _notify(handle, NIMBLE_NETIF_CONN_UPDATED, conn->addr);
}

static int _on_gap_master_evt(struct ble_gap_event *event, void *arg)
{
    int res = 0;
    int handle = (int)arg;
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    assert(conn);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            if (event->connect.status != 0) {
                uint8_t addr[BLE_ADDR_LEN];
                nimble_netif_conn_free(handle, addr);
                _notify(handle, NIMBLE_NETIF_ABORT_MASTER, addr);
                return 0;
            }
            _on_gap_connected(conn, event->connect.conn_handle);
            conn->state |= NIMBLE_NETIF_GAP_MASTER;

            struct os_mbuf *sdu_rx = os_msys_get_pkthdr(
                                            MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
            /* in the rare case that we run out of buffers, we close the new
             * connection right away */
            if (sdu_rx == NULL) {
                ble_gap_terminate(handle, BLE_ERR_REM_USER_CONN_TERM);
                break;
            }
            res = ble_l2cap_connect(event->connect.conn_handle,
                                    NIMBLE_NETIF_CID, NIMBLE_NETIF_MTU, sdu_rx,
                                    _on_l2cap_client_evt, (void *)handle);
            /* should always be successful */
            assert(res == 0);
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT: {
            nimble_netif_event_t type;
            type = (conn->coc != NULL) ? NIMBLE_NETIF_CLOSED_MASTER
                                       : NIMBLE_NETIF_ABORT_MASTER;
            uint8_t addr[BLE_ADDR_LEN];
            nimble_netif_conn_free(handle, addr);
            thread_flags_set(_netif_thread, FLAG_TX_NOTCONN);
            _notify(handle, type, addr);
            _dispatch_connection_event(NETAPI_NOTIFY_L2_NEIGH_DISCONNECTED, addr);
            break;
        }
        case BLE_GAP_EVENT_CONN_UPDATE:
            _on_gap_param_update(handle, conn);
            break;
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        case BLE_GAP_EVENT_MTU:
            /* nothing to do here */
            break;
        default:
            break;
    }

    return res;
}

static int _on_gap_slave_evt(struct ble_gap_event *event, void *arg)
{
    int handle = (int)arg;
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    assert(conn);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            if (event->connect.status != 0) {
                uint8_t addr[BLE_ADDR_LEN];
                nimble_netif_conn_free(handle, addr);
                _notify(handle, NIMBLE_NETIF_ABORT_SLAVE, addr);
                break;
            }
            _on_gap_connected(conn, event->connect.conn_handle);
            assert(conn->state == NIMBLE_NETIF_ADV);
            conn->state = NIMBLE_NETIF_GAP_SLAVE;
            _notify(handle, NIMBLE_NETIF_INIT_SLAVE, conn->addr);
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT: {
            nimble_netif_event_t type;
            type = (conn->coc != NULL) ? NIMBLE_NETIF_CLOSED_SLAVE
                                       : NIMBLE_NETIF_ABORT_SLAVE;
            uint8_t addr[BLE_ADDR_LEN];
            nimble_netif_conn_free(handle, addr);
            thread_flags_set(_netif_thread, FLAG_TX_NOTCONN);
            _notify(handle, type, addr);
            _dispatch_connection_event(NETAPI_NOTIFY_L2_NEIGH_DISCONNECTED, addr);
            break;
        }
        case BLE_GAP_EVENT_CONN_UPDATE:
            _on_gap_param_update(handle, conn);
            break;
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
            /* nothing to do here */
            break;
        case BLE_GAP_EVENT_ADV_COMPLETE: {
            if (conn->state == NIMBLE_NETIF_ADV) {
                uint8_t addr[BLE_ADDR_LEN];
                nimble_netif_conn_free(handle, addr);
                _notify(handle, NIMBLE_NETIF_ACCEPT_STOP, addr);
            }
        }
        case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
            break;
        default:
            break;
    }

    return 0;
}

void nimble_netif_init(void)
{
    int res;
    (void)res;

    /* setup the connection context table */
    nimble_netif_conn_init();

    res = ble_l2cap_create_server(NIMBLE_NETIF_CID, NIMBLE_NETIF_MTU,
                                  _on_l2cap_server_evt, NULL);
    assert(res == 0);
    (void)res;

    gnrc_netif_create(&_netif, _stack, sizeof(_stack), NIMBLE_NETIF_PRIO,
                      "nimble_netif", &_nimble_netdev_dummy, &_nimble_netif_ops);
}

void nimble_netif_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
}

int nimble_netif_connect(const ble_addr_t *addr,
                         const nimble_netif_connect_cfg_t *params)
{
    assert(addr);
    assert(params);

    /* the netif_conn module expects addresses in network byte order */
    uint8_t addrn[BLE_ADDR_LEN];
    bluetil_addr_swapped_cp(addr->val, addrn);

    /* check that there is no open connection with the given address */
    if (nimble_netif_conn_connected(addrn) ||
        nimble_netif_conn_connecting()) {
        return -EBUSY;
    }

    /* get empty connection context */
    int handle = nimble_netif_conn_start_connection(addrn);
    if (handle == NIMBLE_NETIF_CONN_INVALID) {
        return -ENOMEM;
    }

    /* generate connection interval */
    uint16_t itvl = params->conn_itvl_min_ms;
    if (params->conn_itvl_min_ms < params->conn_itvl_max_ms) {
        itvl = nimble_netif_conn_gen_itvl(params->conn_itvl_min_ms,
                                          params->conn_itvl_max_ms);
    }
    if (itvl == 0) {
        nimble_netif_conn_free(handle, NULL);
        return -ECANCELED;
    }

    struct ble_gap_conn_params p = {
        .scan_itvl = BLE_GAP_SCAN_ITVL_MS(params->scan_itvl_ms),
        .scan_window = BLE_GAP_SCAN_WIN_MS(params->scan_window_ms),
        .itvl_min = BLE_GAP_CONN_ITVL_MS(itvl),
        .itvl_max = BLE_GAP_CONN_ITVL_MS(itvl),
        .latency = params->conn_slave_latency,
        .supervision_timeout = BLE_GAP_SUPERVISION_TIMEOUT_MS(
                                        params->conn_supervision_timeout_ms),
        .min_ce_len = 0,
        .max_ce_len = 0,
    };

#if MYNEWT_VAL_BLE_EXT_ADV
    uint8_t phy_mask;
    if (params->phy_mode == NIMBLE_PHY_1M) {
        phy_mask = BLE_GAP_LE_PHY_1M_MASK;
    }
#if IS_USED(MODULE_NIMBLE_PHY_2MBIT)
    else if (params->phy_mode == NIMBLE_PHY_2M) {
        phy_mask = (BLE_GAP_LE_PHY_1M_MASK | BLE_GAP_LE_PHY_2M_MASK);
    }
#endif
#if IS_USED(MODULE_NIMBLE_PHY_CODED)
    else if (params->phy_mode == NIMBLE_PHY_CODED) {
        phy_mask = BLE_GAP_LE_PHY_CODED_MASK;
    }
#endif
    else {
        return -EINVAL;
    }

    int res = ble_gap_ext_connect(params->own_addr_type, addr,
                                  params->timeout_ms, phy_mask, &p, &p, &p,
                                  _on_gap_master_evt, (void *)handle);
#else
    uint32_t timeout = (params->timeout_ms == 0) ? BLE_HS_FOREVER
                                                 : params->timeout_ms;
    int res = ble_gap_connect(params->own_addr_type, addr,
                              timeout, &p,
                              _on_gap_master_evt, (void *)handle);
#endif
    if (res != 0) {
        return -EIO;
    }

    _notify(handle, NIMBLE_NETIF_INIT_MASTER, addrn);
    return handle;
}

int nimble_netif_close(int handle)
{
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    if (conn == NULL) {
        return -EINVAL;
    }
    else if (!(conn->state & NIMBLE_NETIF_L2CAP_CONNECTED)) {
        return -ENOTCONN;
    }

    int res = ble_gap_terminate(conn->gaphandle, BLE_ERR_REM_USER_CONN_TERM);
    if (res != 0) {
        return -EIO;
    }

    return 0;
}

static int _accept(const uint8_t *ad, size_t ad_len, const ble_addr_t *addr,
                   const nimble_netif_accept_cfg_t *params)
{
    int handle;
    int res;
    (void)res;

    assert(params);

    /* allocate a connection context for incoming connections */
    handle = nimble_netif_conn_start_adv();
    if (handle < 0) {
        return handle;
    }

    /* remember address if applicable */
    if (addr) {
        nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
        bluetil_addr_swapped_cp(addr->val, conn->addr);
    }

#if MYNEWT_VAL_BLE_EXT_ADV
    struct ble_gap_ext_adv_params p;
    memset(&p, 0, sizeof(p));

    /* figure out PHY modes */
    int phy_pri = nimble_riot_get_phy_hci(params->primary_phy);
    int phy_sec = nimble_riot_get_phy_hci(params->secondary_phy);
    if ((phy_pri < 0) || (phy_sec < 0)) {
        nimble_netif_conn_free(handle, NULL);
        return -EINVAL;
    }
    /* the 2M PHY is not allowed as primary phy, we need to used the 1M PHY
     * instead. This is for convenience so uses may define 2M as primary PHY */
    if (phy_pri == BLE_HCI_LE_PHY_2M) {
        phy_pri = BLE_HCI_LE_PHY_1M;
    }

    if (addr != NULL) {
        p.directed = 1;
        memcpy(&p.peer, addr, sizeof(p.peer));
        if (params->flags & NIMBLE_NETIF_FLAG_HD_MODE) {
            p.high_duty_directed = 1;
        }
    }
    else {
        p.connectable = 1;
    }
    if (params->flags & NIMBLE_NETIF_FLAG_LEGACY) {
        p.legacy_pdu = 1;
        /* legacy connectable PDUs are always scannable */
        p.scannable = 1;
    }
    p.itvl_min = BLE_GAP_ADV_ITVL_MS(params->adv_itvl_ms);
    p.itvl_max = BLE_GAP_ADV_ITVL_MS(params->adv_itvl_ms);
    p.channel_map = params->channel_map;
    p.own_addr_type = params->own_addr_type;
    p.primary_phy = (uint8_t)phy_pri;
    p.secondary_phy = (uint8_t)phy_sec;
    p.tx_power = params->tx_power;

    res = ble_gap_ext_adv_configure(CONFIG_NIMBLE_NETIF_ADV_INSTANCE, &p, NULL,
                                    _on_gap_slave_evt, (void *)handle);
    if (res != 0) {
        nimble_netif_conn_free(handle, NULL);
        return -EINVAL;
    }

    if (ad != NULL) {
        struct os_mbuf *data = os_msys_get_pkthdr(ad_len, 0);
        if (data == NULL) {
            nimble_netif_conn_free(handle, NULL);
            return -ENOMEM;
        }
        res = os_mbuf_append(data, ad, ad_len);
        if (res != 0) {
            os_mbuf_free_chain(data);
            nimble_netif_conn_free(handle, NULL);
            return -ENOMEM;
        }
        res = ble_gap_ext_adv_set_data(CONFIG_NIMBLE_NETIF_ADV_INSTANCE, data);
        assert(res == 0);
    }
    res = ble_gap_ext_adv_start(CONFIG_NIMBLE_NETIF_ADV_INSTANCE, params->timeout_ms / 10, 0);
#else
    uint8_t mode = (addr != NULL) ? BLE_GAP_CONN_MODE_DIR
                                  : BLE_GAP_CONN_MODE_UND;
    struct ble_gap_adv_params p = {
        .conn_mode = mode,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
        .itvl_min = BLE_GAP_ADV_ITVL_MS(params->adv_itvl_ms),
        .itvl_max = BLE_GAP_ADV_ITVL_MS(params->adv_itvl_ms),
        .channel_map = params->channel_map,
        .filter_policy = 0,
        .high_duty_cycle = (params->flags & NIMBLE_NETIF_FLAG_HD_MODE) ? 1 : 0,
    };

    /* set advertisement data, if applicable */
    if (ad != NULL) {
        res = ble_gap_adv_set_data(ad, (int)ad_len);
        if (res != 0) {
            nimble_netif_conn_free(handle, NULL);
            return -EINVAL;
        }
    }

    /* start advertising */
    uint32_t timeout = (params->timeout_ms == 0) ? BLE_HS_FOREVER
                                                 : params->timeout_ms;
    res = ble_gap_adv_start(params->own_addr_type, addr, timeout,
                            &p, _on_gap_slave_evt, (void *)handle);
#endif
    if (res != 0) {
        nimble_netif_conn_free(handle, NULL);
        return -ECANCELED;
    }

    _notify(handle, NIMBLE_NETIF_ACCEPTING, _netif.l2addr);
    return 0;
}

int nimble_netif_accept(const uint8_t *ad, size_t ad_len,
                        const nimble_netif_accept_cfg_t *params)
{
    assert(ad != NULL);
    assert(ad_len > 0);
    return _accept(ad, ad_len, NULL, params);
}

int nimble_netif_accept_direct(const ble_addr_t *addr,
                               const nimble_netif_accept_cfg_t *params)
{
    assert(addr);
    return _accept(NULL, 0, addr, params);
}

int nimble_netif_accept_stop(void)
{
    int handle = nimble_netif_conn_get_adv();
    if (handle == NIMBLE_NETIF_CONN_INVALID) {
        return -EALREADY;
    }

    int res;
#if MYNEWT_VAL_BLE_EXT_ADV
    res = ble_gap_ext_adv_stop(CONFIG_NIMBLE_NETIF_ADV_INSTANCE);
#else
    res = ble_gap_adv_stop();
#endif
    if (res != 0) {
        return -EIO;
    }
    nimble_netif_conn_free(handle, NULL);
    _notify(handle, NIMBLE_NETIF_ACCEPT_STOP, _netif.l2addr);

    return 0;
}

int nimble_netif_update(int handle,
                        const struct ble_gap_upd_params *conn_params)
{
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    if (conn == NULL) {
        return -ENOTCONN;
    }

    int res = ble_gap_update_params(conn->gaphandle, conn_params);
    if (res != 0) {
        return -ECANCELED;
    }

    return 0;
}

int nimble_netif_used_chanmap(int handle, uint8_t map[5])
{
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    if (conn == NULL) {
        return -ENOTCONN;
    }

    int res = ble_hs_hci_read_chan_map(conn->gaphandle, map);
    if (res != 0) {
        return -ECANCELED;
    }

    return 0;
}
