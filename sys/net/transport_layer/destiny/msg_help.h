/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    msg_help.h
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */
#ifndef MSG_HELP_H_
#define MSG_HELP_H_

#include <stdint.h>

// Function IDs
#define FID_SIXLOWIP_TCP                0
#define FID_SIXLOWIP_UDP                1
#define FID_TCP_PH                      2
#define FID_UDP_PH                      3
#define FID_H_PAYLOAD                   4
#define FID_SOCKET_RECV                 5
#define FID_SOCKET_RECV_FROM            6
#define FID_TCP_SYN_ACK                 7
#define FID_SOCKET_CONNECT              8
#define FID_SOCKET_HANDLE_NEW_TCP_CON   9
#define FID_TCP_ACK                     10
#define FID_SOCKET_CLOSE                11
#define FID_TCP_CHT                     12
#define FID_TCP_SHELL                   13

#define RETURNNOW                       4000

void block_continue_thread(void);
int net_msg_receive(msg_t *m);
int net_msg_reply(msg_t *m, msg_t *reply, uint16_t message);
int net_msg_send(msg_t *m, kernel_pid_t pid, bool block, uint16_t message);
int net_msg_send_recv(msg_t *m, msg_t *reply, kernel_pid_t pid, uint16_t message);

#endif /* MSG_HELP_H_ */
/**
 * @}
 */
