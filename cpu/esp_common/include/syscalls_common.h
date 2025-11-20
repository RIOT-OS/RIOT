/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of required system calls for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "esp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Necessary initializations of system call functions */
void syscalls_init(void);

/** Determine free heap size */
unsigned int get_free_heap_size(void);

/** Time since boot in us (32bit version) */
uint32_t system_get_time(void);

/** Time since boot in ms (32bit version) */
uint32_t system_get_time_ms(void);

/** memset version that the compiler should not be allowed to optimize this */
void *system_secure_memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif
