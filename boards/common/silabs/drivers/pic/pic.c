/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_silabs_drivers_pic
 * @{
 *
 * @file
 * @brief       Implementations of the power-and-interrupt controller.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "pic.h"

#include "periph/gpio.h"
#include "periph/i2c.h"

/**
 * @brief   Ensure the interrupt wake pin is defined.
 */
#ifndef PIC_INT_WAKE_PIN
#error "PIC_I2C_WAKE_PIN is not defined by the board."
#endif

/**
 * @brief   Ensure the PIC I2C device is defined.
 */
#ifndef PIC_I2C
#error "PIC_I2C is not defined by the board."
#endif

/**
 * @brief   Ensure the I2C address of the PIC is defined.
 */
#ifndef PIC_I2C_ADDR
#error "PIC_I2C_ADDR is not defined by the board."
#endif

/**
 * @brief   Microsecond sleep method, which does not rely on xtimer.
 *
 * @param[in] delay     Amount of microseconds to delay.
 */
static inline void _usleep(uint32_t delay)
{
    /* decrement + compare take two cycles, therefore divide by two */
    uint32_t count = (delay * (SystemCoreClock / 1000 / 1000)) / 2;

    while (count--) {}
}

void pic_init(void)
{
    gpio_init(PIC_INT_WAKE_PIN, GPIO_OD);
    gpio_set(PIC_INT_WAKE_PIN);
}

void pic_write(uint8_t addr, uint8_t value)
{
    /* toggle the pin for 4 us */
    gpio_clear(PIC_INT_WAKE_PIN);
    _usleep(4);

    /* write to gpio expander */
    i2c_acquire(PIC_I2C);
    uint8_t bytes[] = { addr, value };
    i2c_write_bytes(PIC_I2C, PIC_I2C_ADDR, bytes, 2, 0);
    i2c_release(PIC_I2C);

    /* put PIC in sleep mode again */
    gpio_set(PIC_INT_WAKE_PIN);
}
