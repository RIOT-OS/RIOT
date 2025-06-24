/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture-specific ADC/DAC definitions for ESP32-C2 variant (family)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"

#include "adc_arch_private.h"
#include "esp_common.h"
#include "soc/adc_channel.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   ADC hardware descriptor table (for internal use only)
 *
 * @note    The index of entries in the table MUST correspond to the
 *          RTCIO GPIO number.
 */
const _adc_hw_desc_t _adc_hw[] = {
    /* rtcio,         gpio,                    adc_ctrl,   adc_channel,   pad_name */
    {  RTCIO_GPIO(0), ADC1_CHANNEL_0_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_0, "XTAL_32K_P" },
    {  RTCIO_GPIO(1), ADC1_CHANNEL_1_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_1, "XTAL_32K_P" },
    {  RTCIO_GPIO(2), ADC1_CHANNEL_2_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_2, "GPIO2" },
    {  RTCIO_GPIO(3), ADC1_CHANNEL_3_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_3, "GPIO3" },
    {  RTCIO_GPIO(4), ADC1_CHANNEL_4_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_4, "MTMS" },
    {  RTCIO_GPIO(5), ADC1_CHANNEL_5_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_5, "MTDI" },
    {  RTCIO_GPIO(6), ADC1_CHANNEL_6_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_6, "MTCK" },
};

/**
 * @brief   GPIO to RTC IO map (for internal use only)
 */
const gpio_t _gpio_rtcio_map[] = {
    RTCIO_GPIO(0),   /* GPIO0 */
    RTCIO_GPIO(1),   /* GPIO1 */
    RTCIO_GPIO(2),   /* GPIO2 */
    RTCIO_GPIO(3),   /* GPIO3 */
    RTCIO_GPIO(4),   /* GPIO4 */
    RTCIO_GPIO(5),   /* GPIO5 */
    RTCIO_GPIO(6),   /* GPIO6 */
    RTCIO_NA,        /* GPIO7 */
    RTCIO_NA,        /* GPIO8 */
    RTCIO_NA,        /* GPIO9 */
    RTCIO_NA,        /* GPIO10 */
    RTCIO_NA,        /* GPIO11 */
    RTCIO_NA,        /* GPIO12 */
    RTCIO_NA,        /* GPIO13 */
    RTCIO_NA,        /* GPIO14 */
    RTCIO_NA,        /* GPIO15 */
    RTCIO_NA,        /* GPIO16 */
    RTCIO_NA,        /* GPIO17 */
    RTCIO_NA,        /* GPIO18 */
    RTCIO_NA,        /* GPIO19 */
    RTCIO_NA,        /* GPIO20 */
    RTCIO_NA,        /* GPIO21 */
    RTCIO_NA,        /* GPIO22 */
    RTCIO_NA,        /* GPIO23 */
    RTCIO_NA,        /* GPIO24 */
    RTCIO_NA,        /* GPIO25 */
    RTCIO_NA,        /* GPIO26 */
    RTCIO_NA,        /* GPIO27 */
    RTCIO_NA,        /* GPIO28 */
    RTCIO_NA,        /* GPIO29 */
    RTCIO_NA,        /* GPIO30 */
};

/**
 * @brief   Default voltage range of ADC results for different attenuations
 *
 * These values are used by function adc_get_vrange_min_max if software
 * calibration doesn't work for any reason and the voltage range can't be
 * determined by software.
 *
 * Table 5-6 in [ESP32-C6 Datasheet]
 * (https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf).
 */
const int _adc_vrange_min_max[] = {
    0, 1000,    /* min, max @ ADC_ATTEN_DB_0   */
    0, 1300,    /* min, max @ ADC_ATTEN_DB_2_5 */
    0, 1900,    /* min, max @ ADC_ATTEN_DB_6   */
    0, 3300,    /* min, max @ ADC_ATTEN_DB_12  */
};

_Static_assert(ARRAY_SIZE(_gpio_rtcio_map) == SOC_GPIO_PIN_COUNT,
               "size of _gpio_rtcio_map does not match SOC_GPIO_PIN_COUNT");
_Static_assert(ARRAY_SIZE(_adc_vrange_min_max) == (SOC_ADC_ATTEN_NUM * 2),
               "size of _adc_vrange_min_max does not match SOC_ADC_ATTEN_NUM");
