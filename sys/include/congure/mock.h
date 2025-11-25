/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_congure_mock    CongURE mock implementation
 * @ingroup     sys_congure
 * @brief       A mock for testing @ref sys_congure
 * @{
 *
 * @file
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef CONGURE_MOCK_H
#define CONGURE_MOCK_H

#include <stdint.h>

#include "congure.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A mock CongURE state object
 *
 * @extends congure_snd_t
 */
typedef struct {
    congure_snd_t super;            /**< see @ref congure_snd_t */
    /**
     * @brief   Optional methods called in addition to the tracking functions
     *          of the mock driver
     */
    const congure_snd_driver_t *methods;
    /**
     * @brief   How often was the congure_snd_driver_t::init() method called?
     */
    uint8_t init_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::inter_msg_interval()
     *          method called?
     */
    uint8_t inter_msg_interval_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_msg_sent()
     *          method called?
     */
    uint8_t report_msg_sent_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_msg_discarded()
     *          method called?
     */
    uint8_t report_msg_discarded_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_msgs_timeout()
     *          method called?
     */
    uint8_t report_msgs_timeout_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_msgs_lost()
     *          method called?
     */
    uint8_t report_msgs_lost_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_msg_acked()
     *          method called?
     */
    uint8_t report_msg_acked_calls;
    /**
     * @brief   How often was the congure_snd_driver_t::report_ecn_ce()
     *          method called?
     */
    uint8_t report_ecn_ce_calls;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::init() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE object to initialize */
        void *ctx;              /**< Context for callbacks specific to CC */
    } init_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::inter_msg_interval() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        unsigned msg_size;      /**< The size of the next message to send */
    } inter_msg_interval_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_msg_sent() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        unsigned msg_size;      /**< Size of the message */
    } report_msg_sent_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_msg_discarded() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        unsigned msg_size;      /**< Size of the message */
    } report_msg_discarded_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_msgs_timeout() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        /**
         * @brief   A collection of messages for which the ACK timed out
         */
        congure_snd_msg_t *msgs;
    } report_msgs_timeout_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_msgs_lost() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        /**
         * @brief   A collection of messages that are known to be lost
         */
        congure_snd_msg_t *msgs;
    } report_msgs_lost_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_msg_acked() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        congure_snd_msg_t *msg; /**< The ACK'd message */
        congure_snd_ack_t *ack; /**< The received ACK */
    } report_msg_acked_args;
    /**
     * @brief   What were the arguments for the last
     *          congure_snd_driver_t::report_ecn_ce() call?
     */
    struct {
        congure_snd_t *c;       /**< The CongURE state object */
        /**
         * @brief   Timestamp of the message the CE event occurred for was sent
         */
        ztimer_now_t time;
    } report_ecn_ce_args;
} congure_mock_snd_t;

/**
 * @brief   Sets up the driver for CongURE mock object
 *
 * @param[in] c         A CongURE mock object
 * @param[in] methods   Methods to call in addition to the tracking of the mock
 *                      driver. May be NULL.
 */
void congure_mock_snd_setup(congure_mock_snd_t *c,
                            const congure_snd_driver_t *methods);

#ifdef __cplusplus
}
#endif

#endif /* CONGURE_MOCK_H */
/** @} */
