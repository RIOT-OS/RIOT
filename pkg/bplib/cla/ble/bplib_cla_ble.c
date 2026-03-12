/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * This file is largely based on the [RIOT]/tests/pkg/nible_l2cap* tests.
 */

/**
 * @ingroup     pkg_bplib_cla_ble
 * @{
 *
 * @file
 * @brief       BLE L2CAP CLA implementation.
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib_cla_ble.h"

#include "bplib_init.h"

#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>

#include "nimble_riot.h"
#include "nimble_autoadv.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/util/util.h"
#include "net/bluetil/addr.h"

#include "msg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RCV_QUEUE_SIZE  8

#define BPLIB_CLA_BLE_TX_STACK_SIZE (THREAD_STACKSIZE_LARGE + CONFIG_BPLIB_CLA_BLE_MTU)
#define BPLIB_CLA_BLE_RX_STACK_SIZE (THREAD_STACKSIZE_LARGE + CONFIG_BPLIB_CLA_BLE_MTU)

/* Thread Flags */
#define FLAG_UP             (1u << 0)
#define FLAG_TX_UNSTALLED   (1u << 1)
#define FLAG_TERMINATE      (1u << 2)

/* Message types */
#define MSG_TYPE_RX_DATA    0x8657
#define MSG_TYPE_TERMINATE  0x8658

/* BLE connection state */
static uint16_t _handle = 0;
static struct ble_l2cap_chan *_coc = NULL;
static ble_addr_t _peer_addr;

/* CLA instance info */
static char stack_tx[BPLIB_CLA_BLE_TX_STACK_SIZE];
static char stack_rx[BPLIB_CLA_BLE_RX_STACK_SIZE];

static volatile bool _is_client = true;
static volatile bool _cla_running = false;
static thread_t *_tx_thread;
static kernel_pid_t _rx_thread;
static volatile uint32_t _contact_id;
static msg_t _rcv_queue[RCV_QUEUE_SIZE];

/* Functions */
static int _on_l2cap_evt(struct ble_l2cap_event *event, void *arg);
static int _on_gap_evt(struct ble_gap_event *event, void *arg);
static void* cla_ble_in(void *arg);
static void* cla_ble_out(void *arg);
static void try_connect_gap(void);
static void try_connect_l2cap(void);

static bool is_cla_running(void)
{
    return _cla_running && bplib_instance_data.running;
}

static void _on_data(struct ble_l2cap_event *event)
{
    struct os_mbuf *rxd = event->receive.sdu_rx;
    msg_t msg;

    DEBUG("# Received: L2CAP\n");

    msg.type = MSG_TYPE_RX_DATA;
    msg.content.ptr = rxd;

    msg_try_send(&msg, _rx_thread);
}

static int _on_gap_evt(struct ble_gap_event *event, void *arg)
{
    (void)arg;
    DEBUG("# GAP event %i\n", (int)event->type);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            _handle = event->connect.conn_handle;
            try_connect_l2cap();
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            _coc = NULL;
            _handle = 0;
            if (!_cla_running) {
                break;
            }
            try_connect_gap();
            break;
        case BLE_GAP_EVENT_CONN_UPDATE:
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        default:
            break;
    }
    return 0;
}

static int _on_l2cap_evt(struct ble_l2cap_event *event, void *arg)
{
    (void)arg;
    struct os_mbuf *sdu_rx;

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED: {
            _coc = event->connect.chan;
            DEBUG("# L2CAP: CONNECTED\n");
            thread_flags_set(_tx_thread, FLAG_UP);
            break;
        }
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            _coc = NULL;
            DEBUG("# L2CAP: DISCONNECTED\n");
            /* This assumed the L2CAP connection never breaks
             * without the GAP connection breaking too */
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT: {
            if (!_is_client) {
                sdu_rx = os_msys_get_pkthdr(MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
                if (sdu_rx != NULL) {
                    ble_l2cap_recv_ready(event->accept.chan, sdu_rx);
                }
            }
            break;
        }
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            _on_data(event);
            break;
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
            thread_flags_set(_tx_thread, FLAG_TX_UNSTALLED);
            break;
        default:
            break;
    }

    return 0;
}

static void try_connect_gap(void)
{
    if(_is_client) {
        DEBUG("Trying to connect to remote\n");
        ble_gap_connect(nimble_riot_own_addr_type, &_peer_addr, CONFIG_BPLIB_CLA_BLE_TIMEOUT,
            NULL, _on_gap_evt, NULL);
    } else {
        nimble_autoadv_start(NULL);
    }
}

static void try_connect_l2cap(void)
{
    if (_is_client) {
        struct os_mbuf *sdu_rx = os_msys_get_pkthdr(MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
        if (ble_l2cap_connect(_handle, CONFIG_BPLIB_CLA_BLE_CID, CONFIG_BPLIB_CLA_BLE_MTU,
            sdu_rx, _on_l2cap_evt, NULL) != 0) {
            os_mbuf_free_chain(sdu_rx);
        }
    }
}

int bplib_cla_ble_start(uint32_t contact_id, bool client)
{
    int rc;
    uint8_t addr_temp[BLE_ADDR_LEN];

    if (_cla_running) {
        return 1;
    }

    DEBUG("Starting BLE CLA\n");

    _contact_id = contact_id;
    _is_client = client;

    if (_is_client) {
        /* Client */
        _peer_addr.type = nimble_riot_own_addr_type;

        if (bluetil_addr_from_str(addr_temp, bplib_instance_data.ConfigPtrs.
            ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr) == NULL)
        {
            DEBUG("Server Address malformed\n");
            return -EINVAL;
        }
        bluetil_addr_swapped_cp(addr_temp, _peer_addr.val);
    }
    else {
        /* Server */
        if (ble_l2cap_create_server(CONFIG_BPLIB_CLA_BLE_CID, CONFIG_BPLIB_CLA_BLE_MTU,
            _on_l2cap_evt, NULL) != 0)
        {
            DEBUG("Failed to start L2CAP Server");
            return -EINVAL;
        }

        nimble_autoadv_set_gap_cb(_on_gap_evt, NULL);
    }
    try_connect_gap();

    /* This should be true before because it is the threads exit condition */
    _cla_running = true;

    rc = thread_create(stack_tx, BPLIB_CLA_BLE_TX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 2, 0, (void* (*)(void*)) cla_ble_out,
        NULL, "bplib-cla-ble-tx");
    if (rc < 0) {
        _cla_running = 0;
        return rc;
    }
    _tx_thread = thread_get(rc);

    /* The thread needs to have higher prio than nimble_host.
     * This is a separate thread because otherwise the stack size of nimble
     * would have to be increased. */
    rc = thread_create(stack_rx, BPLIB_CLA_BLE_RX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 3, 0, (void* (*)(void*)) cla_ble_in,
        NULL, "bplib-cla-ble-rx");
    if (rc < 0) {
        _cla_running = 0;
        return rc;
    }
    _rx_thread = rc;

    return 0;
}

void bplib_cla_ble_stop(void)
{
    msg_t msg = {.type = MSG_TYPE_TERMINATE};

    if (!_cla_running) {
        return;
    }

    _cla_running = 0;

    ble_gap_terminate(_handle, BLE_ERR_REM_USER_CONN_TERM);

    thread_flags_set(_tx_thread, FLAG_TERMINATE);
    msg_send(&msg, _rx_thread);

    nimble_autoadv_stop();
}

static void* cla_ble_out(void *arg)
{
    (void) arg;
    BPLib_Status_t egress_status;
    int res;
    uint8_t buffer[CONFIG_BPLIB_CLA_BLE_MTU] = {0};
    size_t out_size;
    struct os_mbuf *txd;
    thread_flags_t flags;

    while (1) {
        if (_coc == NULL) {
            flags = thread_flags_wait_any(FLAG_UP | FLAG_TERMINATE);
            if (flags & FLAG_TERMINATE || !is_cla_running()) {
                return NULL;
            }
        }

        egress_status = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, _contact_id,
            buffer, &out_size, CONFIG_BPLIB_CLA_BLE_MTU, CONFIG_BPLIB_CLA_BLE_TIMEOUT);

        if (egress_status == BPLIB_SUCCESS) {
            if (_coc == NULL) {
                /* If bplib shutdown or the CLA, the bundle is dropped */
                thread_flags_wait_any(FLAG_UP | FLAG_TERMINATE);
                if (flags & FLAG_TERMINATE || !is_cla_running()) {
                    return NULL;
                }
            }

            txd = os_msys_get_pkthdr(out_size, 0);
            if (txd == NULL) {
                DEBUG("# cla_ble_out() mbuf alloc failed\n");
                return NULL;
            }
            res = os_mbuf_append(txd, buffer, out_size);
            if (res != 0) {
                DEBUG("# cla_ble_out() mbuf append failed: %i\n", res);
                os_mbuf_free_chain(txd);
                return NULL;
            }

            DEBUG("# Sending: size %5" PRIuSIZE "\n", out_size);

            do {
                res = ble_l2cap_send(_coc, txd);
                if (res == BLE_HS_EBUSY) {
                    DEBUG("# Stalled, waiting ...\n");
                    flags = thread_flags_wait_any(FLAG_TX_UNSTALLED | FLAG_TERMINATE);
                    if (flags & FLAG_TERMINATE || !is_cla_running()) {
                        os_mbuf_free_chain(txd);
                        return NULL;
                    }
                }
            } while (res == BLE_HS_EBUSY);

            if (res != 0 && res != BLE_HS_ESTALLED) {
                DEBUG("# err: failed to send (%i)\n", res);
                os_mbuf_free_chain(txd);
            }
        }
        else if (egress_status != BPLIB_CLA_TIMEOUT) {
            DEBUG("Error egressing, RC=%"PRId32"\n", egress_status);
        }
    }
    return NULL;
}

static void* cla_ble_in(void *arg)
{
    (void) arg;
    BPLib_Status_t bp_status;
    struct os_mbuf *rxd;
    char rx_buf[CONFIG_BPLIB_CLA_BLE_MTU];
    uint16_t rx_buf_size;

    msg_t msg;
    msg_init_queue(_rcv_queue, RCV_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case MSG_TYPE_RX_DATA:
                rxd = msg.content.ptr;
                rx_buf_size = OS_MBUF_PKTLEN(rxd);
                os_mbuf_copydata(rxd, 0, rx_buf_size, rx_buf);
                os_mbuf_free_chain(rxd);

                if (!is_cla_running()) {
                    /* This also checks if bplib is still alive */
                    return NULL;
                }

                bp_status = BPLib_CLA_Ingress(&bplib_instance_data.BPLibInst, _contact_id,
                    rx_buf, rx_buf_size, 0);
                if (bp_status != BPLIB_SUCCESS) {
                    DEBUG("BPLib_CLA_Ingress Fail RC=%"PRId32"\n", bp_status);
                }

                rxd = os_msys_get_pkthdr(MYNEWT_VAL_BLE_L2CAP_COC_MPS, 0);
                if (rxd != NULL) {
                    ble_l2cap_recv_ready(_coc, rxd);
                }
                break;
            case MSG_TYPE_TERMINATE:
                return NULL;
            default:
                break;
        }
    }

    return NULL;
}