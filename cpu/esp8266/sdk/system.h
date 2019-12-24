/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_sdk
 * @{
 *
 * @file
 * @brief       ESP8266 user defined SDK function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "esp_task_wdt.h"
#include "sdk/ets.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Functions for NONOS SDK compatibility
 *
 * @{
 */
extern uint8_t system_get_cpu_freq(void);
extern void system_update_cpu_freq(uint8_t);
extern uint32_t system_get_chip_id(void);
extern const char* system_get_sdk_version(void);

extern void system_deep_sleep(uint32_t time_in_us);
extern void system_restart(void);

#define system_wdt_init    esp_task_wdt_init
#define system_wdt_feed    esp_task_wdt_reset
#define system_wdt_start   pp_soft_wdt_stop
#define system_wdt_stop    pp_soft_wdt_restart

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SYSTEM_H */
