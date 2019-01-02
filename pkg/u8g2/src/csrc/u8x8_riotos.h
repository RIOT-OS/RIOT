/*
 * Copyright (C) 2018 Petr Vyleta <vyleta.spam@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_u8g2
 * @{
 *
 * @file        u8x8_riotos.h
 * @brief       U8g2 driver for interacting with RIOT-OS drivers
 *
 * @author      Petr Vyleta <vyleta.spam@gmail.com>
 *
 * @}
 */

#ifndef _U8X8_RIOTOS_H
#define _U8X8_RIOTOS_H

/**
 * Holds RIOT-OS specific data. Has to be set as u8x8->user_ptr either directly,
 * or via u8g2 function u8g2_SetUserPtr() prior the call to u8x8_InitDisplay()
 * or u8g2_InitDisplay() respectively.
 *
 * The structure can be easily extended with further required definitions (e.g
 * other pins) if necessary, without breaking the RIOT-OS adaptation of u8g2.
 */
typedef struct
{
    void *user_ptr; ///< Pointer to optionally store any additional user-data

    unsigned int device_index; ///< Index of the SPI/I2C device
    gpio_t pin_cs; ///< Pin for SPI signal CS, GPIO_UNDEF if not to be used
    gpio_t pin_dc; ///< Pin for SPI signal DC, GPIO_UNDEF if not to be used
    gpio_t pin_reset; ///< Pin for RESET signal, GPIO_UNDEF if not to be used

} u8x8_riotos_t;

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS
 */
uint8_t u8x8_gpio_and_delay_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS.
 */
uint8_t u8x8_byte_riotos_hw_spi(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS.
 */
uint8_t u8x8_byte_riotos_hw_i2c(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif /* _U8X8_RIOTOS_H */
