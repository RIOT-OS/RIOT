/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_netapi   Generic network module interface
 * @ingroup     net
 * @brief       Generic interface for IPC communication between network modules
 *
 * @details     The idea of this interface is that it provides every network
 *              module with a basic set of commands to communicate with its
 *              neighboring modules. In this model every module runs in its own
 *              thread and communication is done using the @ref net_ng_netapi.
 *
 * @{
 *
 * @file
 * @brief       Generic interface to communicate with network modules
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_NETAPI_H_
#define NG_NETAPI_H_

#include "kernel.h"
#include "thread.h"
#include "net/ng_netconf.h"
#include "net/ng_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for passing data up the network stack
 */
#define NG_NETAPI_MSG_TYPE_RCV          (0x0201)

/**
 * @brief   Message type for passing data down the network stack
 */
#define NG_NETAPI_MSG_TYPE_SND          (0x0202)

/**
 * @brief   Message type for setting options of network modules
 */
#define NG_NETAPI_MSG_TYPE_SET          (0x0203)

/**
 * @brief   Message type for getting options from network modules
 */
#define NG_NETAPI_MSG_TYPE_GET          (0x0204)

/**
 * @brief   Message type for replying to get and set option messages
 */
#define NG_NETAPI_MSG_TYPE_ACK          (0x0205)

/**
 * @brief   Data structure to be send for setting and getting options
 */
typedef struct {
    ng_netconf_opt_t opt;       /**< the option to get/set */
    uint16_t context;           /**< (optional) context for that option */
    void *data;                 /**< data to set or buffer to read into */
    uint16_t data_len;          /**< size of the data / the buffer */
} ng_netapi_opt_t;

/**
 * @brief   Shortcut function for sending @ref NG_NETAPI_MSG_TYPE_SND messages
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 *
 * @return              1 if packet was successfully delivered
 * @return              -1 on error (invalid PID or no space in queue)
 */
int ng_netapi_send(kernel_pid_t pid, ng_pktsnip_t *pkt);

/**
 * @brief   Shortcut function for sending @ref NG_NETAPI_MSG_TYPE_GET messages and
 *          parsing the returned @ref NG_NETAPI_MSG_TYPE_ACK message
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] opt       option to get
 * @param[in] context   (optional) context to the given option
 * @param[in] data      pointer to buffer for reading the option's value
 * @param[in] data_len  size of the given buffer
 *
 * @return              value returned by the @ref NG_NETAPI_MSG_TYPE_ACK message
 */
int ng_netapi_get(kernel_pid_t pid, ng_netconf_opt_t opt, uint16_t context,
                  void *data, size_t data_len);

/**
 * @brief   Shortcut function for sending @ref NG_NETAPI_MSG_TYPE_SET messages and
 *          parsing the returned @ref NG_NETAPI_MSG_TYPE_ACK message
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] opt       option to set
 * @param[in] context   (optional) context to the given option
 * @param[in] data      data to set the given option to
 * @param[in] data_len  size of @p data
 *
 * @return              value returned by the @ref NG_NETAPI_MSG_TYPE_ACK message
 */
int ng_netapi_set(kernel_pid_t pid, ng_netconf_opt_t opt, uint16_t context,
                  void *data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* NG_NETAPI_H_ */
/**
 * @}^
 */
