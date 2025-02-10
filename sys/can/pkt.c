/*
 * Copyright (C) 2016-2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_can_dll
 * @{
 * @file
 * @brief       CAN memory allocation module
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "memarray.h"
#include "can/pkt.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define HANDLE_UNUSED 0

static int handle;
static mutex_t _mutex = MUTEX_INIT;

#ifndef CAN_PKT_BUF_SIZE
#define CAN_PKT_BUF_SIZE    128
#endif

static can_pkt_t _pkt_buf[CAN_PKT_BUF_SIZE];
static memarray_t _pkt_array;

void can_pkt_init(void)
{
    static_assert(sizeof(can_pkt_t) >= sizeof(can_rx_data_t), "sizeof(can_rx_data_t) must be at most sizeof(can_pkt_t)");
    mutex_lock(&_mutex);
    handle = 1;
    memarray_init(&_pkt_array, _pkt_buf, sizeof(can_pkt_t), CAN_PKT_BUF_SIZE);
    mutex_unlock(&_mutex);
}

static can_pkt_t *_pkt_alloc(int ifnum, const can_frame_t *frame)
{
    can_pkt_t *pkt;

    mutex_lock(&_mutex);
    pkt = memarray_alloc(&_pkt_array);
    mutex_unlock(&_mutex);

    if (!pkt) {
        return NULL;
    }

    pkt->entry.ifnum = ifnum;
    pkt->frame = *frame;

    DEBUG("can_pkt_alloc: pkt allocated\n");

    return pkt;
}

static void _init_rx_pkt(can_pkt_t *pkt)
{
    pkt->handle = 0;
    atomic_store(&pkt->ref_count, 0);
}

static void _init_tx_pkt(can_pkt_t *pkt)
{
    mutex_lock(&_mutex);
    pkt->handle = handle++;
    if (handle == INT_MAX) {
        handle = 1;
    }
    pkt->entry.next = NULL;
    mutex_unlock(&_mutex);
}

can_pkt_t *can_pkt_alloc_tx(int ifnum, const can_frame_t *frame, kernel_pid_t tx_pid)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_tx_pkt(pkt);
    pkt->entry.target.pid = tx_pid;
#ifdef MODULE_CAN_MBOX
    pkt->entry.type = CAN_TYPE_DEFAULT;
#endif

    return pkt;
}

can_pkt_t *can_pkt_alloc_rx(int ifnum, const can_frame_t *frame)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_rx_pkt(pkt);

    return pkt;
}

#ifdef MODULE_CAN_MBOX
can_pkt_t *can_pkt_alloc_mbox_tx(int ifnum, const can_frame_t *frame, mbox_t *tx_mbox)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_tx_pkt(pkt);
    pkt->entry.target.mbox = tx_mbox;
    pkt->entry.type = CAN_TYPE_MBOX;

    return pkt;
}
#endif

void can_pkt_free(can_pkt_t *pkt)
{
    assert(pkt);

    DEBUG("can_pkt_free: free pkt=%p\n", (void*)pkt);

    mutex_lock(&_mutex);
    memarray_free(&_pkt_array, pkt);
    mutex_unlock(&_mutex);
}

can_rx_data_t *can_pkt_alloc_rx_data(void *data, size_t len, void *arg)
{
    can_rx_data_t *rx;

    mutex_lock(&_mutex);
    rx = memarray_alloc(&_pkt_array);
    mutex_unlock(&_mutex);

    DEBUG("can_pkt_alloc_rx_data: rx=%p\n", (void *)rx);

    if (!rx) {
        return NULL;
    }

    rx->data.iov_base = data;
    rx->data.iov_len = len;
    rx->arg = arg;

    return rx;
}

void can_pkt_free_rx_data(can_rx_data_t *data)
{
    if (!data) {
        return;
    }

    DEBUG("can_pkt_free_rx_data: rx=%p\n", (void *)data);

    mutex_lock(&_mutex);
    memarray_free(&_pkt_array, data);
    mutex_unlock(&_mutex);
}
