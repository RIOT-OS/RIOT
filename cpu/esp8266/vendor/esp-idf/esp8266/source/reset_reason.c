// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#include <string.h>
#include "esp_system.h"
#include "internal/esp_system_internal.h"
#include "esp8266/rtc_register.h"
#include "esp8266/rom_functions.h"
#include "esp_log.h"
#include "esp_libc.h"

#if CONFIG_RESET_REASON

#define RTC_RESET_SW_CAUSE_REG              RTC_STORE0
#define RTC_RESET_HW_CAUSE_REG              RTC_STATE1
#define RTC_WAKEUP_HW_CAUSE_REG             RTC_STATE2

#define RTC_RESET_HW_CAUSE_LSB              0
#define RTC_RESET_HW_CAUSE_MSB              3

#define RTC_WAKEUP_HW_CAUSE_LSB             8
#define RTC_WAKEUP_HW_CAUSE_MSB             13

static const char *TAG = "reset_reason";
static uint32_t s_reset_reason;

static inline void esp_reset_reason_clear_hint(void)
{
    rtc_sys_info.hint = 0;
}

static inline uint32_t esp_reset_reason_get_hint(uint32_t hw_reset)
{
    if (hw_reset == POWERON_RESET && rtc_sys_info.hint != ESP_RST_SW) {
        uint32_t *p = (uint32_t *)&rtc_sys_info;

        for (unsigned i = 0; i < RTC_SYS_RAM_SIZE / sizeof(uint32_t); i++)
            *p++ = 0;
    }

    return rtc_sys_info.hint;
}

static inline uint32_t esp_rtc_get_reset_reason(void)
{
    return GET_PERI_REG_BITS(RTC_RESET_HW_CAUSE_REG, RTC_RESET_HW_CAUSE_MSB, RTC_RESET_HW_CAUSE_LSB);
}

#if CONFIG_RESET_REASON_CHECK_WAKEUP
static inline uint32_t esp_rtc_get_wakeup_reason(void)
{
    return GET_PERI_REG_BITS(RTC_WAKEUP_HW_CAUSE_REG, RTC_WAKEUP_HW_CAUSE_MSB, RTC_WAKEUP_HW_CAUSE_LSB);
}
#endif

static inline uint32_t get_reset_reason(uint32_t rtc_reset_reason, uint32_t reset_reason_hint)
{
    switch (rtc_reset_reason) {
        case POWERON_RESET:
            if (reset_reason_hint == ESP_RST_SW)
                return reset_reason_hint;
            return ESP_RST_POWERON;
        case EXT_RESET:
#ifdef RIOT_OS
            if (reset_reason_hint == ESP_RST_DEEPSLEEP ||
                reset_reason_hint == ESP_RST_SW ||
                reset_reason_hint == ESP_RST_POWERON) {
                return reset_reason_hint;
            }
#else
            if (reset_reason_hint == ESP_RST_DEEPSLEEP) {
                return reset_reason_hint;
            }
#endif
            return ESP_RST_EXT;

        case SW_RESET:
            if (reset_reason_hint == ESP_RST_PANIC ||
                reset_reason_hint == ESP_RST_BROWNOUT ||
                reset_reason_hint == ESP_RST_TASK_WDT) {
                return reset_reason_hint;
            }
            return ESP_RST_SW;
        case DEEPSLEEP_RESET:
            return ESP_RST_DEEPSLEEP;
        case OWDT_RESET:
            return ESP_RST_WDT;
        case SDIO_RESET:
            return ESP_RST_SDIO;
        default:
            return ESP_RST_UNKNOWN;
    }
}

/**
 * @brief  Internal function to get SoC reset reason at system initialization
 */
void esp_reset_reason_init(void)
{
    const uint32_t hw_reset = esp_rtc_get_reset_reason();
#if CONFIG_RESET_REASON_CHECK_WAKEUP
    const uint32_t hw_wakeup = esp_rtc_get_wakeup_reason();
#else
    const uint32_t hw_wakeup = 0;
#endif
    const uint32_t hint = esp_reset_reason_get_hint(hw_reset);

    s_reset_reason = get_reset_reason(hw_reset, hint);
    if (hint != ESP_RST_UNKNOWN) {
        esp_reset_reason_clear_hint();
    }

    ESP_LOGD(TAG, "RTC reset %u wakeup %u store %u, reason is %u", hw_reset, hw_wakeup, hint, s_reset_reason);
}

/**
 * @brief  Internal function to set reset reason hint
 */
void esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
    rtc_sys_info.hint = hint;
}

/**
 * @brief  Get reason of last reset
 */
esp_reset_reason_t esp_reset_reason(void)
{
    return (esp_reset_reason_t)s_reset_reason;
}

#else /* CONFIG_RESET_REASON */

/**
 * null function for pass compiling
 */
void esp_reset_reason_set_hint(esp_reset_reason_t hint)
{

}

#endif /* CONFIG_RESET_REASON */
