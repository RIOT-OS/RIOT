/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_gsm_ublox Ublox
 * @ingroup     drivers_gsm
 * @brief       A generic implementation for the Ublox gsm modules
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author
 */

#ifndef UBLOX_H
#define UBLOX_H

#include <stdint.h>
#include <stdbool.h>

#include "periph/gpio.h"
#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UBLOX_GPIO1 = 16,
    UBLOX_GPIO2 = 23,
    UBLOX_GPIO3 = 24,
    UBLOX_GPIO4 = 25,
} ublox_gpio_t;

typedef enum {
    UBLOX_GPIO_MODE_OUTPUT = 0,
    UBLOX_GPIO_MODE_INPUT,
    UBLOX_GPIO_MODE_NET_STATUS,
    UBLOX_GPIO_MODE_GNSS_SUPPLY_EN,
    UBLOX_GPIO_MODE_GNSS_DATA_RDY,
    UBLOX_GPIO_MODE_GNSS_RTC_SHARING,
    UBLOX_GPIO_MODE_SIM_CARD_DET,
    UBLOX_GPIO_MODE_HEADSET_DET,
    UBLOX_GPIO_MODE_TX_BURST_IND,
    UBLOX_GPIO_MODE_OP_STATUS_IND,
    UBLOX_GPIO_MODE_FN_STATUS_IND,
    UBLOX_GPIO_MODE_I2S,
    UBLOX_GPIO_MODE_SPI,
    UBLOX_GPIO_MODE_MASTER_CLK,
    UBLOX_GPIO_MODE_UART,
    UBLOX_GPIO_MODE_WIFI_EN,
    UBLOX_GPIO_MODE_RI = 18,
    UBLOX_GPIO_MODE_LAST_GAP_EN,
    UBLOX_GPIO_MODE_DISABLED = 255,
    /* Do not change mode */
    UBLOX_GPIO_MODE_DEFAULT,
} ublox_gpio_mode_t;

#define UBLOX_GPIO_OUTPUT_HIGH  (1 << 9)
#define UBLOX_GPIO_OUTPUT_LOW   (0)

typedef struct ublox_params {
    gsm_params_t base;                  /**< gsm base parameters */
    uint32_t    change_over_baudrate;   /**< initial baudrate */
    gpio_t      pwr_on_pin;             /**< ublox power pin*/
    gpio_t      reset_pin;              /**< ublox reset pin*/
    gpio_t      dtr_pin;                /**< ublox dtr pin (modem intput) */
    uint32_t gpio1_mode;                /**< GPIO1 mode */
    uint32_t gpio2_mode;                /**< GPIO2 mode */
    uint32_t gpio3_mode;                /**< GPIO3 mode */
    uint32_t gpio4_mode;                /**< GPIO4 mode */
    /** A GPS module is connected on the I2C interface */
    bool gps_connected;
    bool gps_pm_exti;
    char *agps_server;      /**< A-GPS server URL */
    char *agps_server2;     /**< A-GPS server URL */
    char *agps_token;       /**< A-GPS token */
} ublox_params_t;

typedef struct ublox_gsm {
    gsm_t base;             /**< gsm base driver */
} ublox_t;

extern const gsm_driver_t ublox_driver;

#ifdef __cplusplus
}
#endif

#endif /* QUECTEL_H */
