/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 */

#pragma once

/**
 * @defgroup    sys_riotboot_wdt RIOT watchdog timer
 * @ingroup     sys
 * @{
 *
 * @brief       RIOT bootloader watchdog timer
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.loc>
 * @}
 */

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initial watchdog timeout in milliseconds to confirm a successful boot
 *
 * An initial watchdog timer is started to catch an infinite loop which prevents proper startup.
 * The timeout is doubled on each unsuccessful attempt.
 */
#ifndef CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC
#  define CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC      4000
#endif

static_assert(CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC > 0,
              "CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC must be greater than 0");

/**
 * @brief   Start the watchdog timer to confirm a successful boot
 *
 * The watchdog must be stopped with @ref riotboot_wdt_stop() after a
 * successful boot, otherwise it will reboot the system.
 *
 * @param[in] msec  Initial timeout in milliseconds
 */
void riotboot_wdt_start(unsigned msec);

/**
 * @brief   Stop the watchdog timer started with @ref riotboot_wdt_start()
 */
void riotboot_wdt_stop(void);

#ifdef __cplusplus
}
#endif
