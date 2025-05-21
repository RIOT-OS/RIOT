/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32_esp_idf_api
 * @{
 *
 * @file
 * @brief       Interface for the ESP-IDF GPIO API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "esp_err.h"
#include "hal/gpio_types.h"

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ESP-IDF interface wrapper functions
 * @{
 */
esp_err_t esp_idf_gpio_config(const gpio_config_t *cfg);
esp_err_t esp_idf_gpio_reset_pin(gpio_num_t gpio_num);

esp_err_t esp_idf_gpio_intr_enable(gpio_num_t gpio_num);
esp_err_t esp_idf_gpio_intr_disable(gpio_num_t gpio_num);
esp_err_t esp_idf_gpio_set_intr_type(gpio_num_t gpio_num,
                                     gpio_int_type_t intr_type);
esp_err_t esp_idf_gpio_install_isr_service(int intr_alloc_flags);
esp_err_t esp_idf_gpio_isr_handler_add(gpio_num_t gpio_num,
                                       gpio_isr_t isr_handler, void *args);

esp_err_t esp_idf_gpio_wakeup_enable(gpio_num_t gpio_num,
                                     gpio_int_type_t intr_type);
esp_err_t esp_idf_gpio_deep_sleep_hold(void);
esp_err_t esp_idf_gpio_set_drive_capability(gpio_num_t gpio_num,
                                            gpio_drive_cap_t strength);

esp_err_t esp_idf_rtc_gpio_deinit(gpio_num_t gpio_num);
esp_err_t esp_idf_rtc_gpio_pullup_en(gpio_num_t gpio_num);
esp_err_t esp_idf_rtc_gpio_pullup_dis(gpio_num_t gpio_num);
esp_err_t esp_idf_rtc_gpio_pulldown_en(gpio_num_t gpio_num);
esp_err_t esp_idf_rtc_gpio_pulldown_dis(gpio_num_t gpio_num);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
