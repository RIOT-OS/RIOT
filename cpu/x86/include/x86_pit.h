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
 * @brief Interfacing with the PIT.
 *
 * The core of the x86 port only uses the RTC for the hwtimer.
 * Application developers are free to use this module.
 * Be aware of portability issues.
 *
 * You most likely want to use the vtimer interface instead.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__PIT__H__
#define CPU__X86__PIT__H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIT_CHANNEL_0_PORT (0x40) /**< Channel 0 */
#define PIT_CHANNEL_1_PORT (0x41) /**< Channel 1, DO NOT USE */
#define PIT_CHANNEL_2_PORT (0x42) /**< Channel 2, do not use if you can help it */
#define PIT_COMMAND_PORT   (0x43)

#define PIT_ACCESS_MODE_LATCH_COUNT (0 << 4)
#define PIT_ACCESS_MODE_LO_BYTE     (1 << 4)
#define PIT_ACCESS_MODE_HI_BYTE     (2 << 4)
#define PIT_ACCESS_MODE_LO_HI       (3 << 4)

#define PIT_INTERRUPT_ONTERMINAL_COUNT (0 << 1) /**< */
#define PIT_ONE_SHOT                   (1 << 1) /**< */
#define PIT_RATE_GENERATOR             (2 << 1) /**< */
#define PIT_SQUARE_WAVE                (3 << 1) /**< */
#define PIT_SOFWARE_TRIGGERED_STROBE   (4 << 1) /**< */
#define PIT_HARDWARE_TRIGGERED_STROBE  (5 << 1) /**< */

#define PIT_MIN_HZ (19)
#define PIT_MAX_HZ (1193181)

/**
 * @brief   Initialize the Programmable Interval Timer.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_pit(void);

/**
 * @brief     Reads the current value of the of the channel.
 * @param     channel   Channel (1, 2, or 3) to read from.
 * @returns   Current value of the channel.
 *
 * Channel 1 is the only channel that you should access.
 *
 * Channel 2 might be absent on current systems.
 * It was never free to use for the OSes.
 * Never access this channel!
 *
 * Channel 3 was originally intended to be used by the sound card.
 * It's free to use, but you probably shouldn't.
 */
uint16_t x86_pit_read(unsigned channel);

/**
 * @brief   Sets the speed and operation mode of a channel.
 * @param   channel   Channel (1, 2, or 3) to set.
 * @param   mode      Action on overflow.
 * @param   max       After how many ticks the timer should overflow.
 *
 * See x86_pit_read() for considerations about the channel.
 *
 * The timer has a speed of 1193181hz.
 * That value was convinient for hardware vendors,
 * but has only the quite useless prime factors 3, 11, 19, and 173.
 * That hurts the usefulness severly.
 */
void x86_pit_set2(unsigned channel, unsigned mode, uint16_t max);

/**
 * @brief     Convenience wrapper for x86_pit_set2().
 * @param     hz   After how how long the timer should overflow.
 * @returns   true iff the value of hz was sane.
 *
 * See x86_pit_set2() for a more detailed explanation.
 * max will be set to 1193181 / hz.
 * That means hz has to be a value between 19 and 1193181.
 *
 * Beware: the 1,193,163 different values for hz will only render 2,165 different values for max.
 */
bool x86_pit_set(unsigned channel, unsigned mode, unsigned hz);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
