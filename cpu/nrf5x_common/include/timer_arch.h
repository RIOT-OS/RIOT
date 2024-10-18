/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       CPU specific part of the timer API
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#ifndef TIMER_ARCH_H
#define TIMER_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

static inline bool timer_poll_channel(tim_t tim, int channel)
{
    return timer_config[tim].dev->EVENTS_COMPARE[channel];
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TIMER_ARCH_H */
