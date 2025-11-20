/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_congure_reno    CongURE implementation of TCP Reno
 * @ingroup     sys_congure
 * @brief       Implementation of the TCP Reno congestion control algorithm for
 *              the CongURE framework.
 *
 * @see [RFC 5681](https://tools.ietf.org/html/rfc5681)
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "congure.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of state object for CongURE Reno.
 */
typedef struct congure_reno_snd congure_reno_snd_t;

/**
 * @brief   Constants for the congestion control.
 *
 * Example usage:
 *
 * ~~~~~~~~~~~~~~~~ {.c}
 * static const congure_reno_snd_consts_t consts = {
 *     .fr = _my_fast_retransmit,
 *     .same_wnd_adv = _my_same_window_advertised,
 *     .init_mss = 1460,
 *     .cwnd_upper = 2190,
 *     .cwnd_lower = 1095,
 *     .init_ssthresh = CONGURE_WND_SIZE_MAX,
 *     .frthresh = 3,
 * }
 * static congure_reno_snd_t cong;
 *
 * // ...
 * congure_reno_snd_setup(&cong, &const);
 * ~~~~~~~~~~~~~~~~
 */
typedef struct {
    /**
     * @brief   Callback to enter and perform fast retransmit
     *
     * @param[in] c     The CongURE state object (callback context is at
     *                  `c->super.ctx`)
     */
    void (*fr)(congure_reno_snd_t *c);

    /**
     * @brief   Callback to check if the advertised window within an ACK is the
     *          same as in the context
     *
     * @param[in] c     The CongURE state object (callback context is at
     *                  `c->super.ctx`)
     * @param[in] ack   The ACK to check.
     */
    bool (*same_wnd_adv)(congure_reno_snd_t *c, congure_snd_ack_t *ack);

    /**
     * @brief  Callback to increase congestion window in slow start
     *
     * Defaults to
     *
     * ~~~~~~~~~~ {.c}
     * c->cwnd += c->mss;
     * ~~~~~~~~~~
     *
     * when set to NULL
     *
     * @param[in] c     The CongURE state object (callback context is at
     *                  `c->super.ctx`)
     */
    void (*ss_cwnd_inc)(congure_reno_snd_t *c);

    /**
     * @brief  Callback to increase congestion window in congestion avoidance
     *
     * Defaults to
     *
     * ~~~~~~~~~~ {.c}
     * c->cwnd += (c->in_flight_size < c->mss)
     *          ? c->in_flight_size
     *          : c->mss;
     * ~~~~~~~~~~
     *
     * when set to NULL
     *
     * @param[in] c     The CongURE state object (callback context is at
     *                  `c->super.ctx`)
     */
    void (*ca_cwnd_inc)(congure_reno_snd_t *c);

    /**
     * @brief   Callback to reset congestion window when entering
     *          fast recovery
     *
     * Defaults to
     *
     * ~~~~~~~~~~ {.c}
     * c->ssthresh = max(c->mss * 2, c->cwnd / 2);
     * c->cwnd = c->ssthresh + (3 * c->mss);
     * ~~~~~~~~~~
     *
     * when set to NULL
     *
     * @param[in] c     The CongURE state object (callback context is at
     *                  `c->super.ctx`)
     */
    void (*fr_cwnd_dec)(congure_reno_snd_t *c);

    /**
     * @brief   Initial maximum segment size of the sender in intiator-defined
     *          units
     *
     * 1460 bytes for TCP over Ethernet
     * (see [RFC 3390](https://tools.ietf.org/html/rfc3390)).
     */
    unsigned init_mss;

    /**
     * @brief   Initial upper bound for initial window initiator-defined units
     *
     * 2190 bytes in classic TCP-Reno (3/4 of the assumed MSS for Ethernet,
     * see [RFC 3390](https://tools.ietf.org/html/rfc3390)).
     */
    congure_wnd_size_t cwnd_upper;

    /**
     * @brief   Initial lower bound for initial window initiator-defined units
     *
     * 1095 bytes in classic TCP-Reno (3/8 of the assumed MSS for Ethernet
     * see [RFC 3390](https://tools.ietf.org/html/rfc3390)).
     */
    congure_wnd_size_t cwnd_lower;

    /**
     * @brief   Initial slow-start threshold in initiator-defined units
     */
    congure_wnd_size_t init_ssthresh;

    /**
     * @brief   Threshold for duplicate ACKs to go into Fast Retransmit
     */
    uint8_t frthresh;
} congure_reno_snd_consts_t;

/**
 * @brief   State object for CongURE Reno
 *
 * @extends congure_snd_t
 */
struct congure_reno_snd {
    congure_snd_t super;            /**< see @ref congure_snd_t */

    /**
     * @brief   Constants
     */
    const congure_reno_snd_consts_t *consts;
    uint32_t last_ack;              /**< ID of the last ACK reported */
    /**
     * @brief Maximum segment size of the sender in caller-defined units
     */
    congure_wnd_size_t mss;
    congure_wnd_size_t ssthresh;    /**< Slow-start threshold */
    /**
     * @brief   Sum of caller-defined units of message sizes of all messages
     *          that are yet not ack'd or declared lost
     */
    uint16_t in_flight_size;
    uint8_t dup_acks;               /**< Number of duplicate ACKs reported */
};

/**
 * @brief   Set-up @ref sys_congure_reno driver and constants.
 *
 * @pre Module `congure_reno` is compiled in (note: `congure_reno_methods` can
 *      also compile this module to enable @ref sys_congure_reno_methods, but
 *      not activate the module `congure_reno`)
 *
 * @param[in] c         The @ref sys_congure_reno state object
 * @param[in] consts    The constants to use
 */
void congure_reno_snd_setup(congure_reno_snd_t *c,
                            const congure_reno_snd_consts_t *consts);

/**
 * @defgroup    sys_congure_reno_methods The send driver methods for CongURE TCP Reno
 * @ingroup     sys_congure_reno
 *
 * Many other congestion control mechanisms are just adaptations of TCP Reno,
 * so this makes the methods of @ref sys_congure_reno available to other
 * @ref sys_congure modules. Use module `congure_reno_methods` to only compile
 * in these modules, but not the driver for `congure_reno_snd_t` or
 * @ref congure_reno_snd_setup().
 *
 * @{
 */
/**
 * @brief   Set sender maximum segment size.
 *
 * @attention   This resets congure_reno_snd_t::cwnd to the new initial window
 *              size based on @p mss. So use with care.
 *
 * @param[in] c     A CongURE state object
 * @param[in] mss   Maximum segment size of the sender in caller-defined units
 */
void congure_reno_set_mss(congure_reno_snd_t *c, congure_wnd_size_t mss);

/**
 * @brief   Use to override congure_snd_driver_t::init
 *
 * @param[in,out]   c       The CongURE object to initialize.
 * @param[in]       ctx     Context for callbacks specific to the congestion
 *                          control (such as a TCP PCB). May be NULL.
 */
void congure_reno_snd_init(congure_snd_t *c, void *ctx);

/**
 * @brief   Use to override congure_snd_driver_t::inter_msg_interval
 *
 * @param[in,out]   c       The CongURE object to initialize.
 * @param[in]   msg_size    Size of the next message to sent in caller-defined
 *                          unit.
 *
 * @return  Always -1.
 */
int32_t congure_reno_snd_inter_msg_interval(congure_snd_t *c,
                                            unsigned msg_size);

/**
 * @brief   Use to override congure_snd_driver_t::report_msg_sent
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   msg_size    Size of the message in caller-defined unit.
 */
void congure_reno_snd_report_msg_sent(congure_snd_t *c, unsigned msg_size);

/**
 * @brief   Use to override congure_snd_driver_t::report_msg_discarded
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   msg_size    Size of the discarded message in caller-defined
 *                          unit.
 */
void congure_reno_snd_report_msg_discarded(congure_snd_t *c, unsigned msg_size);

/**
 * @brief   Use to override congure_snd_driver_t::report_msgs_timeout
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   msgs        A collection of messages that are known to be lost.
 *                          The list must not be changed by the method.
 */
void congure_reno_snd_report_msgs_timeout(congure_snd_t *c,
                                          congure_snd_msg_t *msgs);

/**
 * @brief   Use to override congure_snd_driver_t::report_msgs_lost
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   msgs        A collection of messages for which the ACK timed
 *                          out. The list must not be changed by the method.
 */
void congure_reno_snd_report_msgs_lost(congure_snd_t *c,
                                       congure_snd_msg_t *msgs);

/**
 * @brief   Use to override congure_snd_driver_t::report_msg_acked
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   msg         The ACK'd message.
 * @param[in]   ack         The received ACK.
 */
void congure_reno_snd_report_msg_acked(congure_snd_t *c, congure_snd_msg_t *msg,
                                       congure_snd_ack_t *ack);

/**
 * @brief   Use to override congure_snd_driver_t::report_ecn_ce
 *
 * @param[in]   c           The CongURE state object.
 * @param[in]   time        Timestamp in milliseconds of the message the CE
 *                          event occurred for was sent.
 */
void congure_reno_snd_report_ecn_ce(congure_snd_t *c, ztimer_now_t time);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
