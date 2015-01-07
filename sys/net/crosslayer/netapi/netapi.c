/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    netapi.c
 * @brief   You do not necessarily need this file (leave `netapi` out of
 *          `USEMODULE`, the netapi.h file will still be available for
 *          inclusion). It supplies you however with some helper functions
 *          for the thread communication.
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"

#include "netapi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int netapi_send_command(kernel_pid_t pid, netapi_cmd_t *cmd)
{
    msg_t msg_cmd, msg_ack;
    netapi_ack_t ack;
    int ack_result;

    DEBUG("send command to %" PRIkernel_pid ", cmd = %p, cmd->type = %d\n",
          pid, (void *)cmd, (void *)(cmd->type));

    msg_cmd.type = NETAPI_MSG_TYPE;
    msg_cmd.content.ptr = (char *)cmd;

    msg_ack.content.ptr = NULL; /* silence valgrind */

    cmd->ack = &ack;

    msg_send_receive(&msg_cmd, &msg_ack, pid);

    if (msg_ack.content.ptr != (char *)(&ack) ||
        msg_ack.type != NETAPI_MSG_TYPE ||
        ack.type != NETAPI_CMD_ACK ||
        ack.orig != cmd->type) {
        DEBUG("msg_ack.content.ptr = %p and &ack = %p must be equal.\n",
              msg_ack.content.ptr, &ack);

        return -ENOMSG;
    }

    ack_result = ack.result;

    return ack_result;
}

int netapi_send_packet(kernel_pid_t pid, netdev_hlist_t *upper_layer_hdrs,
                       void *addr, size_t addr_len, void *data, size_t data_len)
{
    netapi_snd_pkt_t pkt;

    pkt.type = NETAPI_CMD_SND;

    pkt.ulh = upper_layer_hdrs;
    pkt.dest = addr;
    pkt.dest_len = addr_len;
    pkt.data = data;
    pkt.data_len = data_len;

    return netapi_send_command(pid, (netapi_cmd_t *)(&pkt));
}

static unsigned int _get_set_option(kernel_pid_t pid, netapi_cmd_type_t type,
                                    netapi_conf_type_t param, void *data,
                                    size_t data_len)
{
    netapi_conf_t conf;

    conf.type = type;
    conf.param = param;
    conf.data = data;
    conf.data_len = data_len;

    return netapi_send_command(pid, (netapi_cmd_t *)(&conf));
}

int netapi_get_option(kernel_pid_t pid, netapi_conf_type_t param,
                      void *data, size_t data_len)
{
    return _get_set_option(pid, NETAPI_CMD_GET, param, data, data_len);
}

int netapi_set_option(kernel_pid_t pid, netapi_conf_type_t param,
                      void *data, size_t data_len)
{
    return _get_set_option(pid, NETAPI_CMD_SET, param, data, data_len);
}

static int _netapi_register_cmd(kernel_pid_t pid, netapi_cmd_type_t cmd,
                                kernel_pid_t reg_pid, netapi_reg_demux_ctx_t demux_ctx)
{
    netapi_reg_t reg;

    reg.type = cmd;
    reg.reg_pid = reg_pid;
    reg.demux_ctx = demux_ctx;

    return netapi_send_command(pid, (netapi_cmd_t *)(&reg));
}

int netapi_register(kernel_pid_t pid, kernel_pid_t reg_pid,
                    netapi_reg_demux_ctx_t demux_ctx)
{
    return _netapi_register_cmd(pid, NETAPI_CMD_REG, reg_pid, demux_ctx);
}

int netapi_unregister(kernel_pid_t pid, kernel_pid_t reg_pid)
{
    return _netapi_register_cmd(pid, NETAPI_CMD_UNREG, reg_pid, 0);
}

#ifdef MODULE_NETDEV_DUMMY
int netapi_fire_receive_event(kernel_pid_t pid, void *src, size_t src_len,
                              void *dest, size_t dest_len, void *data,
                              size_t data_len)
{
    netapi_rcv_pkt_t pkt;

    pkt.type = NETAPI_CMD_FIRE_RCV;

    pkt.src = src;
    pkt.src_len = src_len;
    pkt.dest = dest;
    pkt.dest_len = dest_len;
    pkt.data = data;
    pkt.data_len = data_len;

    return netapi_send_command(pid, (netapi_cmd_t *)(&pkt));
}
#endif
/**
 * @}
 */
