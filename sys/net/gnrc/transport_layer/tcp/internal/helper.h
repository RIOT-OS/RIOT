/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief      Helperfunctions and defines
 *
 * @author     Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef GNRC_TCP_INTERNAL_HELPER_H_
#define GNRC_TCP_INTERNAL_HELPER_H_

#include "net/tcp.h"
#include "net/gnrc/netapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bitmasks for control bit handling
 * @{
 */
#define MSK_FIN         0x0001
#define MSK_SYN         0x0002
#define MSK_RST         0x0004
#define MSK_PSH         0x0008
#define MSK_ACK         0x0010
#define MSK_URG         0x0020
#define MSK_FIN_ACK     0x0011
#define MSK_SYN_ACK     0x0012
#define MSK_RST_ACK     0x0014
#define MSK_SYN_FIN_ACK 0x0013
#define MSK_FIN_ACK_PSH 0x0019
#define MSK_CTL         0x003F
#define MSK_OFFSET      0xF000
/** @} */

/**
 * @brief Type field values for TCP internal Message Passing.
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
 * @brief Overflow tolerant comparision operators for sequence and
          acknowledgement number comparision
 * @{
 */
#define LSS_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <  0)
#define LEQ_32_BIT(x, y) (((int32_t) (x)) - ((int32_t) (y)) <= 0)
#define GRT_32_BIT(x, y) (!LEQ_32_BIT(x, y))
#define GEQ_32_BIT(x, y) (!LSS_32_BOT(x, y))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_HELPER_H_ */
/** @} */
