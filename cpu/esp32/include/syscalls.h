/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "syscalls_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Time since boot in us (64bit version) */
uint64_t system_get_time_64 (void);

/** initialize system watchdog timer and start it */
void system_wdt_init (void);

/** start the initialized system watchdog timer */
void system_wdt_start (void);

/** start the running system watchdog timer */
void system_wdt_stop (void);

/** reset the system watchdog timer */
void system_wdt_feed (void);

#ifdef __cplusplus
}
#endif
