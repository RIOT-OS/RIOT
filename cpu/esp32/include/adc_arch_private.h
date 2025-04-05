/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific internal ADC functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/adc_types_legacy.h"
#include "periph/gpio.h"
#include "soc/soc_caps.h"

#ifndef DOXYGEN     /* hide implementation details from doxygen */

#define RTCIO_GPIO(n)   n           /* n-th RTCIO GPIO */
#define RTCIO_NA        UINT8_MAX   /* RTCIO pin not available */

#define ADC_UNIT_INV        ((adc_unit_t)255)                   /* Invalid ADC unit ID */
#define ADC_CHANNEL_INV     SOC_ADC_MAX_CHANNEL_NUM             /* Invalid ADC channel ID */

#define ADC1_CHANNEL_MAX    (SOC_ADC_CHANNEL_NUM(ADC_UNIT_1))   /* Number of channels of ADC1 */
#define ADC2_CHANNEL_MAX    (SOC_ADC_CHANNEL_NUM(ADC_UNIT_2))   /* Number of channels of ADC2 */
#define ADC_CHANNEL_MAX(u)  ((u == ADC_UNIT_1) ? ADC1_CHANNEL_MAX : ADC2_CHANNEL_MAX)

/**
 * @brief   ADC hardware descriptor (for internal use only)
 */
typedef struct {
    uint8_t         rtc_gpio;      /**< RTC GPIO number */
    gpio_t          gpio;          /**< GPIO */
    adc_unit_t      adc_ctrl;      /**< ADC controller */
    adc_channel_t   adc_channel;   /**< channel of ADC controller */
    char*           pad_name;      /**< symbolic name of pad */
} _adc_hw_desc_t;

/**
 * @brief   ADC hardware descriptor table (for internal use only)
 *
 * @note    The index of entries in the table MUST correspond to the
 *          RTCIO GPIO number.
 */
extern const _adc_hw_desc_t _adc_hw[];

/**
 * @brief   GPIO to RTC IO map (for internal use only)
 */
extern const gpio_t _gpio_rtcio_map[];

#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif
