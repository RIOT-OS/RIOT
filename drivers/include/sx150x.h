/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx150x SX150x
 * @ingroup     drivers
 * @brief       Driver for Semtech SX150x voltage level shifting GPIO
 *
 * # Implementation status
 * - support for keypad functionality missing (applicable to SX1508, SX1509)
 * - support for interrupts missing
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the SX150x driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SX150X_H
#define SX150X_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_SX1507) || defined(MODULE_SX1509)
#define SX150X_ADDR_DEFAULT         (0x3e)
#elif defined(MODULE_SX1508)
#define SX150X_ADDR_DEFAULT         (0x20)
#else
#error "[SX150x] No specific device defined"
#endif

enum {
    SX150X_OK     =  0,
    SX150X_NOBUS  = -1,
    SX150X_NODEV  = -2,
    SX150X_BUSERR = -3,
    SX150X_ERR    = -4,
};

typedef struct {
    i2c_t bus;
    uint8_t addr;
} sx150x_params_t;

typedef struct {
    i2c_t bus;
    uint8_t addr;
    uint16_t data;
} sx150x_t;

int sx150x_init(sx150x_t *dev, const sx150x_params_t *params);

int sx150x_gpio_init(sx150x_t *dev, unsigned pin, gpio_mode_t mode);
int sx150x_gpio_read(sx150x_t *dev, unsigned pin);
int sx150x_gpio_set(sx150x_t *dev, unsigned pin);
int sx150x_gpio_clear(sx150x_t *dev, unsigned pin);
int sx150x_gpio_toggle(sx150x_t *dev, unsigned pin);
int sx150x_gpio_write(sx150x_t *dev, unsigned pin, int value);

int sx150x_led_init(sx150x_t *dev, unsigned pin, uint8_t initial_value);
int sx150x_led_set(sx150x_t *dev, unsigned pin, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* SX150X_H */
/** @} */
