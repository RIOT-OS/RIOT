/*
 * Copyright (C) 2016 OTA keys S.A.
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

#include <string.h>
#include <limits.h>
#include <errno.h>

#include "net/gnrc/pktbuf.h"
#include "can/pkt.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define HANDLE_UNUSED 0

static int handle;
static mutex_t _mutex = MUTEX_INIT;

void can_pkt_init(void)
{
    mutex_lock(&_mutex);
    handle = 1;
    mutex_unlock(&_mutex);
}

static can_pkt_t *_pkt_alloc(int ifnum, const struct can_frame *frame)
{
    can_pkt_t *pkt;

    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, sizeof(*pkt), GNRC_NETTYPE_UNDEF);
    if (!snip) {
        DEBUG("can_pkt_alloc: out of memory\n");
        return NULL;
    }

    pkt = snip->data;
    pkt->entry.ifnum = ifnum;
    pkt->frame = *frame;
    pkt->snip = snip;

    DEBUG("can_pkt_alloc: pkt allocated\n");

    return pkt;
}

static void _init_pkt(can_pkt_t *pkt, int tx)
{
    if (tx) {
        mutex_lock(&_mutex);
        pkt->handle = handle++;
        if (handle == INT_MAX) {
            handle = 1;
        }
        pkt->entry.next = NULL;
        mutex_unlock(&_mutex);
    }
    else {
        pkt->handle = 0;
        atomic_store(&pkt->ref_count, 0);
    }
}

can_pkt_t *can_pkt_alloc_tx(int ifnum, const struct can_frame *frame, kernel_pid_t tx_pid)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_pkt(pkt, 1);
    pkt->entry.target.pid = tx_pid;
#ifdef MODULE_CAN_MBOX
    pkt->entry.type = CAN_TYPE_DEFAULT;
#endif

    return pkt;
}

can_pkt_t *can_pkt_alloc_rx(int ifnum, const struct can_frame *frame)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_pkt(pkt, 0);

    return pkt;
}

#ifdef MODULE_CAN_MBOX
can_pkt_t *can_pkt_alloc_mbox_tx(int ifnum, const struct can_frame *frame, mbox_t *tx_mbox)
{
    can_pkt_t *pkt = _pkt_alloc(ifnum, frame);

    if (!pkt) {
        return NULL;
    }

    _init_pkt(pkt, 1);
    pkt->entry.target.mbox = tx_mbox;
    pkt->entry.type = CAN_TYPE_MBOX;

    return pkt;
}
#endif

void can_pkt_free(can_pkt_t *pkt)
{
    if (!pkt) {
        return;
    }

    DEBUG("can_pkt_free: free pkt=%p\n", (void*)pkt);

    gnrc_pktbuf_release(pkt->snip);
}

can_rx_data_t *can_pkt_alloc_rx_data(void *data, size_t len, void *arg)
{
    can_rx_data_t *rx;

    gnrc_pktsnip_t *snip = gnrc_pktbuf_add(NULL, NULL, sizeof(*rx), GNRC_NETTYPE_UNDEF);
    if (!snip) {
        DEBUG("can_pkt_alloc_rx_data: out of memory\n");
        return NULL;
    }

    rx = snip->data;
    DEBUG("can_pkt_alloc_rx_data: rx=%p\n", (void *)rx);

    rx->data.iov_base = data;
    rx->data.iov_len = len;
    rx->arg = arg;
    rx->snip = snip;

    return rx;
}

void can_pkt_free_rx_data(can_rx_data_t *data)
{
    if (!data) {
        return;
    }

    gnrc_pktbuf_release(data->snip);
}
