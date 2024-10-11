/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PIC_H
#define PIC_H

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
#endif /* PIC_H */
