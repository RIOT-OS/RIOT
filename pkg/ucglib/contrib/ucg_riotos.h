/*
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_ucglib
 * @{
 *
 * @file
 * @brief       Ucglib driver for interacting with RIOT-OS peripherals
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>

 *
 * @}
 */

#include "ucg.h"

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Holds RIOT-OS specific peripheral data.
 *
 * This structure has to be set via the Ucglib function @p ucg_SetUserPtr,
 * prior to the call to @p ucg_Init.
 *
 * The structure can be easily extended with further required definitions (e.g
 * other pins) if necessary, without breaking the RIOT-OS adaptation of Ucglib.
 */
typedef struct {
    void *user_ptr;             /**< Pointer to optionally store any additional user-data */

    unsigned int device_index;  /**< Index of the SPI device */
    gpio_t pin_cs;              /**< Pin for SPI CS, GPIO_UNDEF if not used */
    gpio_t pin_cd;              /**< Pin for SPI CD, GPIO_UNDEF if not used */
    gpio_t pin_reset;           /**< Pin for RESET, GPIO_UNDEF if not used */
} ucg_riotos_t;

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS
 */
int16_t ucg_com_hw_spi_riotos(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data);

/**
 * To be used as the u8x8_msg_cb as gpio_and_delay_cb in u8x8_Setup() for use with RIOT-OS.
 */
ucg_int_t ucg_dev_dummy_riotos(ucg_t *ucg, ucg_int_t msg, void *data);

#ifdef __cplusplus
}
#endif
