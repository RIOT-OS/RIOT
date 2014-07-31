/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

typedef struct {
    void (*cb)(void);
} gpio_state_t;

// static gpio_state_t config[GPIO_NUMOF];



int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    Pio *port;
    uint32_t pin;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }

    port->PIO_PER = pin;
    port->PIO_OER = pin;
    port->PIO_CODR = pin;
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -2;
        case GPIO_PULLUP:
            port->PIO_PUER = pin;
            break;
        case GPIO_NOPULL:
            port->PIO_PUDR = pin;
            break;
    }
    return 1;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    Pio *port;
    uint32_t pin;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }

    port->PIO_PER = pin;
    port->PIO_ODR = pin;
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -2;
        case GPIO_PULLUP:
            port->PIO_PUER = pin;
            break;
        case GPIO_NOPULL:
            port->PIO_PUDR = pin;
            break;
    }
    return 1;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, void (*cb)(void))
{
    // TODO implement
    return -3;
}

int gpio_read(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            return GPIO_0_DEV->PIO_PDSR & GPIO_0_PIN;
#endif
#if GPIO_1_EN
        case GPIO_1:
            return GPIO_1_DEV->PIO_PDSR & GPIO_1_PIN;
#endif
#if GPIO_2_EN
        case GPIO_2:
            return GPIO_2_DEV->PIO_PDSR & GPIO_2_PIN;
#endif
#if GPIO_3_EN
        case GPIO_3:
            return GPIO_3_DEV->PIO_PDSR & GPIO_3_PIN;
#endif
#if GPIO_4_EN
        case GPIO_4:
            return GPIO_4_DEV->PIO_PDSR & GPIO_4_PIN;
#endif
#if GPIO_5_EN
        case GPIO_5:
            return GPIO_5_DEV->PIO_PDSR & GPIO_5_PIN;
#endif
#if GPIO_6_EN
        case GPIO_6:
            return GPIO_6_DEV->PIO_PDSR & GPIO_6_PIN;
#endif
#if GPIO_7_EN
        case GPIO_7:
            return GPIO_7_DEV->PIO_PDSR & GPIO_7_PIN;
#endif
#if GPIO_8_EN
        case GPIO_8:
            return GPIO_8_DEV->PIO_PDSR & GPIO_8_PIN;
#endif
#if GPIO_9_EN
        case GPIO_9:
            return GPIO_9_DEV->PIO_PDSR & GPIO_9_PIN;
#endif
#if GPIO_10_EN
        case GPIO_10:
            return GPIO_10_DEV->PIO_PDSR & GPIO_10_PIN;
#endif
#if GPIO_11_EN
        case GPIO_11:
            return GPIO_11_DEV->PIO_PDSR & GPIO_11_PIN;
#endif
#if GPIO_12_EN
        case GPIO_12:
            return GPIO_12_DEV->PIO_PDSR & GPIO_12_PIN;
#endif
#if GPIO_13_EN
        case GPIO_13:
            return GPIO_13_DEV->PIO_PDSR & GPIO_13_PIN;
#endif
#if GPIO_14_EN
        case GPIO_14:
            return GPIO_14_DEV->PIO_PDSR & GPIO_14_PIN;
#endif
#if GPIO_15_EN
        case GPIO_15:
            return GPIO_15_DEV->PIO_PDSR & GPIO_15_PIN;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
}

int gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            return GPIO_0_DEV->PIO_SODR = GPIO_0_PIN;
#endif
#if GPIO_1_EN
        case GPIO_1:
            return GPIO_1_DEV->PIO_SODR = GPIO_1_PIN;
#endif
#if GPIO_2_EN
        case GPIO_2:
            return GPIO_2_DEV->PIO_SODR = GPIO_2_PIN;
#endif
#if GPIO_3_EN
        case GPIO_3:
            return GPIO_3_DEV->PIO_SODR = GPIO_3_PIN;
#endif
#if GPIO_4_EN
        case GPIO_4:
            return GPIO_4_DEV->PIO_SODR = GPIO_4_PIN;
#endif
#if GPIO_5_EN
        case GPIO_5:
            return GPIO_5_DEV->PIO_SODR = GPIO_5_PIN;
#endif
#if GPIO_6_EN
        case GPIO_6:
            return GPIO_6_DEV->PIO_SODR = GPIO_6_PIN;
#endif
#if GPIO_7_EN
        case GPIO_7:
            return GPIO_7_DEV->PIO_SODR = GPIO_7_PIN;
#endif
#if GPIO_8_EN
        case GPIO_8:
            return GPIO_8_DEV->PIO_SODR = GPIO_8_PIN;
#endif
#if GPIO_9_EN
        case GPIO_9:
            return GPIO_9_DEV->PIO_SODR = GPIO_9_PIN;
#endif
#if GPIO_10_EN
        case GPIO_10:
            return GPIO_10_DEV->PIO_SODR = GPIO_10_PIN;
#endif
#if GPIO_11_EN
        case GPIO_11:
            return GPIO_11_DEV->PIO_SODR = GPIO_11_PIN;
#endif
#if GPIO_12_EN
        case GPIO_12:
            return GPIO_12_DEV->PIO_SODR = GPIO_12_PIN;
#endif
#if GPIO_13_EN
        case GPIO_13:
            return GPIO_13_DEV->PIO_SODR = GPIO_13_PIN;
#endif
#if GPIO_14_EN
        case GPIO_14:
            return GPIO_14_DEV->PIO_SODR = GPIO_14_PIN;
#endif
#if GPIO_15_EN
        case GPIO_15:
            return GPIO_15_DEV->PIO_SODR = GPIO_15_PIN;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
}

int gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            return GPIO_0_DEV->PIO_CODR = GPIO_0_PIN;
#endif
#if GPIO_1_EN
        case GPIO_1:
            return GPIO_1_DEV->PIO_CODR = GPIO_1_PIN;
#endif
#if GPIO_2_EN
        case GPIO_2:
            return GPIO_2_DEV->PIO_CODR = GPIO_2_PIN;
#endif
#if GPIO_3_EN
        case GPIO_3:
            return GPIO_3_DEV->PIO_CODR = GPIO_3_PIN;
#endif
#if GPIO_4_EN
        case GPIO_4:
            return GPIO_4_DEV->PIO_CODR = GPIO_4_PIN;
#endif
#if GPIO_5_EN
        case GPIO_5:
            return GPIO_5_DEV->PIO_CODR = GPIO_5_PIN;
#endif
#if GPIO_6_EN
        case GPIO_6:
            return GPIO_6_DEV->PIO_CODR = GPIO_6_PIN;
#endif
#if GPIO_7_EN
        case GPIO_7:
            return GPIO_7_DEV->PIO_CODR = GPIO_7_PIN;
#endif
#if GPIO_8_EN
        case GPIO_8:
            return GPIO_8_DEV->PIO_CODR = GPIO_8_PIN;
#endif
#if GPIO_9_EN
        case GPIO_9:
            return GPIO_9_DEV->PIO_CODR = GPIO_9_PIN;
#endif
#if GPIO_10_EN
        case GPIO_10:
            return GPIO_10_DEV->PIO_CODR = GPIO_10_PIN;
#endif
#if GPIO_11_EN
        case GPIO_11:
            return GPIO_11_DEV->PIO_CODR = GPIO_11_PIN;
#endif
#if GPIO_12_EN
        case GPIO_12:
            return GPIO_12_DEV->PIO_CODR = GPIO_12_PIN;
#endif
#if GPIO_13_EN
        case GPIO_13:
            return GPIO_13_DEV->PIO_CODR = GPIO_13_PIN;
#endif
#if GPIO_14_EN
        case GPIO_14:
            return GPIO_14_DEV->PIO_CODR = GPIO_14_PIN;
#endif
#if GPIO_15_EN
        case GPIO_15:
            return GPIO_15_DEV->PIO_CODR = GPIO_15_PIN;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
}

int gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        return gpio_clear(dev);
    } else {
        return gpio_set(dev);
    }
}

int gpio_write(gpio_t dev, int value)
{
    if (value) {
        return gpio_set(dev);
    } else {
        return gpio_clear(dev);
    }
}
