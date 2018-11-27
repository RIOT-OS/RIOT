/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
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

#define ENABLE_DEBUG            (0)
#include "debug.h"

#ifdef MODULE_GNRC_SIXLOWPAN
#define NETTYPE                 GNRC_NETTYPE_SIXLOWPAN
#elif defined(MODULE_GNRC_IPV6)
#define NETTYPE                 GNRC_NETTYPE_IPV6
#else
#define NETTYPE                 GNRC_NETTYPE_UNDEF
#endif

/* buffer configuration
 * - we need one RX and one TX buffer per connection */
#define MTU_SIZE                (NIMBLE_NETIF_MTU)
#define MBUF_OVHD               (sizeof(struct os_mbuf) + \
                                 sizeof(struct os_mbuf_pkthdr))
#define MBUF_SIZE               (MBUF_OVHD + MYNEWT_VAL_BLE_L2CAP_COC_MPS)
#define MBUF_CNT                (NIMBLE_NETIF_MAX_CONN * 2 * \
                                 ((MTU_SIZE + (MBUF_SIZE - 1)) / MBUF_SIZE))

/* thread flag used for signaling transmit readiness */
#define FLAG_TX_UNSTALLED       (1u << 13)

/* allocate a stack for the netif device */
static char _stack[THREAD_STACKSIZE_DEFAULT];
static thread_t *_netif_thread;

/* keep the actual device state */
static gnrc_netif_t *_nimble_netif = NULL;
static gnrc_nettype_t _nettype = NETTYPE;

/* keep a reference to the event callback */
static nimble_netif_eventcb_t _eventcb;

/* allocation of memory for buffering IP packets when handing them to NimBLE */
static os_membuf_t _mem[OS_MEMPOOL_SIZE(MBUF_CNT, MBUF_SIZE)];
static struct os_mempool _mem_pool;
static struct os_mbuf_pool _mbuf_pool;

/* notify the user about state changes for a connection context */
static void _notify(int handle, nimble_netif_event_t event)
{
    if (_eventcb) {
        _eventcb(handle, event);
    }
}

static void _netif_init(gnrc_netif_t *netif)
{
    (void)netif;

    gnrc_netif_default_init(netif);
    /* save the threads context pointer, so we can set its flags */
    _netif_thread = (thread_t *)thread_get(thread_getpid());

#ifdef MODULE_GNRC_SIXLOWPAN
    /* we disable fragmentation for this device, as the L2CAP layer takes care
     * of this */
    _nimble_netif->sixlo.max_frag_size = 0;
#endif
}

static int _send_pkt(nimble_netif_conn_t *conn, gnrc_pktsnip_t *pkt)
{
    int res;
    int num_bytes = 0;

    if (conn == NULL || conn->coc == NULL) {
        return -ENOTCONN;
    }

    /* copy the data into a newly allocated mbuf */
    struct os_mbuf *sdu = os_mbuf_get_pkthdr(&_mbuf_pool, 0);
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
            thread_flags_wait_all(FLAG_TX_UNSTALLED);
        }
    } while (res == BLE_HS_EBUSY);

    if ((res != 0) && (res != BLE_HS_ESTALLED)) {
        os_mbuf_free_chain(sdu);
        return -ENOBUFS;
    }

    return num_bytes;
}

static int _netif_send_iter(nimble_netif_conn_t *conn,
                            int handle, void *arg)
{
    (void)handle;
    _send_pkt(conn, (gnrc_pktsnip_t *)arg);
    return 0;
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
        nimble_netif_conn_foreach(NIMBLE_NETIF_L2CAP_CONNECTED,
                                  _netif_send_iter, pkt->next);
        res = (int)gnrc_pkt_len(pkt->next);
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
    _nimble_netif = dev->context;

    /* get our own address from the controller */
    uint8_t tmp[6];
    int res = ble_hs_id_copy_addr(nimble_riot_own_addr_type, tmp, NULL);
    assert(res == 0);
    (void)res;

    bluetil_addr_swapped_cp(tmp, _nimble_netif->l2addr);
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
            memcpy(value, _nimble_netif->l2addr, BLE_ADDR_LEN);
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
            *((uint16_t *)value) = MTU_SIZE;
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
                                                   _nimble_netif->l2addr,
                                                   BLE_ADDR_LEN);
    if (if_snip == NULL) {
        goto end;
    }

    /* we need to add the device PID to the netif header */
    gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t *)if_snip->data;
    netif_hdr->if_pid = _nimble_netif->pid;

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
    rxb = os_mbuf_get_pkthdr(&_mbuf_pool, 0);
    /* due to buffer provisioning, there should always be enough space */
    assert(rxb != NULL);
    ble_l2cap_recv_ready(event->receive.chan, rxb);
}

static int _on_l2cap_client_evt(struct ble_l2cap_event *event, void *arg)
{
    int handle = (int)arg;
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    assert(conn && (conn->state & NIMBLE_NETIF_GAP_MASTER));

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED:
            conn->coc = event->connect.chan;
            conn->state |= NIMBLE_NETIF_L2CAP_CLIENT;
            conn->state &= ~NIMBLE_NETIF_CONNECTING;
            _notify(handle, NIMBLE_NETIF_CONNECTED_MASTER);
            break;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            assert(conn->coc);
            conn->coc = NULL;
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
            conn->coc = event->connect.chan;
            conn->state |= NIMBLE_NETIF_L2CAP_SERVER;
            conn->state &= ~(NIMBLE_NETIF_ADV | NIMBLE_NETIF_CONNECTING);
            _notify(handle, NIMBLE_NETIF_CONNECTED_SLAVE);
            break;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            conn = nimble_netif_conn_from_gaphandle(event->disconnect.conn_handle);
            assert(conn && conn->coc);
            conn->coc = NULL;
            conn->state &= ~NIMBLE_NETIF_L2CAP_CONNECTED;
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT: {
            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&_mbuf_pool, 0);
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
    bluetil_addr_swapped_cp(desc.peer_id_addr.val, conn->addr);
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
                nimble_netif_conn_free(handle);
                _notify(handle, NIMBLE_NETIF_CONNECT_ABORT);
                return 0;
            }
            _on_gap_connected(conn, event->connect.conn_handle);
            conn->state |= NIMBLE_NETIF_GAP_MASTER;

            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&_mbuf_pool, 0);
            /* we should never run out of buffer space... */
            assert(sdu_rx != NULL);
            res = ble_l2cap_connect(event->connect.conn_handle,
                                    NIMBLE_NETIF_CID, MTU_SIZE, sdu_rx,
                                    _on_l2cap_client_evt, (void *)handle);
            /* should always success as well */
            assert(res == 0);
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT:
            nimble_netif_conn_free(handle);
            _notify(handle, NIMBLE_NETIF_CLOSED_MASTER);
            break;
        case BLE_GAP_EVENT_CONN_UPDATE:
            _notify(handle, NIMBLE_NETIF_CONN_UPDATED);
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
                nimble_netif_conn_free(handle);
                _notify(handle, NIMBLE_NETIF_CONNECT_ABORT);
                break;
            }
            _on_gap_connected(conn, event->connect.conn_handle);
            assert(conn->state == NIMBLE_NETIF_ADV);
            conn->state = NIMBLE_NETIF_GAP_SLAVE;
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT:
            nimble_netif_conn_free(handle);
            _notify(handle, NIMBLE_NETIF_CLOSED_SLAVE);
            break;
        case BLE_GAP_EVENT_CONN_UPDATE:
            _notify(handle, NIMBLE_NETIF_CONN_UPDATED);
            break;
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
            /* nothing to do here */
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

    /* initialize of BLE related buffers */
    res = os_mempool_init(&_mem_pool, MBUF_CNT, MBUF_SIZE, _mem, "nim_gnrc");
    assert(res == 0);
    res = os_mbuf_pool_init(&_mbuf_pool, &_mem_pool, MBUF_SIZE, MBUF_CNT);
    assert(res == 0);

    res = ble_l2cap_create_server(NIMBLE_NETIF_CID, MTU_SIZE,
                                  _on_l2cap_server_evt, NULL);
    assert(res == 0);
    (void)res;

    gnrc_netif_create(_stack, sizeof(_stack), GNRC_NETIF_PRIO,
                      "nimble_netif", &_nimble_netdev_dummy, &_nimble_netif_ops);
}

void nimble_netif_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
}

int nimble_netif_connect(const ble_addr_t *addr,
                         const struct ble_gap_conn_params *conn_params,
                         uint32_t timeout)
{
    assert(addr);
    assert(_eventcb);

    /* the netif_conn module expects addresses in network byte order */
    uint8_t addrn[BLE_ADDR_LEN];
    bluetil_addr_swapped_cp(addr->val, addrn);

    /* check that there is no open connection with the given address */
    if (nimble_netif_conn_connected(addrn) ||
        nimble_netif_conn_connecting()) {
        return NIMBLE_NETIF_BUSY;
    }

    /* get empty connection context */
    int handle = nimble_netif_conn_start_connection(addrn);
    if (handle == NIMBLE_NETIF_CONN_INVALID) {
        return NIMBLE_NETIF_NOMEM;
    }

    int res = ble_gap_connect(nimble_riot_own_addr_type, addr, timeout,
                              conn_params, _on_gap_master_evt, (void *)handle);
    assert(res == 0);
    (void)res;

    return handle;
}

int nimble_netif_close(int handle)
{
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    if (conn == NULL) {
        return NIMBLE_NETIF_NOTFOUND;
    }
    else if (!(conn->state & NIMBLE_NETIF_L2CAP_CONNECTED)) {
        return NIMBLE_NETIF_NOTCONN;
    }

    int res = ble_gap_terminate(ble_l2cap_get_conn_handle(conn->coc),
                                BLE_ERR_REM_USER_CONN_TERM);
    assert(res == 0);
    (void)res;

    return NIMBLE_NETIF_OK;
}

int nimble_netif_accept(const uint8_t *ad, size_t ad_len,
                        const struct ble_gap_adv_params *adv_params)
{
    assert(ad);
    assert(adv_params);

    int handle;
    int res;
    (void)res;

    /* allocate a connection context for incoming connections */
    handle = nimble_netif_conn_start_adv();
    if (handle < 0) {
        return handle;
    }

    /* set advertisement data */
    res = ble_gap_adv_set_data(ad, (int)ad_len);
    assert(res == 0);
    /* remember context and start advertising */
    res = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                            adv_params, _on_gap_slave_evt, (void *)handle);
    assert(res == 0);

    return NIMBLE_NETIF_OK;
}

int nimble_netif_accept_stop(void)
{
    int handle = nimble_netif_conn_get_adv();
    if (handle == NIMBLE_NETIF_CONN_INVALID) {
        return NIMBLE_NETIF_NOTADV;
    }

    int res = ble_gap_adv_stop();
    assert(res == 0);
    (void)res;
    nimble_netif_conn_free(handle);

    return NIMBLE_NETIF_OK;
}

int nimble_netif_update(int handle,
                        const struct ble_gap_upd_params *conn_params)
{
    nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
    if (conn == NULL) {
        return NIMBLE_NETIF_NOTCONN;
    }

    int res = ble_gap_update_params(conn->gaphandle, conn_params);
    if (res != 0) {
        return NIMBLE_NETIF_DEVERR;
    }

    return NIMBLE_NETIF_OK;
}
