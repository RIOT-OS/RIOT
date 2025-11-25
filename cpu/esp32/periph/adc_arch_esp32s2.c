/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture-specific ADC/DAC definitions for ESP32-S2 variant (family)
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
 * Reference: Technical Reference Manual, Section 5.12 Table 38
 * https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf
 *
 * @note    The index of entries in the table MUST correspond to the
 *          RTCIO GPIO number.
 */
const _adc_hw_desc_t _adc_hw[] = {
    /* rtcio,          gpio,                    adc_ctrl,   adc_channel,   pad_name */
    {  RTCIO_GPIO(0),  GPIO0,                   ADC_UNIT_MAX, ADC_CHANNEL_MAX, "GPIO0" },
    {  RTCIO_GPIO(1),  ADC1_CHANNEL_0_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_0,   "TOUCH1" },
    {  RTCIO_GPIO(2),  ADC1_CHANNEL_1_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_1,   "TOUCH2" },
    {  RTCIO_GPIO(3),  ADC1_CHANNEL_2_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_2,   "TOUCH3" },
    {  RTCIO_GPIO(4),  ADC1_CHANNEL_3_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_3,   "TOUCH4" },
    {  RTCIO_GPIO(5),  ADC1_CHANNEL_4_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_4,   "TOUCH5" },
    {  RTCIO_GPIO(6),  ADC1_CHANNEL_5_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_5,   "TOUCH6" },
    {  RTCIO_GPIO(7),  ADC1_CHANNEL_6_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_6,   "TOUCH7" },
    {  RTCIO_GPIO(8),  ADC1_CHANNEL_7_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_7,   "TOUCH8" },
    {  RTCIO_GPIO(9),  ADC1_CHANNEL_8_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_8,   "TOUCH9" },
    {  RTCIO_GPIO(10), ADC1_CHANNEL_9_GPIO_NUM, ADC_UNIT_1,   ADC_CHANNEL_9,   "TOUCH10" },
    {  RTCIO_GPIO(11), ADC2_CHANNEL_0_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_0,   "TOUCH11" },
    {  RTCIO_GPIO(12), ADC2_CHANNEL_1_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_1,   "TOUCH12" },
    {  RTCIO_GPIO(13), ADC2_CHANNEL_2_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_2,   "TOUCH13" },
    {  RTCIO_GPIO(14), ADC2_CHANNEL_3_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_3,   "TOUCH14" },
    {  RTCIO_GPIO(15), ADC2_CHANNEL_4_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_4,   "XTAL_32K_P" },
    {  RTCIO_GPIO(16), ADC2_CHANNEL_5_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_5,   "XTAL_32K_N" },
    {  RTCIO_GPIO(17), ADC2_CHANNEL_6_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_6,   "DAC1" },
    {  RTCIO_GPIO(18), ADC2_CHANNEL_7_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_7,   "DAC2" },
    {  RTCIO_GPIO(19), ADC2_CHANNEL_8_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_8,   "USB_D-" },
    {  RTCIO_GPIO(20), ADC2_CHANNEL_9_GPIO_NUM, ADC_UNIT_2,   ADC_CHANNEL_9,   "USB_D+" },
    {  RTCIO_GPIO(21), GPIO21,                  ADC_UNIT_MAX, ADC_CHANNEL_MAX, "GPIO21" },
};

/**
 * @brief   GPIO to RTC IO map (for internal use only)
 *
 * Reference: Technical Reference Manual, Section 5.12 Table 38
 * https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf
 */
const gpio_t _gpio_rtcio_map[] = {
    RTCIO_GPIO(0),      /* GPIO0 */
    RTCIO_GPIO(1),      /* GPIO1 TOUCH1 */
    RTCIO_GPIO(2),      /* GPIO2 TOUCH2 */
    RTCIO_GPIO(3),      /* GPIO3 TOUCH3 */
    RTCIO_GPIO(4),      /* GPIO4 TOUCH4 */
    RTCIO_GPIO(5),      /* GPIO5 TOUCH5 */
    RTCIO_GPIO(6),      /* GPIO6 TOUCH6 */
    RTCIO_GPIO(7),      /* GPIO7 TOUCH7 */
    RTCIO_GPIO(8),      /* GPIO8 TOUCH8 */
    RTCIO_GPIO(9),      /* GPIO9 TOUCH9 */
    RTCIO_GPIO(10),     /* GPIO10 TOUCH10 */
    RTCIO_GPIO(11),     /* GPIO11 TOUCH11 */
    RTCIO_GPIO(12),     /* GPIO12 TOUCH12 */
    RTCIO_GPIO(13),     /* GPIO13 TOUCH13 */
    RTCIO_GPIO(14),     /* GPIO14 TOUCH14 */
    RTCIO_GPIO(15),     /* GPIO15 XTAL_32K_P */
    RTCIO_GPIO(16),     /* GPIO16 XTAL_32K_N */
    RTCIO_GPIO(17),     /* GPIO17 DAC1 */
    RTCIO_GPIO(18),     /* GPIO18 DAC2 */
    RTCIO_GPIO(19),     /* GPIO19 USB_D- */
    RTCIO_GPIO(20),     /* GPIO20 USB_D+ */
    RTCIO_GPIO(21),     /* GPIO21 */
    RTCIO_NA,           /* GPIO22 */
    RTCIO_NA,           /* GPIO23 */
    RTCIO_NA,           /* GPIO24 */
    RTCIO_NA,           /* GPIO25 */
    RTCIO_NA,           /* GPIO26 */
    RTCIO_NA,           /* GPIO27 */
    RTCIO_NA,           /* GPIO28 */
    RTCIO_NA,           /* GPIO29 */
    RTCIO_NA,           /* GPIO30 */
    RTCIO_NA,           /* GPIO31 */
    RTCIO_NA,           /* GPIO32 */
    RTCIO_NA,           /* GPIO33 */
    RTCIO_NA,           /* GPIO34 */
    RTCIO_NA,           /* GPIO35 */
    RTCIO_NA,           /* GPIO36 */
    RTCIO_NA,           /* GPIO37 */
    RTCIO_NA,           /* GPIO38 */
    RTCIO_NA,           /* GPIO39 */
    RTCIO_NA,           /* GPIO40 */
    RTCIO_NA,           /* GPIO41 */
    RTCIO_NA,           /* GPIO42 */
    RTCIO_NA,           /* GPIO43 */
    RTCIO_NA,           /* GPIO44 */
    RTCIO_NA,           /* GPIO45 */
    RTCIO_NA,           /* GPIO46 */
};

_Static_assert(ARRAY_SIZE(_adc_hw) == SOC_RTCIO_PIN_COUNT,
               "size of _adc_hw does not match SOC_RTCIO_PIN_COUNT");
_Static_assert(ARRAY_SIZE(_gpio_rtcio_map) == SOC_GPIO_PIN_COUNT,
               "size of _gpio_rtcio_map does not match SOC_GPIO_PIN_COUNT");
