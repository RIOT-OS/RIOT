/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * Basic definitions for the hwtimer.
 *
 * @ingroup x86
 * @ingroup core_hwtimer
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__HWTIMER_CPU__H__
#define CPU__X86__HWTIMER_CPU__H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of configured hardware timers.
 *
 * This value may be set as high as you need it.
 * The x86_hwtimer.c can emulate a lot of timers, since the real timers aren't used. :-)
 */
#ifndef HWTIMER_MAXTIMERS
#   define HWTIMER_MAXTIMERS 4
#endif

/**
 * @brief   How many "ticks" there are in one second of wallclock time.
 *
 * Statically configured as `1 tick = 1 µs`.
 */
#define HWTIMER_SPEED (1000000L)

/**
 * @brief   Biggest value that can specify a valid number of hardware ticks.
 *
 * Since 1 tick = 1 µs, this number will flow over after 4295 seconds (1h11m34s).
 */
#define HWTIMER_MAXTICKS (0xFFFFFFFFu)

#ifdef __cplusplus
}
#endif

#endif

/** @} */
