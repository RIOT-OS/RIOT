/*
 * SPDX-FileCopyrightText: 2015 Jan Wagner <mail@jwagner.eu>
 * SPDX-FileCopyrightText: 2015-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
