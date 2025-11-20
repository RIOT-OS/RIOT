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
 * @brief       CAN Data Link Layer module
 *
 * This module contains the DLL interfaces for upper layer (raw_can_*)
 * and devices (can_dll_*).
 * It manages the connection between an device number and its candev thread.
 *
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "thread.h"
#include "can/dll.h"
#include "can/raw.h"
#include "can/device.h"
#include "can/pkt.h"
#include "can/common.h"
#include "can/router.h"
#include "utlist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static candev_dev_t *candev_list[CAN_DLL_NUMOF];
static int candev_nb = 0;
static can_reg_entry_t *tx_list[CAN_DLL_NUMOF];
static mutex_t tx_lock = MUTEX_INIT;

static int _get_ifnum(kernel_pid_t pid)
{
    for (int i = 0; i < candev_nb; i++) {
        if (candev_list[i]->pid == pid) {
            return i;
        }
    }

    return -ENODEV;
}

int _send_pkt(can_pkt_t *pkt)
{
    assert(pkt);

    msg_t msg;
    int handle = pkt->handle;

    mutex_lock(&tx_lock);
    LL_APPEND(tx_list[pkt->entry.ifnum], &pkt->entry);
    mutex_unlock(&tx_lock);

    msg.type = CAN_MSG_SEND_FRAME;
    msg.content.ptr = (void*) pkt;

    if (msg_send(&msg, candev_list[pkt->entry.ifnum]->pid) <= 0) {
        mutex_lock(&tx_lock);
        LL_DELETE(tx_list[pkt->entry.ifnum], &pkt->entry);
        mutex_unlock(&tx_lock);
        return -EOVERFLOW;
    }

    return handle;
}

int raw_can_send(int ifnum, const can_frame_t *frame, kernel_pid_t pid)
{
    can_pkt_t *pkt;

    assert(frame);
    assert(ifnum < candev_nb);

    pkt = can_pkt_alloc_tx(ifnum, frame, pid);
    if (!pkt) {
        return -ENOMEM;
    }

    DEBUG("raw_can_send: ifnum=%d, id=0x%" PRIx32 " from pid=%" PRIkernel_pid ", handle=%d\n",
          ifnum, frame->can_id, pid, pkt->handle);

    int ret = _send_pkt(pkt);
    if (ret < 0) {
        can_pkt_free(pkt);
    }
    return ret;
}

#ifdef MODULE_CAN_MBOX
int raw_can_send_mbox(int ifnum, const can_frame_t *frame, mbox_t *mbox)
{
    can_pkt_t *pkt;

    assert(frame);
    assert(ifnum < candev_nb);

    pkt = can_pkt_alloc_mbox_tx(ifnum, frame, mbox);
    if (!pkt) {
        return -ENOMEM;
    }

    DEBUG("raw_can_send: ifnum=%d, id=0x%" PRIx32 ", handle=%d\n", ifnum, frame->can_id, pkt->handle);

    int ret = _send_pkt(pkt);
    if (ret < 0) {
        can_pkt_free(pkt);
    }
    return ret;
}
#endif

int raw_can_abort(int ifnum, int handle)
{
    msg_t msg, reply;
    can_pkt_t *pkt = NULL;
    can_reg_entry_t *entry = NULL;
    can_reg_entry_t *prev = tx_list[ifnum];

    assert(ifnum < candev_nb);

    DEBUG("raw_can_abort: ifnum=%u, handle=%d\n", ifnum, handle);

    mutex_lock(&tx_lock);
    LL_FOREACH(tx_list[ifnum], entry) {
        pkt = container_of(entry, can_pkt_t, entry);
        if (pkt->handle == handle) {
            if (prev == tx_list[ifnum]) {
                tx_list[ifnum] = entry->next;
            }
            else {
                prev->next = entry->next;
            }
            break;
        }
        prev = entry;
    }
    mutex_unlock(&tx_lock);

    if (pkt == NULL) {
        DEBUG("raw_can_abort: no pkt\n");
        return -ENODEV;
    }

    msg.type = CAN_MSG_ABORT_FRAME;
    msg.content.ptr = pkt;

    msg_send_receive(&msg, &reply, candev_list[ifnum]->pid);

    can_pkt_free(pkt);

    return 0;
}

static int register_filter_entry(can_reg_entry_t *entry, struct can_filter *filter, void *param)
{
    msg_t msg, reply;
    int ret;

    DEBUG("register_filter_entry: ifnum=%d, filter=0x%" PRIx32 ", mask=0x%" PRIx32 ", param=%p\n",
          entry->ifnum, filter->can_id, filter->can_mask, param);

    ret = can_router_register(entry, filter->can_id, filter->can_mask, param);
    if (ret < 0) {
        return -ENOMEM;
    }
    else if (ret == 1) {
        DEBUG("raw_can_subscribe_rx: filter=0x%" PRIx32 " already in use\n", filter->can_id);
        return 0;
    }

    msg.type = CAN_MSG_SET_FILTER;
    msg.content.ptr = filter;
    msg_send_receive(&msg, &reply, candev_list[entry->ifnum]->pid);

    if ((int) reply.content.value < 0) {
        can_router_unregister(entry, filter->can_id, filter->can_mask, param);
        return -ENOMEM;
    }

    return 0;
}

static int unregister_filter_entry(can_reg_entry_t *entry, struct can_filter *filter, void *param)
{
    msg_t msg, reply;
    int ret;

    DEBUG("unregister_filter_entry: ifnum=%d, filter=0x%" PRIx32 ", mask=0x%" PRIx32 ", param=%p\n",
          entry->ifnum, filter->can_id, filter->can_mask, param);

    ret = can_router_unregister(entry, filter->can_id, filter->can_mask, param);
    if (ret < 0) {
        return -ENOMEM;
    }
    else if (ret == 1) {
        DEBUG("raw_can_unsubscribe_rx: filter=0x%" PRIx32 " still in use\n", filter->can_id);
        return 0;
    }

    msg.type = CAN_MSG_REMOVE_FILTER;
    msg.content.ptr = filter;
    msg_send_receive(&msg, &reply, candev_list[entry->ifnum]->pid);

    if ((int) reply.content.value < 0) {
        return -ENOMEM;
    }

    return 0;
}

int raw_can_subscribe_rx(int ifnum, struct can_filter *filter, kernel_pid_t pid, void *param)
{
    assert(ifnum < candev_nb);
    assert(filter);

    can_reg_entry_t entry;
    entry.ifnum = ifnum;
    entry.target.pid = pid;
#ifdef MODULE_CAN_MBOX
    entry.type = CAN_TYPE_DEFAULT;
#endif

    return register_filter_entry(&entry, filter, param);
}

#ifdef MODULE_CAN_MBOX
int raw_can_subscribe_rx_mbox(int ifnum, struct can_filter *filter, mbox_t *mbox, void *param)
{
    assert(ifnum < candev_nb);
    assert(filter);

    can_reg_entry_t entry;
    entry.ifnum = ifnum;
    entry.target.mbox = mbox;
    entry.type = CAN_TYPE_MBOX;

    return register_filter_entry(&entry, filter, param);
}
#endif

int raw_can_unsubscribe_rx(int ifnum, struct can_filter *filter, kernel_pid_t pid, void *param)
{
    assert(ifnum < candev_nb);
    assert(filter);

    can_reg_entry_t entry;
    entry.ifnum = ifnum;
    entry.target.pid = pid;
#ifdef MODULE_CAN_MBOX
    entry.type = CAN_TYPE_DEFAULT;
#endif

    return unregister_filter_entry(&entry, filter, param);
}

#ifdef MODULE_CAN_MBOX
int raw_can_unsubscribe_rx_mbox(int ifnum, struct can_filter *filter, mbox_t *mbox, void *param)
{
    assert(ifnum < candev_nb);
    assert(filter);

    can_reg_entry_t entry;
    entry.ifnum = ifnum;
    entry.target.mbox = mbox;
    entry.type = CAN_TYPE_MBOX;

    return unregister_filter_entry(&entry, filter, param);
}
#endif

int raw_can_free_frame(can_rx_data_t *frame)
{
    if (!frame) {
        return 0;
    }
    int ret = can_router_free_frame((can_frame_t *)frame->data.iov_base);

    can_pkt_free_rx_data(frame);

    return ret;
}

int raw_can_get_can_opt(int ifnum, can_opt_t *opt)
{
    msg_t msg, reply;

    assert(ifnum < CAN_DLL_NUMOF);

    if (!opt) {
        return -ENOMEM;
    }

    opt->context = (uint16_t)candev_list[ifnum]->pid;

    msg.type = CAN_MSG_GET;
    msg.content.ptr = (void *)opt;
    if (msg_send_receive(&msg, &reply, opt->context) != 1) {
        return -EBUSY;
    }

    return (int) reply.content.value;
}

int raw_can_set_can_opt(int ifnum, can_opt_t *opt)
{
    msg_t msg, reply;

    assert(ifnum < CAN_DLL_NUMOF);

    if (!opt) {
        return -ENOMEM;
    }

    opt->context = (uint16_t)candev_list[ifnum]->pid;

    msg.type = CAN_MSG_SET;
    msg.content.ptr = (void *)opt;
    if (msg_send_receive(&msg, &reply, opt->context) != 1) {
        return -EBUSY;
    }

    return (int) reply.content.value;
}

int can_dll_register_candev(candev_dev_t *candev)
{
    if (candev_nb >= CAN_DLL_NUMOF) {
        return -ENODEV;
    }

    DEBUG("can_dll_register_candev: candev=%p, ifnum=%d, pid=%" PRIkernel_pid "\n",
          (void *)candev, candev_nb, candev->pid);

    candev_list[candev_nb] = candev;

    return candev_nb++;
}

int can_dll_dispatch_rx_frame(can_frame_t *frame, kernel_pid_t pid)
{
    can_pkt_t *pkt = can_pkt_alloc_rx(_get_ifnum(pid), frame);

    return can_router_dispatch_rx_indic(pkt);
}

static int _remove_entry_from_list(can_reg_entry_t **list, can_reg_entry_t *entry)
{
    assert(list);
    int res = -1;
    can_reg_entry_t *_tmp;
    if (*list == entry) {
        res = 0;
        *list = (*list)->next;
    }
    else if (*list != NULL) {
        _tmp = *list;
        while (_tmp->next && (_tmp->next != entry)) {
            _tmp = _tmp->next;
        }
        if (_tmp->next) {
            _tmp->next = entry->next;
            res = 0;
        }
    }
    return res;
}

int can_dll_dispatch_tx_conf(can_pkt_t *pkt)
{
    DEBUG("can_dll_dispatch_tx_conf: pkt=%p\n", (void*)pkt);

    mutex_lock(&tx_lock);
    int res = _remove_entry_from_list(&tx_list[pkt->entry.ifnum], &pkt->entry);
    mutex_unlock(&tx_lock);

    if (res == 0) {
        can_router_dispatch_tx_conf(pkt);
        can_pkt_free(pkt);
    }

    return 0;
}

int can_dll_dispatch_tx_error(can_pkt_t *pkt)
{
    DEBUG("can_dll_dispatch_tx_error: pkt=%p\n", (void*)pkt);

    mutex_lock(&tx_lock);
    int res = _remove_entry_from_list(&tx_list[pkt->entry.ifnum], &pkt->entry);
    mutex_unlock(&tx_lock);

    if (res == 0) {
        can_router_dispatch_tx_error(pkt);
        can_pkt_free(pkt);
    }

    return 0;

}

int can_dll_dispatch_bus_off(kernel_pid_t pid)
{
    int ifnum = _get_ifnum(pid);
    can_reg_entry_t *entry = tx_list[ifnum];

    DEBUG("can_dll_dispatch_bus_off: ifnum=%d, pid=%" PRIkernel_pid "\n", ifnum, pid);

    mutex_lock(&tx_lock);
    while (entry) {
        can_pkt_t *pkt = container_of(entry, can_pkt_t, entry);
        can_router_dispatch_tx_error(pkt);
        can_pkt_free(pkt);
        LL_DELETE(tx_list[ifnum], entry);
        entry = tx_list[ifnum];
    }
    mutex_unlock(&tx_lock);

    return 0;
}

int can_dll_init(void)
{
    can_pkt_init();
    can_router_init();

    return 0;
}

int raw_can_power_down(int ifnum)
{
    msg_t msg, reply;

    assert(ifnum < candev_nb);

    msg.type = CAN_MSG_POWER_DOWN;
    if (msg_send_receive(&msg, &reply, candev_list[ifnum]->pid) != 1) {
        return -EBUSY;
    }

    return (int) reply.content.value;
}

int raw_can_power_up(int ifnum)
{
    msg_t msg, reply;

    assert(ifnum < candev_nb);

    msg.type = CAN_MSG_POWER_UP;
    if (msg_send_receive(&msg, &reply, candev_list[ifnum]->pid) != 1) {
        return -EBUSY;
    }

    return (int) reply.content.value;
}

int raw_can_set_bitrate(int ifnum, uint32_t bitrate, uint32_t sample_point)
{
    if (ifnum < 0 || ifnum >= candev_nb) {
        return -1;
    }

    int res = 0;
    int ret;
    uint32_t clock;
    struct can_bittiming_const btc;
    struct can_bittiming bittiming;
    bittiming.bitrate = bitrate;
    bittiming.sample_point = sample_point;

    can_opt_t opt;
    opt.opt = CANOPT_CLOCK;
    opt.data = &clock;
    opt.data_len = sizeof(clock);
    ret = raw_can_get_can_opt(ifnum, &opt);
    if (ret < 0) {
        DEBUG("raw_can_set_bitrate: error when getting clock (%d)\n", ret);
        return -1;
    }
    DEBUG("raw_can_set_bitrate: clock=%" PRIu32 " Hz\n", clock);

    opt.opt = CANOPT_BITTIMING_CONST;
    opt.data = &btc;
    opt.data_len = sizeof(btc);
    ret = raw_can_get_can_opt(ifnum, &opt);
    if (ret < 0) {
        DEBUG("raw_can_set_bitrate: error when getting const (%d)\n", ret);
        return -1;
    }

    ret = can_device_calc_bittiming(clock, &btc, &bittiming);
    if (ret < 0) {
        DEBUG("raw_can_set_bitrate: bittiming might be wrong, ret=%d\n", ret);
        res = 1;
    }

    opt.data = &bittiming;
    opt.data_len = sizeof(bittiming);
    opt.opt = CANOPT_BITTIMING;

    ret = raw_can_set_can_opt(ifnum, &opt);
    if (ret < 0) {
        DEBUG("raw_can_set_bitrate: error when setting bitrate (%d)\n", ret);
        return -1;
    }

    DEBUG("raw_can_set_bitrate: success bitrate=%" PRIu32 ", spt=%" PRIu32 "\n",
          bittiming.bitrate, bittiming.sample_point);

    return res;
}

#ifdef MODULE_CAN_TRX
int raw_can_set_trx(int ifnum, can_trx_t *trx)
{
    msg_t msg, reply;

    assert(ifnum < candev_nb);

    msg.type = CAN_MSG_SET_TRX;
    msg.content.ptr = trx;
    if (msg_send_receive(&msg, &reply, candev_list[ifnum]->pid) != 1) {
        return -EBUSY;
    }

    return (int) reply.content.value;
}
#endif

int raw_can_get_ifnum_by_name(const char *name)
{
    for (int i = 0; i < candev_nb; i++) {
        if ((strcmp(name, candev_list[i]->name) == 0) &&
                (strlen(name) == strlen(candev_list[i]->name))) {
            return i;
        }
    }

    return RAW_CAN_DEV_UNDEF;
}

const char *raw_can_get_name_by_ifnum(int ifnum)
{
    assert(ifnum >= 0);

    if (ifnum >= candev_nb) {
        return NULL;
    }

    return candev_list[ifnum]->name;
}

candev_dev_t *raw_can_get_dev_by_ifnum(int ifnum)
{
    assert(ifnum >= 0);

    if (ifnum >= candev_nb) {
        return NULL;
    }

    return candev_list[ifnum];
}
