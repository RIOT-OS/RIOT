/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_idf_api
 * @{
 *
 * @file
 * @brief       Interface for the ESP-IDF GPIO HAL API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <stdbool.h>

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "hal/gpio_hal.h"
#include "esp_sleep.h"

#include "esp_idf_api/gpio.h"

esp_err_t esp_idf_gpio_config(const gpio_config_t *cfg)
{
    return gpio_config(cfg);
}

esp_err_t esp_idf_gpio_reset_pin(gpio_num_t gpio_num)
{
    return gpio_reset_pin(gpio_num);
}

esp_err_t esp_idf_gpio_intr_enable(gpio_num_t gpio_num)
{
    return gpio_intr_enable(gpio_num);
}

esp_err_t esp_idf_gpio_intr_disable(gpio_num_t gpio_num)
{
    return gpio_intr_disable(gpio_num);
}

esp_err_t esp_idf_gpio_set_intr_type(gpio_num_t gpio_num,
                                     gpio_int_type_t intr_type)
{
    return gpio_set_intr_type(gpio_num, intr_type);
}

esp_err_t esp_idf_gpio_install_isr_service(int intr_alloc_flags)

{
    return gpio_install_isr_service(intr_alloc_flags);
}

esp_err_t esp_idf_gpio_isr_handler_add(gpio_num_t gpio_num,
                                       gpio_isr_t isr_handler, void *args)

{
    return gpio_isr_handler_add(gpio_num, isr_handler, args);
}

esp_err_t esp_idf_gpio_wakeup_enable(gpio_num_t gpio_num,
                                     gpio_int_type_t intr_type)

{
    return gpio_wakeup_enable(gpio_num, intr_type);
}

#ifdef ESP_PM_GPIO_HOLD
esp_err_t esp_idf_gpio_deep_sleep_hold(void)
{
#if SOC_RTCIO_HOLD_SUPPORTED
    extern esp_err_t rtc_gpio_force_hold_en_all(void);
    return rtc_gpio_force_hold_en_all();
#elif SOC_GPIO_SUPPORT_FORCE_HOLD
    gpio_deep_sleep_hold_en();
    return ESP_OK;
#else
    _Static_assert(0, "ESP32x SoC does not support hold feature in deep sleep");
    return ESP_FAIL;
#endif
#if CPU_FAM_ESP32
    /* isolating GPIO12 from external circuits is especially recommended for
     * ESP32-WROVER that have an external pullup on GPIO12 */
    rtc_gpio_isolate(GPIO_NUM_12);
#endif
}
#endif /* ESP_PM_GPIO_HOLD */

esp_err_t esp_idf_gpio_set_drive_capability(gpio_num_t gpio_num,
                                            gpio_drive_cap_t strength)
{
    return gpio_set_drive_capability(gpio_num, strength);
}

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
esp_err_t esp_idf_rtc_gpio_deinit(gpio_num_t gpio_num)

{
    if (!rtc_gpio_is_valid_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    return rtc_gpio_deinit(gpio_num);
}

esp_err_t esp_idf_rtc_gpio_pullup_en(gpio_num_t gpio_num)
{
    if (!rtc_gpio_is_valid_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    return rtc_gpio_pullup_en(gpio_num);
}

esp_err_t esp_idf_rtc_gpio_pullup_dis(gpio_num_t gpio_num)
{
    if (!rtc_gpio_is_valid_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    return rtc_gpio_pullup_dis(gpio_num);
}

esp_err_t esp_idf_rtc_gpio_pulldown_en(gpio_num_t gpio_num)
{
    if (!rtc_gpio_is_valid_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    return rtc_gpio_pulldown_en(gpio_num);
}

esp_err_t esp_idf_rtc_gpio_pulldown_dis(gpio_num_t gpio_num)
{
    if (!rtc_gpio_is_valid_gpio(gpio_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    return rtc_gpio_pulldown_dis(gpio_num);
}

#endif
