/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "esp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Necessary initializations of system call functions */
void syscalls_init (void);

/** System standard printf function */
int printf(const char* format, ...);

/** Determine free heap size */
unsigned int get_free_heap_size (void);

/** Time since boot in us (32bit version) */
uint32_t system_get_time (void);

/** Time since boot in us (64bit version) */
uint64_t system_get_time_64 (void);

/** Time since boot in ms (32bit version) */
uint32_t system_get_time_ms (void);

/** initialize system watchdog timer ans start it */
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

#endif /* SYSCALLS_H */
