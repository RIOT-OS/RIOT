/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_congure_abe CongURE implementation of TCP ABE
 * @ingroup     sys_congure
 * @brief       Implementation of the TCP Alternative Backoff with ECN (ABE)
 *              congestion control mechanism for the CongURE framework.
 *
 * @see [RFC 8511](https://tools.ietf.org/html/rfc8511)
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef CONGURE_ABE_H
#define CONGURE_ABE_H

#include <stdint.h>

#include "congure/config.h"
#include "congure/reno.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Alias of CongURE Reno state object for CongURE ABE.
 *
 * @extends congure_reno_snd_t
 */
typedef congure_reno_snd_t congure_abe_snd_t;

/**
 * @brief   Constants for the congestion control.
 *
 * @extends congure_reno_snd_consts_t
 */
typedef struct {
    congure_reno_snd_consts_t reno;     /**< TCP Reno constants */
    /**
     * @brief   numerator for the `beta_{ecn} `factor used to set the slow start
     *          threshold on an ECN CE.
     *
     * Use @ref CONFIG_CONGURE_ABE_MULTIPLIER_NUMERATOR_DEFAULT for a default
     * value
     */
    uint8_t abe_multiplier_numerator;
    /**
     * @brief   numerator for the `beta_{ecn} `factor used to set the slow start
     *          threshold on an ECN CE.
     *
     * Use @ref CONFIG_CONGURE_ABE_MULTIPLIER_DENOMINATOR_DEFAULT for a default
     * value
     */
    uint8_t abe_multiplier_denominator;
} congure_abe_snd_consts_t;

/**
 * @brief   Set's up the driver for a CongURE ABE object
 *
 * @param[in] c         A CongURE ABE object.
 * @param[in] consts    The constants to use for @p c
 */
void congure_abe_snd_setup(congure_abe_snd_t *c,
                           const congure_abe_snd_consts_t *consts);

/**
 * @brief   Set sender maximum segment size.
 *
 * @attention   This resets congure_reno_snd_t::cwnd to the new initial window
 *              size based on @p mss. So use with care.
 *
 * @param[in] c     A CongURE state object
 * @param[in] mss   Maximum segment size of the sender in caller-defined units
 */
static inline void congure_abe_set_mss(congure_abe_snd_t *c, unsigned mss)
{
    congure_reno_set_mss((congure_reno_snd_t *)c, mss);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CONGURE_ABE_H */
