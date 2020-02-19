/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of power management functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "esp_attr.h"
#include "esp_sleep.h"
#include "syscalls.h"

#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

void pm_set_lowest(void)
{
    DEBUG ("%s enter to sleep @%u\n", __func__, system_get_time());

    #if !defined(QEMU)
    /* passive wait for interrupt to leave lowest power mode */
    __asm__ volatile ("waiti 0");

    /* reset system watchdog timer */
    system_wdt_feed();
    #endif

    DEBUG ("%s exit from sleep @%u\n", __func__, system_get_time());
}

void IRAM_ATTR pm_off(void)
{
    /* enter hibernate mode without any enabled wake-up sources */
    esp_deep_sleep_start();
}

extern void esp_restart_noos(void) __attribute__ ((noreturn));

void pm_reboot(void)
{
    DEBUG ("%s\n", __func__);

    /* suspend and flush UARTs */
    for (int i = 0; i < 3; ++i) {
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        uart_tx_wait_idle(i);
    }

    software_reset();
}
