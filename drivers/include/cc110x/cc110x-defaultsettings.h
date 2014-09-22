/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       TI Chipcon CC110x default settings
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef CC110X_DEFAULTSETTINGS_H
#define CC110X_DEFAULTSETTINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns hwtimer ticks per us */
#define RTIMER_TICKS(us) HWTIMER_TICKS(us)

#define TIMER_TICK_USEC_RES     (122)

/* Reset wait time (in reset procedure) */
#define RESET_WAIT_TIME         RTIMER_TICKS(4 * TIMER_TICK_USEC_RES)

/* Time chip needs to go to RX */
#define IDLE_TO_RX_TIME         RTIMER_TICKS(1 * TIMER_TICK_USEC_RES)

/* Time to wait for SO to go low after CS */
#define CS_SO_WAIT_TIME         RTIMER_TICKS(4 * TIMER_TICK_USEC_RES)

/* Max. retries for SO to go low after CS */
#define CC1100_GDO1_LOW_RETRY   (100)

/* The size of the configuration array for CC1100 in bytes */
#define CC1100_CONF_SIZE        (39)

/* The default channel number (0-24) for CC1100 */
#define CC1100_DEFAULT_CHANNR   (0)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC110X_DEFAULTSETTINGS_H */
