/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_congure CongURE - A Congestion control framework
 * @ingroup     sys
 * @brief       <b>Cong</b>estion control <b>u</b>tilizing <b>r</b>e-usable
 *              <b>e</b>lements
 * @{
 *
 * @file
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "clist.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum value the window size can take
 */
#define CONGURE_WND_SIZE_MAX    (UINT16_MAX)

/**
 * @brief   Define type for window size to allow for possible window size
 *          scaling
 */
typedef uint16_t congure_wnd_size_t;

/**
 * @brief   Forward-declariton for the driver struct
 */
typedef struct congure_snd_driver congure_snd_driver_t;

/**
 * @brief   Base state object for CongURE implementations
 */
typedef struct {
    /**
     * @brief   Driver for the state object. See @ref congure_snd_driver_t.
     */
    const congure_snd_driver_t *driver;
    /**
     * @brief   Context for callbacks specific to the congestion control
     *
     * E.g. A TCP PCB.
     */
    void *ctx;
    congure_wnd_size_t cwnd;        /**< Congestion window size */
} congure_snd_t;

/**
 * @brief   Object to represent a collection of sent messages.
 */
typedef struct {
    clist_node_t super;         /**< see @ref clist_node_t */
    /**
     * @brief   timestamp in milliseconds of when the message was sent.
     */
    ztimer_now_t send_time;
    congure_wnd_size_t size;    /**< size in initiator-defined units */
    /**
     * @brief   number of times the message has already been resent.
     *
     * This does not include the first send.
     */
    uint8_t resends;
} congure_snd_msg_t;

/**
 * @brief   Object to represent an ACK to a message
 */
typedef struct {
    /**
     * @brief   Timestamp in milliseconds of when the ACK was received.
     */
    ztimer_now_t recv_time;
    uint32_t id;            /**< ID of the message the ACK is for */
    /**
     * @brief size of the ACK in initiator-defined units
     */
    congure_wnd_size_t size;
    /**
     * @brief   the peer-reported window size in caller defined units. Leave
     *          0 if not supplied by the protocol.
     */
    congure_wnd_size_t wnd;
    /**
     * @brief   true, if ACK only contains an ACK, false if not
     *
     * This e.g. can be used to tell the congestion control mechanism that the
     * SYN or FIN tag are cleared in the ACK with TCP.
     */
    uint16_t clean;
    /**
     * @brief   the peer-reported time in milliseconds the ACK was delayed
     *          since message reception. Leave 0 if not supplied by the
     *          protocol.
     */
    uint16_t delay;
} congure_snd_ack_t;

/**
 * @brief   Driver for CongURE objects.
 */
struct congure_snd_driver {
    /**
     * @brief   Initializes a CongURE object.
     *
     * @param[in,out]   c       The CongURE object to initialize.
     * @param[in]       ctx     Context for callbacks specific to the
     *                          congestion control (such as a TCP PCB).
     *                          May be NULL.
     */
    void (*init)(congure_snd_t *c, void *ctx);

    /**
     * @brief   Get current interval between messages for pacing.
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msg_size    The size of the next message to send in
     *                          caller-defined unit.
     *
     * @return  The current interval between sent messages in microseconds
     *          when pacing supported by congestion control implementation.
     * @return  -1, if pacing is not supported by the congestion control
     *          implementation.
     */
    int32_t (*inter_msg_interval)(congure_snd_t *c, unsigned msg_size);

    /**
     * @brief   Report that a message was sent.
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msg_size    Size of the message in caller-defined unit.
     */
    void (*report_msg_sent)(congure_snd_t *c, unsigned msg_size);

    /**
     * @brief   Report message as discarded.
     *
     * Discarded messages are not further taken into account for congestion
     * control.
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msg_size    Size of the discarded message in caller-defined
     *                          unit.
     */
    void (*report_msg_discarded)(congure_snd_t *c, unsigned msg_size);

    /**
     * @brief   Report that the ACKs for a collection of messages timed out.
     *
     * @note    As many congestion control algorithms do not distinguish loss
     *          and ACK timeout, this method and
     *          congure_snd_t::report_msgs_lost need to have the same
     *          signature so the same function can be used here
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msgs        A collection of messages for which the ACK
     *                          timed out. The list must not be changed by the
     *                          method.
     */
    void (*report_msgs_timeout)(congure_snd_t *c, congure_snd_msg_t *msgs);

    /**
     * @brief   Report that a collection of messages that is known to be lost.
     *
     * One indicator for a lost message may e.g. be the reception of an ACK of a
     * later sent packet, but not a ACK timeout (see
     * congure_snd_driver_t::report_msgs_timeout() for that).
     *
     * @note    As many congestion control algorithms do not distinguish loss
     *          and ACK timeout, this method and
     *          congure_snd_t::report_msgs_timeout need to have the same
     *          signature so the same function can be used here
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msgs        A collection of messages that are known to
     *                          be lost. The list must not be be changed by the
     *                          method.
     */
    void (*report_msgs_lost)(congure_snd_t *c, congure_snd_msg_t *msgs);

    /**
     * @brief   Report that the ACK for a message was received.
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   msg         The ACK'd message.
     * @param[in]   ack         The received ACK.
     */
    void (*report_msg_acked)(congure_snd_t *c, congure_snd_msg_t *msg,
                             congure_snd_ack_t *ack);

    /**
     * @brief   Report that "congestion encountered" CE signals were received
     *          for a message by means of explicit congestion notification
     *          (ECN).
     *
     * @param[in]   c           The CongURE state object.
     * @param[in]   time        Timestamp in milliseconds of the message the CE
     *                          event occurred for was sent.
     */
    void (*report_ecn_ce)(congure_snd_t *c, ztimer_now_t time);
};

#ifdef __cplusplus
}
#endif

/** @} */
