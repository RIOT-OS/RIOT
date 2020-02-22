/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_ext
 * @{
 *
 * @file
 * @brief       Example GPIO extender driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef FOO_GPIO_EXT_H
#define FOO_GPIO_EXT_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Example GPIO extender device structure type
 */
typedef struct {
    const char *name;
    gpio_mask_t state;
} foo_gpio_ext_t;

/**
 * @brief   Example GPIO extender driver structure
 */
extern const gpio_driver_t foo_gpio_ext_driver;

/**
 * @name    API of the example GPIO extender driver that implements the
 *          low-level GPIO API.
 * @{
 */
int  foo_gpio_ext_init(foo_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode);
#ifdef MODULE_PERIPH_GPIO_IRQ
int  foo_gpio_ext_init_int(foo_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg);
void foo_gpio_ext_irq_enable(foo_gpio_ext_t *dev, uint8_t pin);
void foo_gpio_ext_irq_disable(foo_gpio_ext_t *dev, uint8_t pin);
#endif
gpio_mask_t foo_gpio_ext_read(foo_gpio_ext_t *dev);
void foo_gpio_ext_set(foo_gpio_ext_t *dev, gpio_mask_t pins);
void foo_gpio_ext_clear(foo_gpio_ext_t *dev, gpio_mask_t pins);
void foo_gpio_ext_toggle(foo_gpio_ext_t *dev, gpio_mask_t pins);
void foo_gpio_ext_write(foo_gpio_ext_t *dev, gpio_mask_t values);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* FOO_GPIO_EXT_H */
/** @} */
