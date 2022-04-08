/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP CAN Driver
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdlib.h>
#include <errno.h>

#include "thread.h"
#include "luid.h"
#include "ztimer.h"
#include "timex.h"

#include "csp/csp.h"
#include "csp/csp_id.h"
#include "csp/interfaces/csp_if_can.h"
#include "csp_drivers_can.h"

#include "can/dll.h"
#include "can/can.h"
#include "can/conn/raw.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_WARNING
#endif
#include "log.h"

static int _csp_can_tx_frame(void *driver_data, uint32_t id, const uint8_t *data, uint8_t dlc)
{
    if (dlc > 8) {
        return CSP_ERR_INVAL;
    }

    struct can_frame frame = {
        .can_id = id | CAN_EFF_FLAG,
        .can_dlc = dlc
    };

    memcpy(frame.data, data, dlc);

    csp_can_iface_t *ctx = driver_data;
    conn_can_raw_t conn;

    conn_can_raw_create(&conn, NULL, 0, ctx->dev->ifnum, 0);
    int ret = conn_can_raw_send(&conn, &frame, 0);

    LOG_DEBUG("[csp_can] %s: id: %" PRIx32 " dlc: %d Data: \n", ctx->dev->name, id, dlc);
    if (ret < 0) {
        LOG_WARNING("[csp_can] %s: write() failed, errno %d\n", ctx->dev->name, ret);
        return CSP_ERR_TX;
    }
    conn_can_raw_close(&conn);

    return CSP_ERR_NONE;
}

static void *_csp_can_rx_thread(void *arg)
{
    csp_can_iface_t *ctx = (csp_can_iface_t *)arg;
    conn_can_raw_t conn;
    struct can_frame frame;

    /* setup mask to receive all messages */
    struct can_filter filters[1];

    filters[0].can_id = 0;
    filters[0].can_mask = 0;
    conn_can_raw_create(&conn, filters, 1, ctx->dev->ifnum, 0);

    LOG_INFO("[csp_can] %s: rx_thread running\n", ctx->dev->name);
    while (1) {
        int len = conn_can_raw_recv(&conn, &frame, 0);
        if (len < 0) {
            LOG_ERROR("[csp_can] %s rx: read() failed, %d\n", ctx->dev->name, len);
            continue;
        }
        if (len != sizeof(struct can_frame)) {
            LOG_WARNING(
                "[csp_can] %s rx: Read incomplete CAN frame, size: %d, expected: %d bytes\n",
                ctx->dev->name, len, sizeof(frame));
            continue;
        }

        if (len != sizeof(frame)) {
            LOG_WARNING(
                "[csp_can] %s rx: Read incomplete CAN frame, size: %d, expected: %d bytes\n",
                ctx->dev->name, len, sizeof(frame));
            continue;
        }

        /* drop frames with standard id (CSP uses extended) */
        if (!(frame.can_id & CAN_EFF_FLAG)) {
            printf("[csp_can] %s :DROP standard id\n", ctx->dev->name);
            continue;
        }

        /* drop error and remote frames */
        if (frame.can_id & (CAN_ERR_FLAG | CAN_RTR_FLAG)) {
            LOG_WARNING("[csp_can] %s rx: discarding ERR/RTR/SFF frame\n", ctx->dev->name);
            continue;
        }
        /* strip flags */
        frame.can_id &= CAN_EFF_MASK;

        /* call csp rx callback */
        int res = csp_can_rx(&ctx->iface, frame.can_id, frame.data, frame.can_dlc, NULL);
        LOG_INFO("[csp_can] %s rx: res=(%d) on %d bytes\n", ctx->dev->name, res, len);
    }

    return NULL;
}

int csp_can_iface_init(csp_can_iface_t *ctx, const csp_can_iface_params_t *params)
{
    assert(ctx);
    memset(ctx, '\0', sizeof(csp_can_iface_t));

    ctx->dev = raw_can_get_dev_by_ifnum(params->ifnum);
    if (!ctx->dev) {
        LOG_ERROR("[csp_can]: ERROR, invalid can interface %d\n", params->ifnum);
        return -ENODEV;
    }

    ctx->iface.name = ctx->dev->name;
    ctx->iface.interface_data = &ctx->ifdata;
    ctx->iface.driver_data = ctx;
    ctx->ifdata.tx_func = _csp_can_tx_frame;
    ctx->ifdata.pbufs = NULL;
    if (params->addr == CSP_CAN_IFACE_ADDR_UNDEF) {
        luid_get(&ctx->iface.addr, 2);
        ctx->iface.addr = ctx->iface.addr & csp_id_get_max_nodeid();
    }
    else {
        ctx->iface.addr = params->addr;
    }
    ctx->iface.netmask = params->netmask;

    return 0;
}

kernel_pid_t csp_can_iface_create(csp_can_iface_t *ctx, char *stack, int stacksize, char priority,
                                  const char *name)
{
    assert(ctx);

    int res = csp_can_add_interface(&ctx->iface);

    if (res < 0) {
        LOG_ERROR("[csp_can] %s: ERROR, failed to add interface: %d\n", ctx->dev->name, res);
        return -EINVAL;
    }

    /* Create receive thread */
    ctx->pid = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                             _csp_can_rx_thread, (void *)ctx, name);
    if (ctx->pid < 0) {
        LOG_ERROR("[csp_can] %s: ERROR, failed to create rx thread\n", ctx->dev->name);
    }

    return ctx->pid;
}
