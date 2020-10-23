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
 * @brief       Implementation of isotp CAN connection
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifdef MODULE_CAN_ISOTP
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "can/conn/isotp.h"
#include "can/isotp.h"
#include "can/device.h"

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
#include "utlist.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#include "xtimer.h"

#define _TIMEOUT_TX_MSG_TYPE    (0x8000)
#define _TIMEOUT_RX_MSG_TYPE    (0x8001)
#define _CLOSE_CONN_MSG_TYPE    (0x8002)
#define _TIMEOUT_MSG_VALUE      (0xABCDEFAB)

#ifndef CONN_CAN_ISOTP_TIMEOUT_TX_CONF
#define CONN_CAN_ISOTP_TIMEOUT_TX_CONF (10 * US_PER_SEC)
#endif

static inline int try_put_msg(conn_can_isotp_t *conn, msg_t *msg)
{
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    return mbox_try_put(&conn->master->mbox, msg);
#else
    return mbox_try_put(&conn->mbox, msg);
#endif
}

static inline void put_msg(conn_can_isotp_t *conn, msg_t *msg)
{
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    mbox_put(&conn->master->mbox, msg);
#else
    mbox_put(&conn->mbox, msg);
#endif
}

static inline void get_msg(conn_can_isotp_t *conn, msg_t *msg)
{
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    mbox_get(&conn->master->mbox, msg);
#else
    mbox_get(&conn->mbox, msg);
#endif
}

int conn_can_isotp_create(conn_can_isotp_t *conn, struct isotp_options *options, int ifnum)
{
    assert(conn != NULL);
    assert(options != NULL);
    assert(ifnum < CAN_DLL_NUMOF);

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    DEBUG("conn_can_isotp_create: conn=%p, conn->master=%p, ifnum=%d\n",
          (void *)conn, (void *)conn->master, ifnum);

    if (conn->master == conn || conn->master == NULL) {
        conn->master = conn;
        conn->master->next = NULL;
        mutex_init(&conn->master->lock);
        mutex_lock(&conn->master->lock);
        DEBUG("conn_can_isotp_create: init master conn\n");
        mbox_init(&conn->master->mbox, conn->master->mbox_queue, CONN_CAN_ISOTP_MBOX_SIZE);
        mutex_unlock(&conn->master->lock);
    }
#else
    mbox_init(&conn->mbox, conn->mbox_queue, CONN_CAN_ISOTP_MBOX_SIZE);
#endif

    conn->ifnum = ifnum;

    memset(&conn->isotp, 0, sizeof(struct isotp));
    conn->isotp.opt = *options;

    return 0;
}

int conn_can_isotp_bind(conn_can_isotp_t *conn, struct isotp_fc_options *fc_options)
{
    assert(conn != NULL);
    assert(conn->isotp.opt.tx_id != 0 || conn->isotp.opt.rx_id != 0);

    DEBUG("conn_can_isotp_bind: conn=%p, ifnum=%d\n",
          (void *)conn, conn->ifnum);

    if (conn->bound) {
        return -EALREADY;
    }
    msg_t msg;
    int ret;
    can_reg_entry_t entry;
    entry.ifnum = conn->ifnum;
    entry.type = CAN_TYPE_MBOX;
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    assert(conn->master != NULL);

    entry.target.mbox = &(conn->master->mbox);
    if (conn != conn->master) {
        mutex_lock(&conn->master->lock);
        LL_APPEND(conn->master->next, (conn_can_isotp_slave_t *)conn);
        mutex_unlock(&conn->master->lock);
    }
    ret = mbox_try_get(&conn->master->mbox, &msg);
#else
    entry.target.mbox = &conn->mbox;
    ret = mbox_try_get(&conn->mbox, &msg);
#endif
    if ((ret == 1) && (msg.type != _CLOSE_CONN_MSG_TYPE)) {
        DEBUG("conn_can_isotp_bind: msg in queue type=%x\n", msg.type);
        put_msg(conn, &msg);
    }

    ret = isotp_bind(&conn->isotp, &entry, conn, fc_options);
    if (!ret) {
        conn->bound = 1;
    }
    return ret;
}

static void _tx_conf_timeout(void *arg)
{
    conn_can_isotp_t *conn = arg;
    msg_t msg;

    msg.type = _TIMEOUT_TX_MSG_TYPE;
    msg.content.value = _TIMEOUT_MSG_VALUE;

    try_put_msg(conn, &msg);
}

int conn_can_isotp_send(conn_can_isotp_t *conn, const void *buf, size_t size, int flags)
{
    assert(conn != NULL);
    assert(buf != NULL || size == 0);

    int ret = 0;

    if (!conn->bound) {
        return -ENOTCONN;
    }

    if (flags & CAN_ISOTP_TX_DONT_WAIT) {
        return isotp_send(&conn->isotp, buf, size, flags);
    }
    else {
        xtimer_t timer;
        timer.callback = _tx_conf_timeout;
        timer.arg = conn;
        xtimer_set(&timer, CONN_CAN_ISOTP_TIMEOUT_TX_CONF);

        ret = isotp_send(&conn->isotp, buf, size, flags);

        msg_t msg;
        while (1) {
            get_msg(conn, &msg);
            switch (msg.type) {
            case CAN_MSG_TX_ERROR:
                if (msg.content.ptr == conn) {
                    ret = -EIO;
                }
                /* Fall through */
            case CAN_MSG_TX_CONFIRMATION:
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
                if (msg.content.ptr != conn) {
                    mbox_put(&conn->master->mbox, &msg);
                    break;
                }
#endif
                xtimer_remove(&timer);
                return ret;
            case _TIMEOUT_TX_MSG_TYPE:
                return -ETIMEDOUT;
            default:
                DEBUG("conn_can_isotp_send: unexpected msg %x, requeing\n", msg.type);
                put_msg(conn, &msg);
                break;
            }
        }
    }

    return ret;
}

static void _rx_timeout(void *arg)
{
    conn_can_isotp_t *conn = arg;
    msg_t msg;

    msg.type = _TIMEOUT_RX_MSG_TYPE;
    msg.content.value = _TIMEOUT_MSG_VALUE;

    try_put_msg(conn, &msg);
}

int conn_can_isotp_recv(conn_can_isotp_t *conn, void *buf, size_t size, uint32_t timeout)
{
    assert(conn != NULL);
    assert(buf != NULL);

    int ret = 0;
    gnrc_pktsnip_t *snip;

    if (!conn->bound) {
        return -ENOTCONN;
    }

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    if (conn->rx) {
        snip = conn->rx->data.iov_base;
        if (snip->size <= size) {
            memcpy(buf, snip->data, snip->size);
            ret = snip->size;
        }
        else {
            ret = -EOVERFLOW;
        }
        isotp_free_rx(conn->rx);
        conn->rx = NULL;
        return ret;
    }
#endif

    xtimer_t timer;
    if (timeout != 0) {
        timer.callback = _rx_timeout;
        timer.arg = conn;
        xtimer_set(&timer, timeout);
    }

    msg_t msg;
    can_rx_data_t *rx;

    while (1) {
        get_msg(conn, &msg);
        switch (msg.type) {
        case CAN_MSG_RX_INDICATION:
            DEBUG("conn_can_isotp_recv: CAN_MSG_RX_INDICATION\n");
            rx = msg.content.ptr;
            snip = rx->data.iov_base;
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
            if (rx->arg != conn) {
                mbox_put(&conn->master->mbox, &msg);
                break;
            }
#endif
            if (timeout != 0) {
                xtimer_remove(&timer);
            }
            if (snip->size <= size) {
                memcpy(buf, snip->data, snip->size);
                ret = snip->size;
            }
            else {
                ret = -EOVERFLOW;
            }
            isotp_free_rx(rx);
            return ret;
        case _TIMEOUT_RX_MSG_TYPE:
            DEBUG("conn_can_isotp_recv: _TIMEOUT_RX_MSG_TYPE\n");
            if (msg.content.value == _TIMEOUT_MSG_VALUE) {
                ret = -ETIMEDOUT;
            }
            else {
                ret = -EINTR;
            }
            return ret;
        case _CLOSE_CONN_MSG_TYPE:
            DEBUG("conn_can_isotp_recv: _CLOSE_CONN_MSG_TYPE\n");
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
            if ((msg.content.ptr == conn) || (msg.content.ptr == conn->master)) {
#endif
                if (timeout != 0) {
                    xtimer_remove(&timer);
                }
                return -ECONNABORTED;
#ifdef MODULE_CONN_CAN_ISOTP_MULTI
            }
#endif
            break;
        default:
            DEBUG("conn_can_isotp_recv: unexpected msg %x\n", msg.type);
            if (timeout != 0) {
                xtimer_remove(&timer);
            }
            ret = -EINTR;
            return ret;
        }
    }

    return ret;
}

int conn_can_isotp_close(conn_can_isotp_t *conn)
{
    assert(conn != NULL);
    msg_t msg;

    DEBUG("conn_can_isotp_close: conn=%p, ifnum=%d\n",
          (void *)conn, conn->ifnum);

    if (!conn->bound) {
        return -EALREADY;
    }

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    assert(conn->master != NULL);

    if (conn->master != conn) {
        mutex_lock(&conn->master->lock);
        LL_DELETE(conn->master->next, (conn_can_isotp_slave_t *)conn);
        mutex_unlock(&conn->master->lock);
    }
    else {
        if (conn->master->next) {
            return -EBUSY;
        }
    }
#endif

    isotp_release(&conn->isotp);

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    if (conn->rx) {
        isotp_free_rx(conn->rx);
    }
    if (conn->master == conn) {
        while (mbox_try_get(&conn->master->mbox, &msg)) {
            if (msg.type == CAN_MSG_RX_INDICATION) {
                DEBUG("conn_can_isotp_close: freeing %p\n", msg.content.ptr);
                isotp_free_rx(msg.content.ptr);
            }
        }
    }
#else
    while (mbox_try_get(&conn->mbox, &msg)) {
        if (msg.type == CAN_MSG_RX_INDICATION) {
            DEBUG("conn_can_isotp_close: freeing %p\n", msg.content.ptr);
            isotp_free_rx(msg.content.ptr);
        }
    }
#endif

    msg.type = _CLOSE_CONN_MSG_TYPE;
    msg.content.ptr = conn;
    try_put_msg(conn, &msg);

    conn->bound = 0;

    return 0;
}

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
int conn_can_isotp_select(conn_can_isotp_slave_t **conn, conn_can_isotp_t *master, uint32_t timeout)
{
    assert(master != NULL);
    assert(conn != NULL);

    int ret;

    xtimer_t timer;
    if (timeout != 0) {
        timer.callback = _rx_timeout;
        timer.arg = master;
        xtimer_set(&timer, timeout);
    }

    msg_t msg;
    can_rx_data_t *rx;

    mbox_get(&master->mbox, &msg);

    if (timeout != 0) {
        xtimer_remove(&timer);
    }
    switch (msg.type) {
    case CAN_MSG_RX_INDICATION:
        DEBUG("conn_can_isotp_select: CAN_MSG_RX_INDICATION\n");
        rx = msg.content.ptr;
        *conn = rx->arg;
        (*conn)->rx = rx;
        ret = 0;
        break;
    case _TIMEOUT_RX_MSG_TYPE:
        DEBUG("conn_can_isotp_select: _TIMEOUT_MSG_VALUE\n");
        if (msg.content.value == _TIMEOUT_MSG_VALUE) {
            ret = -ETIMEDOUT;
        }
        else {
            ret = -EINTR;
        }
        *conn = NULL;
        break;
    default:
        DEBUG("conn_can_isotp_select: %d\n", msg.type);
        *conn = NULL;
        ret = -EINTR;
        break;
    }

    return ret;
}
#endif /* MODULE_CONN_CAN_ISOTP_MULTI */

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CAN_ISOTP */
