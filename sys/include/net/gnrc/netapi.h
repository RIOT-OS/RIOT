/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netapi   GNRC communication interface
 * @ingroup     net_gnrc
 * @brief       Generic interface for IPC communication between GNRC modules
 *
 * @details     The idea of this interface is that it provides every network
 *              module with a basic set of commands to communicate with its
 *              neighboring modules. In this model every module runs in its own
 *              thread and communication is done using the @ref net_gnrc_netapi.
 *
 * @{
 *
 * @file
 * @brief       Generic interface to communicate with GNRC modules
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @defgroup    net_gnrc_netapi_mbox   Mailbox IPC extension
 * @ingroup     net_gnrc_netapi
 * @brief       @ref core_mbox "Mailbox IPC" extension for @ref net_gnrc_netapi
 * @{
 *
 * @details The submodule `gnrc_netapi_mbox` provides an extension for
 *          @ref core_mbox "Mailbox IPC".
 *
 * To use, add the module `gnrc_netapi_mbox` to the `USEMODULE` macro in your
 * application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEMODULE += gnrc_netapi_mbox
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @}
 *
 * @defgroup    net_gnrc_netapi_callbacks   Callback extension
 * @ingroup     net_gnrc_netapi
 * @brief       Callback extension for @ref net_gnrc_netapi
 * @{
 * @details The submodule `gnrc_netapi_callbacks` provides an extension for
 *          callbacks to run GNRC thread-less.
 *
 * To use, add the module `gnrc_netapi_callbacks` to the `USEMODULE` macro in
 * your application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEMODULE += gnrc_netapi_callbacks
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @}
 */

#ifndef NET_GNRC_NETAPI_H
#define NET_GNRC_NETAPI_H

#include "thread.h"
#include "net/netopt.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   @ref core_msg type for passing a @ref net_gnrc_pkt up the network stack
 */
#define GNRC_NETAPI_MSG_TYPE_RCV        (0x0201)

/**
 * @brief   @ref core_msg type for passing a @ref net_gnrc_pkt down the network stack
 */
#define GNRC_NETAPI_MSG_TYPE_SND        (0x0202)

/**
 * @brief   @ref core_msg type for setting options of network modules
 */
#define GNRC_NETAPI_MSG_TYPE_SET        (0x0203)

/**
 * @brief   @ref core_msg type for getting options from network modules
 */
#define GNRC_NETAPI_MSG_TYPE_GET        (0x0204)

/**
 * @brief   @ref core_msg type for replying to get and set option messages
 */
#define GNRC_NETAPI_MSG_TYPE_ACK        (0x0205)

/**
 * @brief   Data structure to be send for setting (@ref GNRC_NETAPI_MSG_TYPE_SET)
 *          and getting (@ref GNRC_NETAPI_MSG_TYPE_GET) options
 */
typedef struct {
    netopt_t opt;               /**< the option to get/set */
    uint16_t context;           /**< (optional) context for that option */
    void *data;                 /**< data to set or buffer to read into */
    uint16_t data_len;          /**< size of the data / the buffer */
} gnrc_netapi_opt_t;

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_SND or
 *          @ref GNRC_NETAPI_MSG_TYPE_RCV messages
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 * @param[in] type      type of the message to send. Must be either
 *                      @ref GNRC_NETAPI_MSG_TYPE_SND or
 *                      @ref GNRC_NETAPI_MSG_TYPE_RCV
 *
 * @return              1 if packet was successfully delivered
 * @return              -1 on error (invalid PID or no space in queue)
 */
int _gnrc_netapi_send_recv(kernel_pid_t pid, gnrc_pktsnip_t *pkt, uint16_t type);

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_GET or
 *          @ref GNRC_NETAPI_MSG_TYPE_SET messages and parsing the returned
 *          @ref GNRC_NETAPI_MSG_TYPE_ACK message
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] opt       option to get
 * @param[in] context   (optional) context to the given option
 * @param[in] data      pointer to buffer for reading the option's value
 * @param[in] data_len  (maximum) number of bytes in @p data
 * @param[in] type      type of the message to send. Must be either
 *                      @ref GNRC_NETAPI_MSG_TYPE_GET or
 *                      @ref GNRC_NETAPI_MSG_TYPE_SET
 *
 * @return              value returned by the @ref GNRC_NETAPI_MSG_TYPE_ACK message i.e. the actual
 *                      length of the resulting data on success, a negative errno on error. The
 *                      actual error value is for the implementation to decide but should be
 *                      sensible to indicate what went wrong.
 */
int _gnrc_netapi_get_set(kernel_pid_t pid, netopt_t opt, uint16_t context,
                         void *data, size_t data_len, uint16_t type);

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_SND messages
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 *
 * @return              1 if packet was successfully delivered
 * @return              -1 on error (invalid PID or no space in queue)
 */
static inline int gnrc_netapi_send(kernel_pid_t pid, gnrc_pktsnip_t *pkt)
{
    return _gnrc_netapi_send_recv(pid, pkt, GNRC_NETAPI_MSG_TYPE_SND);
}

/**
 * @brief   Sends @p cmd to all subscribers to (@p type, @p demux_ctx).
 *
 * @param[in] type      protocol type of the targeted network module.
 * @param[in] demux_ctx demultiplexing context for @p type.
 * @param[in] cmd       command for all subscribers
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 *
 * @return Number of subscribers to (@p type, @p demux_ctx).
 */
int gnrc_netapi_dispatch(gnrc_nettype_t type, uint32_t demux_ctx, uint16_t cmd,
                         gnrc_pktsnip_t *pkt);

/**
 * @brief   Sends a @ref GNRC_NETAPI_MSG_TYPE_SND command to all subscribers to
 *          (@p type, @p demux_ctx).
 *
 * @param[in] type      protocol type of the targeted network module.
 * @param[in] demux_ctx demultiplexing context for @p type.
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 *
 * @return Number of subscribers to (@p type, @p demux_ctx).
 */
static inline int gnrc_netapi_dispatch_send(gnrc_nettype_t type, uint32_t demux_ctx,
                                            gnrc_pktsnip_t *pkt)
{
    return gnrc_netapi_dispatch(type, demux_ctx, GNRC_NETAPI_MSG_TYPE_SND, pkt);
}

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_RCV messages
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] pkt       pointer into the packet buffer holding the received data
 *
 * @return              1 if packet was successfully delivered
 * @return              -1 on error (invalid PID or no space in queue)
 */
static inline int gnrc_netapi_receive(kernel_pid_t pid, gnrc_pktsnip_t *pkt)
{
    return _gnrc_netapi_send_recv(pid, pkt, GNRC_NETAPI_MSG_TYPE_RCV);
}

/**
 * @brief   Sends a @ref GNRC_NETAPI_MSG_TYPE_RCV command to all subscribers to
 *          (@p type, @p demux_ctx).
 *
 * @param[in] type      protocol type of the targeted network module.
 * @param[in] demux_ctx demultiplexing context for @p type.
 * @param[in] pkt       pointer into the packet buffer holding the data to send
 *
 * @return Number of subscribers to (@p type, @p demux_ctx).
 */
static inline int gnrc_netapi_dispatch_receive(gnrc_nettype_t type, uint32_t demux_ctx,
                                               gnrc_pktsnip_t *pkt)
{
    return gnrc_netapi_dispatch(type, demux_ctx, GNRC_NETAPI_MSG_TYPE_RCV, pkt);
}

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_GET messages and
 *          parsing the returned @ref GNRC_NETAPI_MSG_TYPE_ACK message
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] opt       option to get
 * @param[in] context   (optional) context to the given option
 * @param[in] data      pointer to buffer for reading the option's value
 * @param[in] max_len   maximum number of bytes that fit into @p data
 *
 * @return              value returned by the @ref GNRC_NETAPI_MSG_TYPE_ACK message i.e. the actual
 *                      length of the resulting data on success, a negative errno on error. The
 *                      actual error value is for the implementation to decide but should be
 *                      sensible to indicate what went wrong.
 */
static inline int gnrc_netapi_get(kernel_pid_t pid, netopt_t opt,
                                  uint16_t context, void *data, size_t max_len)
{
    return _gnrc_netapi_get_set(pid, opt, context, data, max_len,
                                GNRC_NETAPI_MSG_TYPE_GET);
}

/**
 * @brief   Shortcut function for sending @ref GNRC_NETAPI_MSG_TYPE_SET messages and
 *          parsing the returned @ref GNRC_NETAPI_MSG_TYPE_ACK message
 *
 * @param[in] pid       PID of the targeted network module
 * @param[in] opt       option to set
 * @param[in] context   (optional) context to the given option
 * @param[in] data      data to set the given option to
 * @param[in] data_len  size of @p data
 *
 * @return              value returned by the @ref GNRC_NETAPI_MSG_TYPE_ACK message i.e. 0 on
 *                      success, a negative errno on error. The actual error value is for the
 *                      implementation to decide but should be sensible to indicate what went
 *                      wrong.
 */
static inline int gnrc_netapi_set(kernel_pid_t pid, netopt_t opt,
                                  uint16_t context, const void *data,
                                  size_t data_len)
{
    /* disregard const pointer. This *should* be safe and any modification
     * to `data` should be considered a bug */
    return _gnrc_netapi_get_set(pid, opt, context, (void *)data, data_len,
                                GNRC_NETAPI_MSG_TYPE_SET);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETAPI_H */
/**
 * @}^
 */
