/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_silabs_drivers_pic Power-and-interrupt driver
 * @ingroup     boards_common_silabs_drivers
 * @{
 *
 * @file
 * @brief       Implementations of the power-and-interrupt controller.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the power-and-interrupt controller.
 *
 * The following values must be defined:
 *
 * - PIC_INT_WAKE_PIN
 * - PIC_I2C
 * - PIC_I2C_ADDR
 */
void pic_init(void);

/**
 * @brief   Write data to the power-and-interrupt controller.
 *
 * @param[in] addr      Address of the peripheral to write to.
 * @param[in] value     Value to write.
 */
void pic_write(uint8_t addr, uint8_t value);

#ifdef __cplusplus
}
#endif

/** @} */
