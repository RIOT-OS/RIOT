/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_can_conn
 * @{
 * @file
 * @brief       Implementation of raw CAN connection
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "can/conn/raw.h"
#include "can/can.h"
#include "can/raw.h"
#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "ztimer.h"

#define _TIMEOUT_TX_MSG_TYPE    (0x8000)
#define _TIMEOUT_RX_MSG_TYPE    (0x8001)
#define _CLOSE_CONN_MSG_TYPE    (0x8002)
#define _TIMEOUT_MSG_VALUE      (0xABCDEFAB)

#ifndef CONN_CAN_ISOTP_TIMEOUT_TX_CONF_US
#define CONN_CAN_ISOTP_TIMEOUT_TX_CONF_US   (1 * US_PER_SEC)
#endif

int conn_can_raw_create(conn_can_raw_t *conn, struct can_filter *filter, size_t count,
                        int ifnum, int flags)
{
    assert(conn != NULL);
    if (ifnum < 0 || ifnum >= CAN_DLL_NUMOF) {
        memset(conn, 0, sizeof (*conn));
        conn->ifnum = -1;
        return -ENODEV;
    }

    DEBUG("conn_can_raw_create: create conn=%p, ifnum=%d flags=%d\n", (void *)conn, ifnum, flags);

    mbox_init(&conn->mbox, conn->mbox_queue, CONN_CAN_RAW_MBOX_SIZE);
    conn->flags = flags;
    conn->count = 0;
    conn->ifnum = ifnum;

    if (flags & CONN_CAN_RECVONLY) {
        can_opt_t opt;
        opt.opt = CANOPT_STATE;
        canopt_state_t state = CANOPT_STATE_LISTEN_ONLY;
        opt.data = &state;
        opt.data_len = sizeof(state);
        int ret = raw_can_set_can_opt(ifnum, &opt);
        if (ret < 0) {
            return ret;
        }
    }

    return conn_can_raw_set_filter(conn, filter, count);
}

int conn_can_raw_set_filter(conn_can_raw_t *conn, struct can_filter *filter, size_t count)
{
    assert(conn != NULL);
    assert(filter != NULL || count == 0);

    DEBUG("conn_can_raw_set_filter: conn=%p, filter=%p, count=%" PRIuSIZE "\n",
          (void *)conn, (void *)filter, count);
    DEBUG("conn_can_raw_set_filter: conn->filter=%p, conn->count=%" PRIuSIZE "\n",
          (void *)conn->filter, conn->count);

    /* unset previous filters */
    if (conn->count) {
        for (size_t i = 0; i < conn->count; i++) {
            DEBUG("conn_can_raw_set_filter: unsetting filter=0x%" PRIx32 ", mask=0x%" PRIx32 "\n",
                 conn->filter[i].can_id, conn->filter[i].can_mask);
            raw_can_unsubscribe_rx_mbox(conn->ifnum, &conn->filter[i], &conn->mbox, conn);
        }
    }

    for (size_t i = 0; i < count; i++) {
        DEBUG("conn_can_raw_set_filter: setting filter=0x%" PRIx32 ", mask=0x%" PRIx32 "\n",
              filter[i].can_id, filter[i].can_mask);
        int ret = raw_can_subscribe_rx_mbox(conn->ifnum, &filter[i], &conn->mbox, conn);
        if (ret < 0) {
            DEBUG("conn_can_raw_set_filter: error setting filters %d\n", ret);
            for (size_t j = 0; j < i; j++) {
                DEBUG("conn_can_raw_set_filter: unsetting filter=0x%" PRIx32 ", mask=0x%" PRIx32 "\n",
                      filter[j].can_id, filter[j].can_mask);
                raw_can_unsubscribe_rx_mbox(conn->ifnum, &filter[j], &conn->mbox, conn);
            }
            return ret;
        }
    }

    conn->filter = filter;
    conn->count = count;

    return 0;
}

static void _tx_conf_timeout(void *arg)
{
    conn_can_raw_t *conn = arg;
    msg_t msg;

    msg.type = _TIMEOUT_TX_MSG_TYPE;
    msg.content.value = _TIMEOUT_MSG_VALUE;

    mbox_try_put(&conn->mbox, &msg);
}

int conn_can_raw_send(conn_can_raw_t *conn, const can_frame_t *frame, int flags)
{
    assert(conn != NULL);

    if (conn->ifnum < 0 || conn->ifnum >= CAN_DLL_NUMOF) {
        return -ENODEV;
    }

    assert((conn->flags & CONN_CAN_RECVONLY) == 0);
    assert(frame != NULL);

    int ret = 0;
    int handle;

    DEBUG("conn_can_raw_send: conn=%p, frame=%p, flags=%d\n",
          (void *)conn, (void *)frame, flags);

    if (flags & CONN_CAN_DONTWAIT) {
        handle = ret = raw_can_send(conn->ifnum, frame, 0);
        if (ret >= 0) {
            ret = 0;
        }
    }
    else {
        ztimer_t timer;
        timer.callback = _tx_conf_timeout;
        timer.arg = conn;
        ztimer_set(ZTIMER_USEC, &timer, CONN_CAN_ISOTP_TIMEOUT_TX_CONF_US);

        handle = raw_can_send_mbox(conn->ifnum, frame, &conn->mbox);
        if (handle < 0) {
            ztimer_remove(ZTIMER_USEC, &timer);
            return handle;
        }

        msg_t msg;
        int timeout = 5;
        while (1) {
            mbox_get(&conn->mbox, &msg);
            ztimer_remove(ZTIMER_USEC, &timer);
            switch (msg.type) {
            case CAN_MSG_TX_ERROR:
                return -EIO;
            case CAN_MSG_TX_CONFIRMATION:
                if ((int)msg.content.value == handle) {
                    DEBUG("conn_can_raw_send: frame sent correctly\n");
                    return 0;
                }
                else {
                    raw_can_abort(conn->ifnum, handle);
                    return -EINTR;
                }
                break;
            case _TIMEOUT_TX_MSG_TYPE:
                DEBUG("conn_can_raw_send: timeout\n");
                raw_can_abort(conn->ifnum, handle);
                return -ETIMEDOUT;
                break;
            default:
                DEBUG("conn_can_raw_send: unexpected msg=%x, requeing\n", msg.type);
                mbox_put(&conn->mbox, &msg);
                if (!timeout--) {
                    return -EINTR;
                }
                ztimer_set(ZTIMER_USEC, &timer, CONN_CAN_ISOTP_TIMEOUT_TX_CONF_US);
                break;
            }
        }
    }

    return ret;
}

static void _rx_timeout(void *arg)
{
    conn_can_raw_t *conn = arg;
    msg_t msg;

    msg.type = _TIMEOUT_RX_MSG_TYPE;
    msg.content.value = _TIMEOUT_MSG_VALUE;

    mbox_try_put(&conn->mbox, &msg);
}

int conn_can_raw_recv(conn_can_raw_t *conn, can_frame_t *frame, uint32_t timeout)
{
    assert(conn != NULL);

    if (conn->ifnum < 0 || conn->ifnum >= CAN_DLL_NUMOF) {
        return -ENODEV;
    }

    assert(frame != NULL);

    ztimer_t timer;

    if (timeout != 0) {
        timer.callback = _rx_timeout;
        timer.arg = conn;
        ztimer_set(ZTIMER_USEC, &timer, timeout);
    }

    int ret;
    msg_t msg;
    can_rx_data_t *rx;

    mbox_get(&conn->mbox, &msg);
    if (timeout != 0) {
        ztimer_remove(ZTIMER_USEC, &timer);
    }
    switch (msg.type) {
    case CAN_MSG_RX_INDICATION:
        DEBUG("conn_can_raw_recv: CAN_MSG_RX_INDICATION\n");
        rx = msg.content.ptr;
        memcpy(frame, rx->data.iov_base, rx->data.iov_len);
        ret = rx->data.iov_len;
        raw_can_free_frame(rx);
        break;
    case _TIMEOUT_RX_MSG_TYPE:
        if (msg.content.value == _TIMEOUT_MSG_VALUE) {
            ret = -ETIMEDOUT;
        }
        else {
            ret = -EINTR;
        }
        break;
    case _CLOSE_CONN_MSG_TYPE:
        if (msg.content.ptr == conn) {
            ret = -ECONNABORTED;
        }
        else {
            ret = -EINTR;
        }
        break;
    default:
        mbox_put(&conn->mbox, &msg);
        ret = -EINTR;
        break;
    }

    return ret;
}

int conn_can_raw_close(conn_can_raw_t *conn)
{
    assert(conn != NULL);

    if (conn->ifnum < 0 || conn->ifnum >= CAN_DLL_NUMOF) {
        return -ENODEV;
    }

    DEBUG("conn_can_raw_close: conn=%p\n", (void *)conn);

    if (conn->count) {
        for (size_t i = 0; i < conn->count; i++) {
            DEBUG("conn_can_raw_close: unsetting filter=0x%" PRIx32 ", mask=0x%" PRIx32 "\n",
                 conn->filter[i].can_id, conn->filter[i].can_mask);
            raw_can_unsubscribe_rx_mbox(conn->ifnum, &conn->filter[i], &conn->mbox, conn);
        }
        conn->count = 0;
        msg_t msg;
        while (mbox_try_get(&conn->mbox, &msg)) {
            if (msg.type == CAN_MSG_RX_INDICATION) {
                DEBUG("conn_can_raw_close: incoming msg pending, freeing\n");
                raw_can_free_frame(msg.content.ptr);
            }
        }
        msg.type = _CLOSE_CONN_MSG_TYPE;
        msg.content.ptr = conn;
        mbox_try_put(&conn->mbox, &msg);
    }

    return 0;
}
