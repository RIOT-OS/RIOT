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

#ifndef GNRC_TCP_COMMON_H
#define GNRC_TCP_COMMON_H

#include <stdint.h>
#include "thread.h"
#include "mutex.h"
#include "evtimer.h"
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
#define STATUS_LISTENING      (1 << 0) /**< Internal: Status bitmask LISTENING */
#define STATUS_ALLOW_ANY_ADDR (1 << 1) /**< Internal: Status bitmask ALLOW_ANY_ADDR */
#define STATUS_NOTIFY_USER    (1 << 2) /**< Internal: Status bitmask NOTIFY_USER */
#define STATUS_ACCEPTED       (1 << 3) /**< Internal: Status bitmask ACCEPTED */
#define STATUS_LOCKED         (1 << 4) /**< Internal: Status bitmask LOCKED */
/** @} */

/**
 * @brief Defines for "eventloop" thread settings.
 * @{
 */
#define TCP_EVENTLOOP_PRIO       (THREAD_PRIORITY_MAIN - 2U) /**< Internal: Handler priority */
#define TCP_EVENTLOOP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)  /**< Internal: Handler stack size */
/** @} */

/**
 * @brief Bitmasks for control bit field handling.
 * @{
 */
#define MSK_FIN         (0x0001) /**< Internal: Packet bitmask FIN */
#define MSK_SYN         (0x0002) /**< Internal: Packet bitmask SYN */
#define MSK_RST         (0x0004) /**< Internal: Packet bitmask RST */
#define MSK_PSH         (0x0008) /**< Internal: Packet bitmask PSH */
#define MSK_ACK         (0x0010) /**< Internal: Packet bitmask ACK */
#define MSK_URG         (0x0020) /**< Internal: Packet bitmask URG */
#define MSK_FIN_ACK     (0x0011) /**< Internal: Packet bitmask FIN & ACK */
#define MSK_SYN_ACK     (0x0012) /**< Internal: Packet bitmask SYN & ACK */
#define MSK_RST_ACK     (0x0014) /**< Internal: Packet bitmask RST & ACK */
#define MSK_SYN_FIN_ACK (0x0013) /**< Internal: Packet bitmask SYN & FIN & ACK */
#define MSK_FIN_ACK_PSH (0x0019) /**< Internal: Packet bitmask FIN & ACK & PSH */
#define MSK_CTL         (0x003F) /**< Internal: Packet bitmask TCP status control bits */
#define MSK_OFFSET      (0xF000) /**< Internal: Packet bitmask TCP status option offset */
/** @} */

/**
 * @brief Message types for GNRC TCPs internal message passing.
 * @{
 */
#define MSG_TYPE_CONNECTION_TIMEOUT (GNRC_NETAPI_MSG_TYPE_ACK + 101) /**< Internal: message id */
#define MSG_TYPE_PROBE_TIMEOUT      (GNRC_NETAPI_MSG_TYPE_ACK + 102) /**< Internal: message id */
#define MSG_TYPE_USER_SPEC_TIMEOUT  (GNRC_NETAPI_MSG_TYPE_ACK + 103) /**< Internal: message id */
#define MSG_TYPE_RETRANSMISSION     (GNRC_NETAPI_MSG_TYPE_ACK + 104) /**< Internal: message id */
#define MSG_TYPE_TIMEWAIT           (GNRC_NETAPI_MSG_TYPE_ACK + 105) /**< Internal: message id */
#define MSG_TYPE_NOTIFY_USER        (GNRC_NETAPI_MSG_TYPE_ACK + 106) /**< Internal: message id */
/** @} */

/**
 * @brief Define for marking that time measurement is uninitialized.
 */
#define RTO_UNINITIALIZED (-1) /**< Internal: Constant RTO uninitialized */

/**
 * @brief Overflow tolerant comparison operators for sequence and
          acknowledgement number comparison.
 * @{
 */
#define LSS_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <  0) /**< Internal: operator < */
#define LEQ_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <= 0) /**< Internal: operator <= */
#define GRT_32_BIT(x, y) (!LEQ_32_BIT(x, y)) /**< Internal: operator > */
#define GEQ_32_BIT(x, y) (!LSS_32_BOT(x, y)) /**< Internal: operator >= */
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
 * @brief Helper macro used to create debug message on function entry.
 *
 * @note Compilation units using TCP_DEBUG_ENTER must set ENABLE_DEBUG and include debug.h.
 */
#define TCP_DEBUG_ENTER DEBUG("GNRC_TCP: Enter \"%s\", File: %s(%d)\n", \
                              DEBUG_FUNC, RIOT_FILE_RELATIVE, __LINE__)

/**
 * @brief Helper macro used to create debug message on function exit.
 *
 * @note Compilation units using TCP_DEBUG_LEAVE must set ENABLE_DEBUG and include debug.h.
 */
#define TCP_DEBUG_LEAVE DEBUG("GNRC_TCP: Leave \"%s\", File: %s(%d)\n", \
                              DEBUG_FUNC, RIOT_FILE_RELATIVE, __LINE__)

/**
 * @brief Helper macro used to create debug message on error.
 *
 * @note Compilation units using TCP_DEBUG_ERROR must set ENABLE_DEBUG and include debug.h.
 */
#define TCP_DEBUG_ERROR(msg) DEBUG("GNRC_TCP: Error: \"%s\", Func: %s, File: %s(%d)\n", \
                                   msg, DEBUG_FUNC, RIOT_FILE_RELATIVE, __LINE__)

/**
 * @brief Helper macro used to create informational debug message.
 *
 * @note Compilation units using TCP_DEBUG_INFO must set ENABLE_DEBUG and include debug.h.
 */
#define TCP_DEBUG_INFO(msg) DEBUG("GNRC_TCP: Info: \"%s\", Func: %s, File: %s(%d)\n", \
                                  msg, DEBUG_FUNC, RIOT_FILE_RELATIVE, __LINE__)

/**
 * @brief TCB list type.
 */
typedef struct {
    gnrc_tcp_tcb_t *head; /**< Head of TCB list */
    mutex_t lock;         /**< Lock of TCB list */
} _gnrc_tcp_common_tcb_list_t;

/**
 * @brief Function to access to TCB list
 *
 * @returns Pointer to global TCB list.
 */
_gnrc_tcp_common_tcb_list_t *_gnrc_tcp_common_get_tcb_list(void);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_COMMON_H */
/** @} */
