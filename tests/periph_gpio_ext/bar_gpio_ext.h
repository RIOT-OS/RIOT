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

#ifndef BAR_GPIO_EXT_H
#define BAR_GPIO_EXT_H

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
} bar_gpio_ext_t;

/**
 * @brief   Example GPIO extender driver structure
 */
extern const gpio_driver_t bar_gpio_ext_driver;

/**
 * @name    API of the example GPIO extender driver that implements the
 *          low-level GPIO API.
 * @{
 */
int  bar_gpio_ext_init(bar_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode);
#ifdef MODULE_PERIPH_GPIO_IRQ
int  bar_gpio_ext_init_int(bar_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg);
void bar_gpio_ext_irq_enable(bar_gpio_ext_t *dev, uint8_t pin);
void bar_gpio_ext_irq_disable(bar_gpio_ext_t *dev, uint8_t pin);
#endif
gpio_mask_t bar_gpio_ext_read(bar_gpio_ext_t *dev);
void bar_gpio_ext_set(bar_gpio_ext_t *dev, gpio_mask_t pins);
void bar_gpio_ext_clear(bar_gpio_ext_t *dev, gpio_mask_t pins);
void bar_gpio_ext_toggle(bar_gpio_ext_t *dev, gpio_mask_t pins);
void bar_gpio_ext_write(bar_gpio_ext_t *dev, gpio_mask_t values);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BAR_GPIO_EXT_H */
/** @} */
