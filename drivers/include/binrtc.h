/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_rtc_binrtc Binary counter as RTC
 * @ingroup     drivers_rtc
 * @brief       Binary counter RTC wrapper declarations
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules
 *
 * The name binrtc means Binary RTC, i.e. an RTC with a binary time counter
 * instead of different calendar registers.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BINRTC_H
#define BINRTC_H

#include "rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief binrtc operations table
 *
 * See `cpu/kinetis_common/periph/rtc.c` for a usage example
 */
extern const rtc_ops_t binrtc_ops;

#ifdef __cplusplus
}
#endif

#endif /* BINRTC_H */

/** @} */
