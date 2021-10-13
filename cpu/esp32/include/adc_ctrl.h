/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ADC controller functions used by ADC and DAC peripherals
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ADC_CTRL_H
#define ADC_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/gpio.h"

/**
 * @brief   ADC controllers
 */
enum {
    ADC1_CTRL,              /**< ADC1 controller */
    ADC2_CTRL               /**< ADC2 controller */
};

/**
 * @brief   RTC IO pin type (does not correspond to RTC gpio num order)
 */
enum {

    RTCIO_TOUCH0 = 0,        /**< touch sensor 0 */
    RTCIO_TOUCH1,            /**< touch sensor 1 */
    RTCIO_TOUCH2,            /**< touch sensor 2 */
    RTCIO_TOUCH3,            /**< touch sensor 3 */
    RTCIO_TOUCH4,            /**< touch sensor 4 */
    RTCIO_TOUCH5,            /**< touch sensor 5 */
    RTCIO_TOUCH6,            /**< touch sensor 6 */
    RTCIO_TOUCH7,            /**< touch sensor 7 */
    RTCIO_TOUCH8,            /**< touch sensor 8, 32K_XP */
    RTCIO_TOUCH9,            /**< touch sensor 9, 32K_XN */

    RTCIO_ADC_ADC1,          /**< VDET_1 */
    RTCIO_ADC_ADC2,          /**< VDET_2 */

    RTCIO_SENSOR_SENSE1,     /**< SENSOR_VP */
    RTCIO_SENSOR_SENSE2,     /**< SENSOR_CAPP */
    RTCIO_SENSOR_SENSE3,     /**< SENSOR_CAPN */
    RTCIO_SENSOR_SENSE4,     /**< SENSOR_VN */

    RTCIO_DAC1,              /**< DAC output */
    RTCIO_DAC2,              /**< DAC output */

    RTCIO_NA,                /**< RTC pad not available */
};

/**
 * @brief   ADC pin hardware information type (for internal use only)
 */
struct _adc_hw_t {
    gpio_t   gpio;          /**< GPIO */
    uint8_t  rtc_gpio;      /**< corresponding RTC GPIO */
    uint8_t  adc_ctrl;      /**< ADC controller */
    uint8_t  adc_channel;   /**< channel of ADC controller */
    char*    pad_name;      /**< symbolic name of pad */
};

/**
 * @brief   RTC hardware map
 *
 * The index corresponds to RTC pin type _rtcio_pin_t (Table 19 in Technical
 * Reference)
 */
extern const struct _adc_hw_t _adc_hw[];

#ifdef __cplusplus
}
#endif

#endif /* ADC_CTRL_H */
