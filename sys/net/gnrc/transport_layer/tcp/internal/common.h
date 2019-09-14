/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       Internally used defines, macros and variable declarations.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include "assert.h"
#include "kernel_types.h"
#include "thread.h"
#include "mutex.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Port number unspecified.
 *
 * @see https://www.ietf.org/rfc/rfc1700.txt
 */
#define PORT_UNSPEC (0)

/**
 * @brief TCB status flags
 * @{
 */
#define STATUS_PASSIVE        (1 << 0)
#define STATUS_ALLOW_ANY_ADDR (1 << 1)
#define STATUS_NOTIFY_USER    (1 << 2)
#define STATUS_WAIT_FOR_MSG   (1 << 3)
/** @} */

/**
 * @brief Defines for "eventloop" thread settings.
 * @{
 */
#define TCP_EVENTLOOP_MSG_QUEUE_SIZE (8U)
#define TCP_EVENTLOOP_PRIO           (THREAD_PRIORITY_MAIN - 2U)
#define TCP_EVENTLOOP_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
/** @} */

/**
 * @brief Bitmasks for control bit field handling.
 * @{
 */
#define MSK_FIN         (0x0001)
#define MSK_SYN         (0x0002)
#define MSK_RST         (0x0004)
#define MSK_PSH         (0x0008)
#define MSK_ACK         (0x0010)
#define MSK_URG         (0x0020)
#define MSK_FIN_ACK     (0x0011)
#define MSK_SYN_ACK     (0x0012)
#define MSK_RST_ACK     (0x0014)
#define MSK_SYN_FIN_ACK (0x0013)
#define MSK_FIN_ACK_PSH (0x0019)
#define MSK_CTL         (0x003F)
#define MSK_OFFSET      (0xF000)
/** @} */

/**
 * @brief Message types for GNRC TCPs internal message passing.
 * @{
 */
#define MSG_TYPE_CONNECTION_TIMEOUT (GNRC_NETAPI_MSG_TYPE_ACK + 101)
#define MSG_TYPE_PROBE_TIMEOUT      (GNRC_NETAPI_MSG_TYPE_ACK + 102)
#define MSG_TYPE_USER_SPEC_TIMEOUT  (GNRC_NETAPI_MSG_TYPE_ACK + 103)
#define MSG_TYPE_RETRANSMISSION     (GNRC_NETAPI_MSG_TYPE_ACK + 104)
#define MSG_TYPE_TIMEWAIT           (GNRC_NETAPI_MSG_TYPE_ACK + 105)
#define MSG_TYPE_NOTIFY_USER        (GNRC_NETAPI_MSG_TYPE_ACK + 106)
/** @} */

/**
 * @brief Define for marking that time measurement is uninitialized.
 */
#define RTO_UNINITIALIZED (-1)

/**
 * @brief Overflow tolerant comparison operators for sequence and
          acknowledgement number comparison.
 * @{
 */
#define LSS_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <  0)
#define LEQ_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <= 0)
#define GRT_32_BIT(x, y) (!LEQ_32_BIT(x, y))
#define GEQ_32_BIT(x, y) (!LSS_32_BOT(x, y))
/** @} */

/**
 * @brief Check if a given sequence number falls into receive window.
 */
#define INSIDE_WND(l_ed, seq_num, r_ed) (LEQ_32_BIT(l_ed, seq_num) && LSS_32_BIT(seq_num, r_ed))

/**
 * @brief Extract offset value from "offctl" field in TCP header.
 */
#define GET_OFFSET( x ) (((x) & MSK_OFFSET) >> 12)

/**
 * @brief PID of GNRC TCP event handling thread
 */
extern kernel_pid_t gnrc_tcp_pid;

/**
 * @brief Head of linked TCB list.
 */
extern gnrc_tcp_tcb_t *_list_tcb_head;

/**
 * @brief Mutex to protect TCB list.
 */
extern mutex_t _list_tcb_lock;

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
/** @} */
