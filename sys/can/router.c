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
 * @brief       Functions for routing RX can frames
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <inttypes.h>

#include "kernel_defines.h"

#include "can/router.h"
#include "can/pkt.h"
#include "can/device.h"
#include "utlist.h"
#include "mutex.h"
#include "assert.h"
#include "memarray.h"

#include "mbox.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * This is a can_id element
 */
typedef struct filter_el {
    can_reg_entry_t entry;   /**< filter entry */
    canid_t can_id;          /**< CAN ID of the element */
    canid_t mask;            /**< Mask of the element */
    void *data;              /**< Private data */
} filter_el_t;

/**
 * This table contains @p CAN_ROUTER_APP_MAX lists of CAN IDs per interface
 */
static can_reg_entry_t *table[CAN_DLL_NUMOF];

#ifndef CAN_ROUTER_MAX_FILTER
#define CAN_ROUTER_MAX_FILTER   64
#endif

static filter_el_t _filter_buf[CAN_ROUTER_MAX_FILTER];
static memarray_t _filter_array;
static mutex_t lock = MUTEX_INIT;

static filter_el_t *_alloc_filter_el(canid_t can_id, canid_t mask, void *data);
static void _free_filter_el(filter_el_t *el);
static void _insert_to_list(can_reg_entry_t **list, filter_el_t *el);
static filter_el_t *_find_filter_el(can_reg_entry_t *list, can_reg_entry_t *entry, canid_t can_id, canid_t mask, void *data);
static int _filter_is_used(unsigned int ifnum, canid_t can_id, canid_t mask);

#if IS_ACTIVE(ENABLE_DEBUG)
static void _print_filters(void)
{
    for (int i = 0; i < (int)CAN_DLL_NUMOF; i++) {
        DEBUG("--- Ifnum: %d ---\n", i);
        can_reg_entry_t *entry;
        LL_FOREACH(table[i], entry) {
            filter_el_t *el = container_of(entry, filter_el_t, entry);
            DEBUG("App pid=%" PRIkernel_pid ", el=%p, can_id=0x%" PRIx32 ", mask=0x%" PRIx32 ", data=%p\n",
                  el->entry.target.pid, (void*)el, el->can_id, el->mask, el->data);
        }
    }
}
#define PRINT_FILTERS() _print_filters()
#else
#define PRINT_FILTERS()
#endif

void can_router_init(void)
{
    mutex_init(&lock);
    memarray_init(&_filter_array, _filter_buf, sizeof(filter_el_t), CAN_ROUTER_MAX_FILTER);
}

static filter_el_t *_alloc_filter_el(canid_t can_id, canid_t mask, void *data)
{
    filter_el_t *el;
    el = memarray_alloc(&_filter_array);
    if (!el) {
        DEBUG("can_router: _alloc_canid_el: out of memory\n");
        return NULL;
    }

    el->can_id = can_id;
    el->mask = mask;
    el->data = data;
    el->entry.next = NULL;
    DEBUG("_alloc_canid_el: el allocated with can_id=0x%" PRIx32 ", mask=0x%" PRIx32
          ", data=%p\n", can_id, mask, data);
    return el;
}

static void _free_filter_el(filter_el_t *el)
{
    assert(el);

    DEBUG("_free_canid_el: el freed with can_id=0x%" PRIx32 ", mask=0x%" PRIx32
          ", data=%p\n", el->can_id, el->mask, el->data);

    memarray_free(&_filter_array, el);
}

/* Insert to the list in a sorted way
 * Lower CAN IDs are inserted first */
static void _insert_to_list(can_reg_entry_t **list, filter_el_t *el)
{
    can_reg_entry_t *next_entry = *list;
    filter_el_t *next_el = container_of(next_entry, filter_el_t, entry);

    DEBUG("_insert_to_list: list=%p, el=%p\n", (void *)list, (void *)el);

    if (!(*list) || (next_el->can_id > el->can_id)) {
        LL_PREPEND(*list, &el->entry);
        DEBUG("_insert_to_list: inserting first el, list=%p\n", (void *)list);
    }
    else {
        do {
            if (el->can_id <= next_el->can_id) {
                DEBUG("_insert_to_list: found next_el can_id:0x%" PRIx32
                      "\n", next_el->can_id);
                LL_PREPEND_ELEM(*list, next_entry, &el->entry);
                return;
            }
            else if (next_el->entry.next == NULL) {
                DEBUG("_insert_to_list: insert at the end\n");
                LL_APPEND(next_entry, &el->entry);
                return;
            }
            else {
                next_entry = next_entry->next;
                next_el = container_of(next_entry, filter_el_t, entry);
                DEBUG("_insert_to_list: going to next el: %p\n", (void*) next_el);
            }
        } while (next_el);

    }
}

#ifdef MODULE_CAN_MBOX
#define ENTRY_MATCHES(e1, e2) (((e1)->type == (e2)->type) && \
    (((e1)->type == CAN_TYPE_DEFAULT && (e1)->target.pid == (e2)->target.pid) ||\
    ((e1)->type == CAN_TYPE_MBOX && (e1)->target.mbox == (e2)->target.mbox)))
#else
#define ENTRY_MATCHES(e1, e2)  ((e1)->target.pid == (e2)->target.pid)
#endif

static filter_el_t *_find_filter_el(can_reg_entry_t *list, can_reg_entry_t *entry, canid_t can_id, canid_t mask, void *data)
{
    filter_el_t *el = container_of(list, filter_el_t, entry);
    if (!el) {
        return el;
    }
    do {
        if ((el->can_id == can_id) && (el->mask == mask) && (el->data == data) &&
                ENTRY_MATCHES(&el->entry, entry)) {
            DEBUG("_find_filter_el: found el=%p, can_id=%" PRIx32 ", mask=%" PRIx32 ", data=%p\n",
                  (void *)el, el->can_id, el->mask, el->data);
            return el;
        }
        el = container_of(el->entry.next, filter_el_t, entry);
    }  while (el);

    return NULL;
}

static int _filter_is_used(unsigned int ifnum, canid_t can_id, canid_t mask)
{
    filter_el_t *el = container_of(table[ifnum], filter_el_t, entry);
    if (!el) {
        DEBUG("_filter_is_used: empty list\n");
        return 0;
    }
    do {
        if ((el->can_id == can_id) && (el->mask == mask)) {
            DEBUG("_filter_is_used: found el=%p, can_id=%" PRIx32 ", mask=%" PRIx32 ", data=%p\n",
                  (void *)el, el->can_id, el->mask, el->data);
            return 1;
        }
        el = container_of(el->entry.next, filter_el_t, entry);
    }  while (el);

    DEBUG("_filter_is_used: filter not found\n");

    return 0;
}

/* register interested users */
int can_router_register(can_reg_entry_t *entry, canid_t can_id, canid_t mask, void *param)
{
    filter_el_t *filter;
    int ret;

#ifdef MODULE_CAN_MBOX
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (entry->type == CAN_TYPE_DEFAULT) {
            DEBUG("can_router_register: ifnum=%d, pid=%" PRIkernel_pid ", can_id=0x%" PRIx32
                ", mask=0x%" PRIx32 ", data=%p\n", entry->ifnum, entry->target.pid, can_id, mask, param);
        } else if (entry->type == CAN_TYPE_MBOX) {
            DEBUG("can_router_register: ifnum=%d, mbox=%p, can_id=0x%" PRIx32
                ", mask=0x%" PRIx32 ", data=%p\n", entry->ifnum, (void *)entry->target.mbox, can_id, mask, param);
        }
    }
#endif

    mutex_lock(&lock);
    ret = _filter_is_used(entry->ifnum, can_id, mask);

    filter = _alloc_filter_el(can_id, mask, param);
    if (!filter) {
        mutex_unlock(&lock);
        return -ENOMEM;
    }

#ifdef MODULE_CAN_MBOX
    filter->entry.type = entry->type;
    switch (entry->type) {
    case CAN_TYPE_DEFAULT:
        filter->entry.target.pid = entry->target.pid;
        break;
    case CAN_TYPE_MBOX:
        filter->entry.target.mbox = entry->target.mbox;
        break;
    }

#else
    filter->entry.target.pid = entry->target.pid;
#endif
    filter->entry.ifnum = entry->ifnum;
    _insert_to_list(&table[entry->ifnum], filter);
    mutex_unlock(&lock);

    PRINT_FILTERS();

    return ret;
}

/* unregister interested users */
int can_router_unregister(can_reg_entry_t *entry, canid_t can_id,
                          canid_t mask, void *param)
{
    filter_el_t *el;
    int ret;

#ifdef MODULE_CAN_MBOX
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (entry->type == CAN_TYPE_DEFAULT) {
            DEBUG("can_router_unregister: ifnum=%d, pid=%" PRIkernel_pid ", can_id=0x%" PRIx32
                ", mask=0x%" PRIx32 ", data=%p", entry->ifnum, entry->target.pid, can_id, mask, param);
        } else if (entry->type == CAN_TYPE_MBOX) {
            DEBUG("can_router_unregister: ifnum=%d, mbox=%p, can_id=0x%" PRIx32
                ", mask=0x%" PRIx32 ", data=%p\n", entry->ifnum, (void *)entry->target.mbox, can_id, mask, param);
        }
    }
#endif

    mutex_lock(&lock);
    el = _find_filter_el(table[entry->ifnum], entry, can_id, mask, param);
    if (!el) {
        mutex_unlock(&lock);
        return -EINVAL;
    }
    LL_DELETE(table[entry->ifnum], &el->entry);
    _free_filter_el(el);
    ret = _filter_is_used(entry->ifnum, can_id, mask);
    mutex_unlock(&lock);

    PRINT_FILTERS();

    return ret;
}

static int _send_msg(msg_t *msg, can_reg_entry_t *entry)
{
#ifdef MODULE_CAN_MBOX
    switch (entry->type) {
    case CAN_TYPE_DEFAULT:
        return msg_try_send(msg, entry->target.pid);
    case CAN_TYPE_MBOX:
        DEBUG("_send_msg: sending msg=%p to mbox=%p\n", (void *)msg, (void *)entry->target.mbox);
        return mbox_try_put(entry->target.mbox, msg);
    default:
        return -ENOTSUP;
    }
#else
    return msg_try_send(msg, entry->target.pid);
#endif
}

/* send received pkt to all interested users */
int can_router_dispatch_rx_indic(can_pkt_t *pkt)
{
    if (!pkt) {
        DEBUG("can_router_dispatch_rx_indic: invalid pkt\n");
        return -EINVAL;
    }

    int res = 0;
    msg_t msg;
    msg.type = CAN_MSG_RX_INDICATION;
    int msg_cnt = 0;

    DEBUG("can_router_dispatch_rx_indic: pkt=%p, ifnum=%d, can_id=%" PRIx32 "\n",
          (void *)pkt, pkt->entry.ifnum, pkt->frame.can_id);

    mutex_lock(&lock);
    can_reg_entry_t *entry = NULL;
    filter_el_t *el;
    LL_FOREACH(table[pkt->entry.ifnum], entry) {
        el = container_of(entry, filter_el_t, entry);
        if ((pkt->frame.can_id & el->mask) == el->can_id) {
            DEBUG("can_router_dispatch_rx_indic: found el=%p, data=%p\n",
                  (void *)el, (void *)el->data);
            DEBUG("can_router_dispatch_rx_indic: rx_ind to pid: %"
                  PRIkernel_pid "\n", entry->target.pid);
            atomic_fetch_add(&pkt->ref_count, 1);
            msg.content.ptr = can_pkt_alloc_rx_data(&pkt->frame, sizeof(pkt->frame), el->data);

            if (IS_ACTIVE(ENABLE_DEBUG)) {
                msg_cnt++;
            }

            if (!msg.content.ptr || (_send_msg(&msg, entry) <= 0)) {
                can_pkt_free_rx_data(msg.content.ptr);
                atomic_fetch_sub(&pkt->ref_count, 1);
                DEBUG("can_router_dispatch_rx_indic: failed to send msg to "
                      "pid=%" PRIkernel_pid "\n", entry->target.pid);
                res = -EBUSY;
                break;
            }
        }
    }
    mutex_unlock(&lock);

    DEBUG("can_router_dispatch_rx: msg send to %d threads\n", msg_cnt);

    if (atomic_load(&pkt->ref_count) == 0) {
        can_pkt_free(pkt);
    }

    return res;
}

int can_router_dispatch_tx_conf(can_pkt_t *pkt)
{
    msg_t msg;
    msg.type = CAN_MSG_TX_CONFIRMATION;
    msg.content.value = pkt->handle;

    DEBUG("can_router_dispatch_tx_conf: frame=%p, pid=%" PRIkernel_pid "\n",
          (void *)&pkt->frame, pkt->entry.target.pid);

    if (_send_msg(&msg, &pkt->entry) <= 0) {
        return -1;
    }

    return 0;
}

int can_router_dispatch_tx_error(can_pkt_t *pkt)
{
    msg_t msg;
    msg.type = CAN_MSG_TX_ERROR;
    msg.content.value = pkt->handle;

    DEBUG("can_router_dispatch_tx_error: frame=%p, pid=%" PRIkernel_pid "\n",
          (void *)&pkt->frame, pkt->entry.target.pid);

    if (_send_msg(&msg, &pkt->entry) <= 0) {
        return -1;
    }

    return 0;
}

int can_router_free_frame(struct can_frame *frame)
{
    can_pkt_t *pkt = container_of(frame, can_pkt_t, frame);

    DEBUG("can_router_free_frame: pkt=%p\n", (void*) pkt);

    if (!pkt || (atomic_load(&pkt->ref_count) <= 0)) {
        return -1;
    }

    atomic_fetch_sub(&pkt->ref_count, 1);

    if (atomic_load(&pkt->ref_count) == 0) {
        can_pkt_free(pkt);
    }
    return 0;
}
