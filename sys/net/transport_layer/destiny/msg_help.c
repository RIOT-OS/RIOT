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
    //  blip_t recv_m;
    //  recv_m.type = TCP_NOT_DEFINED;
    //  while (recv_m.type != TCP_CONTINUE)
    //      {
    //      net_msg_receive(&recv_m);
    //      }
}

int net_msg_receive(blip_t *m)
{
    return blip_receive(m);
}

int net_msg_reply(blip_t *m, blip_t *reply, uint16_t message)
{
    reply->type = message;
    return blip_reply(m, reply);
}

int net_msg_send(blip_t *m, kernel_pid_t pid, bool block, uint16_t message)
{
    m->type = message;
    return blip_send(m, pid, block);
}

int net_msg_send_recv(blip_t *m, blip_t *reply, kernel_pid_t pid, uint16_t message)
{
    m->type = message;
    return blip_send_receive(m, reply, pid);;
}

/**
 * @}
 */
