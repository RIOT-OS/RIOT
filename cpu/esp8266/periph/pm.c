/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of power management functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include "sdk/sdk.h"
#include "syscalls.h"

void pm_set_lowest(void)
{
    DEBUG("%s enter to sleep @%u\n", __func__, system_get_time());

    /* reset system watchdog timer */
    system_wdt_feed();

    #ifndef MODULE_ESP_QEMU
    /* passive wait for interrupt to leave lowest power mode */
    __asm__ volatile ("waiti 0");
    #endif

    DEBUG("%s exit from sleep @%u\n", __func__, system_get_time());

    /* reset system watchdog timer */
    system_wdt_feed();
}

void pm_off(void)
{
    DEBUG("%s\n", __func__);
    system_deep_sleep(0);
}

void pm_reboot(void)
{
    DEBUG("%s\n", __func__);

#ifdef MODULE_PERIPH_RTT
    /* save counters */
    extern void rtt_save_counter(void);
    rtt_save_counter();
#endif

    /* shut down WIFI and call system_restart_local after timer */
    system_restart ();
}
