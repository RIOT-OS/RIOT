/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    msg_help.c
 * @brief   Providing implementation for prototypes defined in msg_help.h.
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#include <stdio.h>

#include "thread.h"

#include "tcp_timer.h"

#include "msg_help.h"

void block_continue_thread(void)
{
    //  msg_pulse_t recv_m;
    //  recv_m.type = TCP_NOT_DEFINED;
    //  while (recv_m.type != TCP_CONTINUE)
    //      {
    //      net_msg_receive(&recv_m);
    //      }
}

int net_msg_receive(msg_pulse_t *m)
{
    return msg_receive_pulse(m);
}

int net_msg_reply(msg_pulse_t *m, msg_pulse_t *reply, uint16_t message)
{
    reply->type = message;
    return msg_reply_pulse(m, reply);
}

int net_msg_send(msg_pulse_t *m, kernel_pid_t pid, bool block, uint16_t message)
{
    m->type = message;
    if (block) {
        return msg_send_pulse(m, pid);
    }
    else {
        return msg_try_send_pulse(m, pid);
    }
}

int net_msg_send_recv(msg_pulse_t *m, msg_pulse_t *reply, kernel_pid_t pid, uint16_t message)
{
    m->type = message;
    return msg_send_receive_pulse(m, reply, pid);;
}

/**
 * @}
 */
