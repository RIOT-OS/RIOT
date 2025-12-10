/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture-specific ADC/DAC definitions for ESP32 variant (family)
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
 * Reference: Technical Reference Manual, Section 4.11 Table 19
 * https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
 *
 * @note    The index of entries in the table MUST correspond to the
 *          RTCIO GPIO number.
 */
const _adc_hw_desc_t _adc_hw[] = {
    /* rtcio,          gpio,                    adc_ctrl,   adc_channel,   pad_name */
    {  RTCIO_GPIO(0),  ADC1_CHANNEL_0_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_0, "SENSOR_VP" },
    {  RTCIO_GPIO(1),  ADC1_CHANNEL_1_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_1, "SENSOR_CAPP" },
    {  RTCIO_GPIO(2),  ADC1_CHANNEL_2_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_2, "SENSOR_CAPN" },
    {  RTCIO_GPIO(3),  ADC1_CHANNEL_3_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_3, "SENSOR_VN" },
    {  RTCIO_GPIO(4),  ADC1_CHANNEL_6_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_6, "VDET_1" },
    {  RTCIO_GPIO(5),  ADC1_CHANNEL_7_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_7, "VDET_2" },
    {  RTCIO_GPIO(6),  ADC2_CHANNEL_8_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_8, "GPIO25" },
    {  RTCIO_GPIO(7),  ADC2_CHANNEL_9_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_9, "GPIO26" },
    {  RTCIO_GPIO(8),  ADC1_CHANNEL_5_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_5, "32K_XN" },
    {  RTCIO_GPIO(9),  ADC1_CHANNEL_4_GPIO_NUM, ADC_UNIT_1, ADC_CHANNEL_4, "32K_XP" },
    {  RTCIO_GPIO(10), ADC2_CHANNEL_0_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_0, "GPIO4" },
    {  RTCIO_GPIO(11), ADC2_CHANNEL_1_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_1, "GPIO0" },
    {  RTCIO_GPIO(12), ADC2_CHANNEL_2_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_2, "GPIO2" },
    {  RTCIO_GPIO(13), ADC2_CHANNEL_3_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_3, "MTDO" },
    {  RTCIO_GPIO(14), ADC2_CHANNEL_4_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_4, "MTCK" },
    {  RTCIO_GPIO(15), ADC2_CHANNEL_5_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_5, "MTDI" },
    {  RTCIO_GPIO(16), ADC2_CHANNEL_6_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_6, "MTMS" },
    {  RTCIO_GPIO(17), ADC2_CHANNEL_7_GPIO_NUM, ADC_UNIT_2, ADC_CHANNEL_7, "GPIO27" },
};

/**
 * @brief   GPIO to RTC IO map (for internal use only)
 *
 * Reference: Technical Reference Manual, Section 4.11 Table 19
 * https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
 */
const gpio_t _gpio_rtcio_map[] = {
    RTCIO_GPIO(11),     /* GPIO0 */
    RTCIO_NA,           /* GPIO1 */
    RTCIO_GPIO(12),     /* GPIO2 */
    RTCIO_NA,           /* GPIO3 */
    RTCIO_GPIO(10),     /* GPIO4 */
    RTCIO_NA,           /* GPIO5 */
    RTCIO_NA,           /* GPIO6 */
    RTCIO_NA,           /* GPIO7 */
    RTCIO_NA,           /* GPIO8 */
    RTCIO_NA,           /* GPIO9 */
    RTCIO_NA,           /* GPIO10 */
    RTCIO_NA,           /* GPIO11 */
    RTCIO_GPIO(15),     /* GPIO12 MTDI */
    RTCIO_GPIO(14),     /* GPIO13 MTCK */
    RTCIO_GPIO(16),     /* GPIO14 MTMS */
    RTCIO_GPIO(13),     /* GPIO15 MTDO */
    RTCIO_NA,           /* GPIO16 */
    RTCIO_NA,           /* GPIO17 */
    RTCIO_NA,           /* GPIO18 */
    RTCIO_NA,           /* GPIO19 */
    RTCIO_NA,           /* GPIO20 */
    RTCIO_NA,           /* GPIO21 */
    RTCIO_NA,           /* GPIO22 */
    RTCIO_NA,           /* GPIO23 */
    RTCIO_NA,           /* GPIO24 */
    RTCIO_GPIO(6),      /* GPIO25 */
    RTCIO_GPIO(7),      /* GPIO26 */
    RTCIO_GPIO(17),     /* GPIO27 */
    RTCIO_NA,           /* GPIO28 */
    RTCIO_NA,           /* GPIO29 */
    RTCIO_NA,           /* GPIO30 */
    RTCIO_NA,           /* GPIO31 */
    RTCIO_GPIO(9),      /* GPIO32 32K_XP */
    RTCIO_GPIO(8),      /* GPIO33 32K_XN */
    RTCIO_GPIO(4),      /* GPIO34 VDET_1 */
    RTCIO_GPIO(5),      /* GPIO35 VDET_2 */
    RTCIO_GPIO(0),      /* GPIO36 SENSOR_VP */
    RTCIO_GPIO(1),      /* GPIO37 SENSOR_CAPP */
    RTCIO_GPIO(2),      /* GPIO38 SENSOR_CAPN */
    RTCIO_GPIO(3),      /* GPIO39 SENSOR_VN */
};

/**
 * @brief   Default voltage range of ADC results for different attenuations
 *
 * These values are used by function adc_get_vrange_min_max if software
 * calibration doesn't work for any reason and the voltage range can't be
 * determined by software.
 */
const int _adc_vrange_min_max[] = {
      0,  950,  /* min, max @ ADC_ATTEN_DB_0   */
      0, 1250,  /* min, max @ ADC_ATTEN_DB_2_5 */
      0, 1750,  /* min, max @ ADC_ATTEN_DB_6   */
      0, 2450,  /* min, max @ ADC_ATTEN_DB_12  */
};

_Static_assert(ARRAY_SIZE(_adc_hw) == SOC_RTCIO_PIN_COUNT,
               "size of _adc_hw does not match SOC_RTCIO_PIN_COUNT");
_Static_assert(ARRAY_SIZE(_gpio_rtcio_map) == SOC_GPIO_PIN_COUNT,
               "size of _gpio_rtcio_map does not match SOC_GPIO_PIN_COUNT");
_Static_assert(ARRAY_SIZE(_adc_vrange_min_max) == (SOC_ADC_ATTEN_NUM * 2),
               "size of _adc_vrange_min_max does not match SOC_ADC_ATTEN_NUM");
