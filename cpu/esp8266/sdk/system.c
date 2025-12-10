/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp8266_sdk
 * @{
 *
 * @file
 * @brief       ESP8266 user defined SDK functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "sdk/sdk.h"

#include "esp/dport_regs.h"
#include "esp_sleep.h"
#include "esp_system.h"

uint32_t system_get_chip_id(void)
{
    /* Chip ID as determined by NONOS SDK */
    return (((DPORT.OTP_MAC1  << 8) & 0xffffff00) + ((DPORT.OTP_MAC0 >> 24) & 0xff));
}

const char* system_get_sdk_version(void)
{
    return esp_get_idf_version();
}

void system_deep_sleep(uint32_t time_in_us)
{
    /* TODO test */
    esp_deep_sleep(time_in_us);
}

void system_restart(void)
{
    esp_restart();
}

void system_update_cpu_freq(uint8_t freq)
{
    if (freq == 160) {
        rtc_clk_cpu_freq_set(RTC_CPU_FREQ_160M);
    }
    else {
        rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    }
}
