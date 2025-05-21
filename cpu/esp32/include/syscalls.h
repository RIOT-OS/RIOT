/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
int64_t system_get_time_64 (void);

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
