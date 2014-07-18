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
 * @addtogroup  sys
 * @{
 * @file
 *
 * @brief       Simple multitasking features.
 * @details     This module enables you to write bad code, that employs busy loops.
 *              The currently running thread will be yielded involuntarily at a frequency of `MULTITASKING_HZ`.
 *              By design of the vtimer, neither ISRs nor code in disableIRQ() sections will be yielded.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef SYS__MULTITASKING__H
#define SYS__MULTITASKING__H

#include <stdbool.h>

/**
 * @def         MULTITASKING_HZ
 * @brief       Frequency with which threads a yielded.
 * @details     A high value will fastly drain your battery.
 *              A low value might make the module futile.
 *              You should try out the least value that works for you yourself.
 *              Might or might now work with values less than 1Hz.
 * @details     The default is 33.0.
 */
#ifndef MULTITASKING_HZ
#   define MULTITASKING_HZ 33.0
#endif

/**
 * @def         MULTITASKING_INITIAL
 * @brief       Whether multitasking should be enabled by directy after starting.
 */
#ifndef MULTITASKING_INITIAL
#   define MULTITASKING_INITIAL 1
#endif

/**
 * @brief       Enable and disable multitasking.
 * @param[in]   active   `true` = active.
 */
void multitasking_active(bool active);

/**
 * @brief       Called in sched_run().
 * @note        Never call on your own accord.
 */
void multitasking_sched_ran(void);

#endif
