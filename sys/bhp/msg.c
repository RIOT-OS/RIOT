/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_bhp
 * @{
 *
 * @file
 * @brief       Message based Bottom Half Processor implementation
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>
#include "bhp/msg.h"

void bhp_msg_init(bhp_msg_t *bhp_msg, bhp_cb_t cb, void *ctx)
{
    bhp_set_cb(&bhp_msg->bhp, cb, ctx);
    bhp_msg->pid = KERNEL_PID_UNDEF;
    bhp_msg->msg.type = BHP_MSG_BH_REQUEST;
    bhp_msg->msg.content.ptr = &bhp_msg->bhp;
}

void bhp_msg_isr_cb(void *bhp_msg_ctx)
{
    bhp_msg_t *bhp_msg = bhp_msg_ctx;

    assert(bhp_msg->pid != KERNEL_PID_UNDEF);
    msg_send(&bhp_msg->msg, bhp_msg->pid);
}
