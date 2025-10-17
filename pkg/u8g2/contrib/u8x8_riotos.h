/*
 * Copyright (C) 2018 Petr Vyleta <vyleta.developer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_u8g2
 * @{
 *
 * @file
 * @brief       U8g2 driver for interacting with RIOT-OS peripherals
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Petr Vyleta <vyleta.developer@gmail.com>
 *
 * @}
 */

#include "u8x8.h"

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Holds RIOT-OS specific peripheral data.
 *
 * This structure has to be set via the U8g2 function @p u8g2_SetUserPtr, prior
 * the call to @p u8x8_InitDisplay or @p u8g2_InitDisplay respectively.
 *
 * The structure can be easily extended with further required definitions (e.g
 * other pins) if necessary, without breaking the RIOT-OS adaptation of U8g2.
 */
typedef struct {
    void *user_ptr;             /**< Pointer to optionally store any additional user-data */

    unsigned int device_index;  /**< Index of the SPI/I2C device */
    gpio_t pin_cs;              /**< Pin for SPI CS, GPIO_UNDEF if not used */
    gpio_t pin_dc;              /**< Pin for SPI DC, GPIO_UNDEF if not used */
    gpio_t pin_reset;           /**< Pin for RESET, GPIO_UNDEF if not used */
} u8x8_riotos_t;

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS
 */
uint8_t u8x8_gpio_and_delay_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS.
 */
uint8_t u8x8_byte_hw_spi_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS.
 */
uint8_t u8x8_byte_hw_i2c_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#ifdef __cplusplus
}
#endif
