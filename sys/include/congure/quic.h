/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CONGURE_QUIC_H
#define CONGURE_QUIC_H
/**
 * @defgroup    sys_congure_quic    CongURE implementation of QUIC's CC
 * @ingroup     sys_congure
 * @brief       Implementation of QUIC's congestion control algorithm for the
 *              CongURE framework.
 * @{
 *
 * @file
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "ztimer.h"

#include "congure.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Constants for the congestion control.
 *
 * Example usage (to use the same values as specified in
 * [RFC 9002](https://tools.ietf.org/html/rfc9002#section-7.6)):
 *
 * ~~~~~~~~~~~~~~~~ {.c}
 * static const congure_quic_snd_consts_t consts = {
 *     .cong_event_cb = _maybe_send_one_pkt,
 *     .init_wnd = 12000,                   // 10 * max_datagram_size
 *     .min_wnd = 2400,                     // 2 * max_datagram_size
 *     .init_rtt = 333,                     // kInitialRtt = 333ms
 *     .max_msg_size = 1200,                // max_datagram_size
 *     .pc_thresh = 3000,                   // kPersistentCongestionThreshold = 3s
 *     .granularity = 1,                    // kGranularity = 1ms
 *     .loss_reduction_numerator = 1,       // kLossReductionFactor = .5
 *     .loss_reduction_denominator = 2,
 *     .inter_msg_interval_numerator = 5,   // Pacing factor N = 1.25
 *     .inter_msg_interval_denominator = 4,
 * }
 * static congure_quic_snd_t cong;
 *
 * // ...
 * congure_quic_snd_setup(&cong, &const);
 * ~~~~~~~~~~~~~~~~
 */
typedef struct {
    /**
     * @brief   congestion event callback
     *
     * This callback is called when congestion event is detected by
     * message loss or a CE notification. QUIC typically uses this to send
     * a packet to speed up loss
     * recovery.
     *
     * @param[in] ctx   callback context
     */
    void (*cong_event_cb)(void *ctx);

    /**
     * @brief   Initial congestion window size in initiator-defined units.
     */
    congure_wnd_size_t init_wnd;

    /**
     * @brief   minimum congestion window size in initiator-defined units.
     */
    congure_wnd_size_t min_wnd;

    /**
     * @brief   The assumed RTT in milliseconds before an RTT sample is taken
     */
    uint16_t init_rtt;

    /**
     * @brief   maximum message size in initiator-defined units.
     */
    uint16_t max_msg_size;

    /**
     * @brief   period of time in milliseconds for persistent congestion
     *          to be establisched
     * @see [RFC 9002, section 7.6](https://tools.ietf.org/html/rfc9002#section-7.6)
     */
    uint16_t pc_thresh;

    /**
     * @brief   system timer granularity in milliseconds (typically 1)
     */
    uint16_t granularity;

    /**
     * @brief   numerator for the factor the congestion window should be
     *          reduced by when a new loss event is detected
     */
    uint8_t loss_reduction_numerator;

    /**
     * @brief   denominator for the factor the congestion window should be
     *          reduced by when a new loss event is detected
     */
    uint8_t loss_reduction_denominator;

    /**
     * @brief   numerator for the factor N used to adapt the message interval
     *
     * @see [RFC 9002, section 7.7](https://tools.ietf.org/html/rfc9002#section-7.7)
     */
    uint8_t inter_msg_interval_numerator;

    /**
     * @brief   denominator for the factor N used to adapt the message interval
     *
     * @see [RFC 9002, section 7.7](https://tools.ietf.org/html/rfc9002#section-7.7)
     */
    uint8_t inter_msg_interval_denominator;
} congure_quic_snd_consts_t;

/**
 * @brief   State object for CongURE QUIC
 *
 * @extends congure_snd_t
 */
typedef struct {
    congure_snd_t super;    /**< see @ref congure_snd_t */

    /**
     * @brief   Constants
     */
    const congure_quic_snd_consts_t *consts;

    /**
     * @brief   Timestamp in milliseconds of when the first RTT sample was
     *          obtained
     */
    ztimer_now_t first_rtt_sample;

    /**
     * @brief   Sum of caller-defined units of message sizes of all messages
     *          that are yet not ack'd or declared lost
     */
    unsigned in_flight_size;

    /**
     * @brief   Timestamp in milliseconds of when congestion was first detected.
     *
     * This is the time when congestion recovery mode is entered.
     */
    ztimer_now_t recovery_start;

    /**
     * @brief   Slow start threshold in caller-defined units.
     *
     * When congure_quic_snd_t::cwnd is below congure_quic_snd_t::ssthresh the
     * algorithm is in slow start mode and congure_quic_snd_t::cwnd grows in
     * number of caller-defined units of acknowledged messages sizes
     */
    congure_wnd_size_t ssthresh;

    /**
     * @brief   The smoothed RTT of a connection between peers in milliseconds
     */
    uint16_t smoothed_rtt;

    /**
     * @brief   The RTT variation
     */
    uint16_t rtt_var;

    /**
     * @brief   The minimum RTT seen over a period of time
     */
    uint16_t min_rtt;

    /**
     * @brief   Set to one if congestion control should is limited by the
     *          application or flow control
     *
     * Should be supplied and may be changed by user before calling a @ref
     * sys_congure function.
     *
     * @see [RFC 9002, Appendix B.5](https://tools.ietf.org/html/rfc9002#appendix-B.5)
     */
    uint16_t limited;

    /**
     * @brief   Advertised maximum amount of time in milliseconds a receiver
     *          intends to delay its acknowledgements
     *
     * Used to establish persistent congestion.
     *
     * Should be supplied and may be changed by user before calling a @ref
     * sys_congure function. If this value is not provided by the * protocol,
     * leave it at 0.
     */
    uint16_t max_ack_delay;
} congure_quic_snd_t;

/**
 * @brief   Set's up the driver for a CongURE QUIC object
 *
 * @pre inter_msg_interval_numerator of `consts` must be greater than or equal
 *      to its inter_msg_interval_denominator.
 *      See [RFC 9002, section 7.7](https://tools.ietf.org/html/rfc9002#section-7.7):
 *      > Using a value for "N" that is small, but at least 1 (for
 *      > example, 1.25) ensures that variations in round-trip time do not
 *      > result in under-utilization of the congestion window.
 *
 * @param[in] c         A CongURE QUIC object.
 * @param[in] consts    The constants to use for @p c.
 *                      congure_quic_snd_consts_t::inter_msg_interval_numerator
 *                      must be greater than or equal to
 *                      congure_quic_snd_consts_t::inter_msg_interval_denominator
 */
void congure_quic_snd_setup(congure_quic_snd_t *c,
                            const congure_quic_snd_consts_t *consts);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CONGURE_QUIC_H */
