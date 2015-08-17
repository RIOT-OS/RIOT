/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_nettest  NETAPI test framework
 * @ingroup     net_gnrc_netapi
 * @brief       This provides a framework to test the @ref net_gnrc_netapi IPC
 *              calls.
 * @{
 *
 * @file
 * @brief   Definitions for the @ref net_gnrc_netapi test framework
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NETTEST_H_
#define GNRC_NETTEST_H_

#include <stdint.h>
#include <stdlib.h>

#include "kernel_types.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout for tests in microseconds
 */
#ifndef GNRC_NETTEST_TIMEOUT
#define GNRC_NETTEST_TIMEOUT        (1000)
#endif

/**
 * @brief   Default stack size to use for the nettest thread
 */
#ifndef GNRC_NETTEST_STACK_SIZE
#define GNRC_NETTEST_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the nettest thread
 */
#ifndef GNRC_NETTEST_PRIO
#define GNRC_NETTEST_PRIO           (THREAD_PRIORITY_MAIN)
#endif

/**
 * @brief   Default message queue size to use for the nettest thread.
 */
#ifndef GNRC_NETTEST_MSG_QUEUE_SIZE
#define GNRC_NETTEST_MSG_QUEUE_SIZE (8U)
#endif


/**
 * @brief   Type for get/set callbacks.
 *
 * @param[in] context   (Optional) context for the option.
 *                      Compare gnrc_netapi_opt_t::context.
 * @param[in,out] data  Data to set or buffer to read into.
 *                      Compare gnrc_netapi_opt_t::data.
 * @param[in] data_len  Size of the data / the buffer.
 *                      Compare gnrc_netapi_opt_t::data_len
 *
 * @return  The value for the @ref GNRC_NETAPI_MSG_TYPE_ACK message.
 */
typedef int (*gnrc_nettest_opt_cb_t)(uint16_t context, void *data, uint16_t data_len);

/**
 * @brief   Option callback list element.
 */
typedef struct {
    gnrc_nettest_opt_cb_t get;  /**< getter for an option */
    gnrc_nettest_opt_cb_t set;  /**< setter for an option */
} gnrc_nettest_opt_cbs_t;

/**
 * @brief   Result type for tests.
 */
typedef enum {
    GNRC_NETTEST_SUCCESS = 0,   /**< test was successful */
    GNRC_NETTEST_FAIL,          /**< test failed */
    GNRC_NETTEST_TIMED_OUT,     /**< test timed out */
    GNRC_NETTEST_WRONG_MSG,     /**< wrong message type received */
    GNRC_NETTEST_WRONG_SENDER,  /**< wrong message type received */
} gnrc_nettest_res_t;

/**
 * @brief   Registers a getter for an option.
 *
 * @details Overrides previous registrations.
 *
 * @param[in] opt   The option to register the getter for.
 * @param[in] cb    An option getter. NULL to delete.
 */
void gnrc_nettest_register_get(netopt_t opt, gnrc_nettest_opt_cb_t cb);

/**
 * @brief   Registers a setter for an option.
 *
 * @details Overrides previous registrations.
 *
 * @param[in] opt   The option to register the setter for.
 * @param[in] cb    An option setter. NULL to delete.
 */
void gnrc_nettest_register_set(netopt_t opt, gnrc_nettest_opt_cb_t cb);

/**
 * @brief   Test @ref GNRC_NETAPI_MSG_TYPE_SND command to @p pid.
 *
 * @details This registered the nettest thread to (@p exp_type, @p exp_demux_ctx)
 * and checks if @p exp_pkts of @p exp_out were received from @p exp_senders.
 * If no message was received after @ref GNRC_NETTEST_TIMEOUT microseconds, while
 * there are still packets expected, the function will return GNRC_NETTEST_TIMED_OUT.
 *
 * In case of success it releases all packets send by the tested module.
 *
 * @param[in] pid           The thread you want to test the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SND command for.
 * @param[in] in            The packet you want to send through @p pid.
 * @param[in] exp_pkts      The number of packets expected to be received.
 * @param[in] exp_senders   The PID the resulting packet should be coming from.
 *                          Must be of dimension @p exp_pkts.
 * @param[in] exp_out       The expected packet from @p exp_sender.
 *                          Must be of dimension @p exp_pkts.
 * @param[in] exp_type      The expected receiver type for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SND command.
 * @param[in] exp_demux_ctx The expected receiver demux type for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SND command.
 *
 * @return  @see gnrc_nettest_res_t
 */
gnrc_nettest_res_t gnrc_nettest_send(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                     unsigned int exp_pkts,
                                     const kernel_pid_t *exp_senders,
                                     const gnrc_pktsnip_t **exp_out,
                                     gnrc_nettype_t exp_type, uint32_t exp_demux_ctx);

/**
 * @brief   Test @ref GNRC_NETAPI_MSG_TYPE_SND command to @p pid with the receiving
 *          thread being an interface.
 *
 * @details This registered the nettest thread as an interface and checks ifx
 * @p exp_pkts of @p exp_out were received from @p exp_senders. If no message
 * was received after @ref GNRC_NETTEST_TIMEOUT microseconds, while there are
 * still packets expected, the function will return GNRC_NETTEST_TIMED_OUT.
 *
 * In case of success it releases all packets received from the tested module.
 *
 * @param[in] pid           The thread you want to test the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SND command for.
 * @param[in] in            The packet you want to send through @p pid.
 * @param[in] exp_pkts      The number of packets expected to be received.
 * @param[in] exp_senders   The PID the resulting packet should be coming from.
 *                          Must be of dimension @p exp_pkts. May be NULL if
 *                          @p exp_pkts == 0.
 * @param[in] exp_out       The expected packet from @p exp_sender.
 *                          Must be of dimension @p exp_pkts. May be NULL if
 *                          @p exp_pkts == 0.
 *
 * @return  @see gnrc_nettest_res_t
 */
gnrc_nettest_res_t gnrc_nettest_send_iface(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                           unsigned int exp_pkts,
                                           const kernel_pid_t *exp_senders,
                                           const gnrc_pktsnip_t **exp_out);

/**
 * @brief   Test @ref GNRC_NETAPI_MSG_TYPE_RCV command to @p pid.
 *
 * @details This registered the nettest thread to (@p exp_type, @p exp_demux_ctx)
 * and checks if @p exp_pkts of @p exp_out were received from @p exp_senders.
 * If no message was received after @ref GNRC_NETTEST_TIMEOUT microseconds, while
 * there are still packets expected, the function will return GNRC_NETTEST_TIMED_OUT.
 *
 * @param[in] pid           The thread you want to test the
 *                          @ref GNRC_NETAPI_MSG_TYPE_RCV command for.
 * @param[in] in            The packet you want to send through @p pid.
 * @param[in] exp_pkts      The number of packets expected to be received.
 * @param[in] exp_senders   The PID the resulting packet should be coming from.
 *                          Must be of dimension @p exp_pkts.
 * @param[in] exp_out       The expected packet from @p exp_sender.
 *                          Must be of dimension @p exp_pkts.
 * @param[in] exp_type      The expected receiver type for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_RCV command.
 * @param[in] exp_demux_ctx The expected receiver demux type for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_RCV command.
 *
 * @return  @see gnrc_nettest_res_t
 */
gnrc_nettest_res_t gnrc_nettest_receive(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                        unsigned int exp_pkts,
                                        const kernel_pid_t *exp_senders,
                                        const gnrc_pktsnip_t **exp_out,
                                        gnrc_nettype_t exp_type, uint32_t exp_demux_ctx);

/**
 * @brief   Test @ref GNRC_NETAPI_MSG_TYPE_GET command to @p pid.
 *
 * @param[in] pid           The thread you want to test the
 *                          @ref GNRC_NETAPI_MSG_TYPE_GET command for.
 * @param[in] opt           The option you want to test.
 * @param[in] context       The context for the option.
 * @param[in] data          The data pointer for the @ref GNRC_NETAPI_MSG_TYPE_GET
 *                          command.
 * @param[in] data_len      The maximum length for @p data.
 * @param[in] exp_data      The expected value for the returned data. May be
 *                          NULL if @p exp_res < 0
 * @param[in] exp_res       The expected return value for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_GET command.
 *
 * @return  @see gnrc_nettest_res_t
 */
gnrc_nettest_res_t gnrc_nettest_get(kernel_pid_t pid, netopt_t opt,
                                    uint16_t context, void *data, size_t data_len,
                                    void *exp_data, int exp_res);

/**
 * @brief   Test @ref GNRC_NETAPI_MSG_TYPE_SET command to @p pid.
 *
 * @param[in] pid           The thread you want to test the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SET command for.
 * @param[in] opt           The option you want to test.
 * @param[in] context       The context for the option.
 * @param[in] data          The data pointer for the @ref GNRC_NETAPI_MSG_TYPE_SET
 *                          command.
 * @param[in] data_len      The maximum length for @p data.
 * @param[in] exp_res       The expected return value for the
 *                          @ref GNRC_NETAPI_MSG_TYPE_SET command.
 *
 * @return  @see gnrc_nettest_res_t
 */
gnrc_nettest_res_t gnrc_nettest_set(kernel_pid_t pid, netopt_t opt,
                                    uint16_t context, void *data, size_t data_len,
                                    int exp_res);

/**
 * @brief   Initializes the @ref net_gnrc_nettest module.
 *
 * @return  The PID to the nettest thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 */
int gnrc_nettest_init(void);

/**
 * @brief   Resets gnrc_nettest_opt_cbs_t list.
 */
void gnrc_nettest_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETTEST_H_ */
/** @} */
