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
    DEBUG ("%s\n", __func__);

    /* suspend UARTs */
    for (int i = 0; i < 3; ++i) {
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        uart_tx_wait_idle(i);
    }

    /* set all power down flags */
    uint32_t pd_flags = RTC_SLEEP_PD_DIG |
                        RTC_SLEEP_PD_RTC_PERIPH |
                        RTC_SLEEP_PD_RTC_SLOW_MEM |
                        RTC_SLEEP_PD_RTC_FAST_MEM |
                        RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU |
                        RTC_SLEEP_PD_VDDSDIO;

    rtc_sleep_config_t config = RTC_SLEEP_CONFIG_DEFAULT(pd_flags);
    config.wifi_pd_en = 1;
    config.rom_mem_pd_en = 1;
    config.lslp_meminf_pd = 1;

    /* Save current frequency and switch to XTAL */
    rtc_cpu_freq_t cpu_freq = rtc_clk_cpu_freq_get();
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);

    /* set deep sleep duration to forever */
    rtc_sleep_set_wakeup_time(rtc_time_get() + ~0x0UL);

    /* configure deep sleep */
    rtc_sleep_init(config);
    rtc_sleep_start(RTC_TIMER_TRIG_EN, 0);

    /* Restore CPU frequency */
    rtc_clk_cpu_freq_set(cpu_freq);

    /* resume UARTs */
    for (int i = 0; i < 3; ++i) {
        REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
        REG_CLR_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XON);
    }
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
