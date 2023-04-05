/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       Peripheral GPIO Low-Level API implementation for the ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "irq.h"
#include "periph/gpio_ll.h"

#include "esp/common_macros.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "gpio_ll_arch.h"
#include "soc/gpio_reg.h"

#include "esp_idf_api/gpio.h"

/* variables that have to be used together with periph/gpio */
#ifdef ESP_PM_WUP_PINS
extern bool _gpio_pin_pu[GPIO_PIN_NUMOF];
extern bool _gpio_pin_pd[GPIO_PIN_NUMOF];
#endif

static gpio_conf_t _gpio_conf[GPIO_PIN_NUMOF] = { };

const _esp32_port_t _esp32_ports[GPIO_PORT_NUMOF] = {
    {
        .out = (uint32_t *)GPIO_OUT_REG,
        .out_w1ts = (uint32_t *)GPIO_OUT_W1TS_REG,
        .out_w1tc = (uint32_t *)GPIO_OUT_W1TC_REG,
        .in = (uint32_t *)GPIO_IN_REG,
        .enable = (uint32_t *)GPIO_ENABLE_REG,
        .enable_w1ts = (uint32_t *)GPIO_ENABLE_W1TS_REG,
        .enable_w1tc = (uint32_t *)GPIO_ENABLE_W1TC_REG,
        .status_w1tc = (uint32_t *)GPIO_STATUS_W1TC_REG,
    },
#if GPIO_PORT_NUMOF > 1
    {
        .out = (uint32_t *)GPIO_OUT1_REG,
        .out_w1ts = (uint32_t *)GPIO_OUT1_W1TS_REG,
        .out_w1tc = (uint32_t *)GPIO_OUT1_W1TC_REG,
        .in = (uint32_t *)GPIO_IN1_REG,
        .enable = (uint32_t *)GPIO_ENABLE1_REG,
        .enable_w1ts = (uint32_t *)GPIO_ENABLE1_W1TS_REG,
        .enable_w1tc = (uint32_t *)GPIO_ENABLE1_W1TC_REG,
        .status_w1tc = (uint32_t *)GPIO_STATUS1_W1TC_REG,
    }
#endif
};

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    assert(port);
    assert(conf);
    assert(GPIO_PORT_NUM(port) < GPIO_PORT_NUMOF);
    assert(pin < GPIO_PORT_PIN_NUMOF(port));

    gpio_t gpio = GPIO_PIN(GPIO_PORT_NUM(port), pin);

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << gpio),
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = false,
        .pull_down_en = false,
    };

    switch (conf->state) {
    case GPIO_OUTPUT_PUSH_PULL:
        cfg.mode = GPIO_MODE_DEF_OUTPUT;
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
        cfg.mode = GPIO_MODE_DEF_OUTPUT | GPIO_MODE_DEF_OD;
        break;
    case GPIO_INPUT:
        cfg.mode = GPIO_MODE_DEF_INPUT;
        break;
    case GPIO_DISCONNECT:
        cfg.mode = GPIO_MODE_DEF_DISABLE;
        break;
    default:
        return -ENOTSUP;
    }

    switch (conf->pull) {
    case GPIO_FLOATING:
        break;
    case GPIO_PULL_UP:
        cfg.pull_up_en = true;
        break;
    case GPIO_PULL_DOWN:
        cfg.pull_down_en = true;
        break;
    default:
        return -ENOTSUP;
    }

    /* Some ESP32x GPIOs may not be available at all */
    if (cfg.pin_bit_mask & ~SOC_GPIO_VALID_GPIO_MASK) {
        return -ENOTSUP;
    }

    /* Some ESP32x GPIOs may have limited features (input only and no pull-up/-down) */
    if ((cfg.pin_bit_mask & ~SOC_GPIO_VALID_OUTPUT_GPIO_MASK) &&
        ((cfg.mode & GPIO_MODE_DEF_OUTPUT) || cfg.pull_up_en || cfg.pull_down_en)) {
        return -ENOTSUP;
    }

#ifdef ESP_PM_WUP_PINS
/* for saving the pullup/pulldown settings of wakeup pins in deep sleep mode */
    _gpio_pin_pu[pin] = cfg.pull_up_en;
    _gpio_pin_pd[pin] = cfg.pull_down_en;
#endif

    if (conf->state == GPIO_DISCONNECT) {
        /* reset the pin to disconnects any other peripheral output configured
           via GPIO Matrix, the pin is reconfigured according to given conf */
        esp_idf_gpio_reset_pin(gpio);
    }

    /* since we can't read back the configuration, we have to save it */
    _gpio_conf[gpio] = *conf;
    _gpio_conf[gpio].schmitt_trigger = false;

    if (esp_idf_gpio_config(&cfg) != ESP_OK) {
        return -ENOTSUP;
    }

    /* if output pin, try to set drive strength */
    gpio_drive_cap_t strength;
    switch (conf->drive_strength) {
    case GPIO_DRIVE_WEAKEST:
        strength = GPIO_DRIVE_CAP_0;
        break;
    case GPIO_DRIVE_WEAK:
        strength = GPIO_DRIVE_CAP_1;
        break;
    case GPIO_DRIVE_STRONG:
        strength = GPIO_DRIVE_CAP_2;
        break;
    case GPIO_DRIVE_STRONGEST:
        strength = GPIO_DRIVE_CAP_3;
        break;
    default:
        strength = GPIO_DRIVE_CAP_DEFAULT;
    }
    if ((cfg.pin_bit_mask & SOC_GPIO_VALID_OUTPUT_GPIO_MASK) &&
        (esp_idf_gpio_set_drive_capability(gpio, strength) != ESP_OK)) {
        return -ENOTSUP;
    }

    if (conf->initial_value) {
        gpio_ll_set(port, 1UL << pin);
    }
    else {
        gpio_ll_clear(port, 1UL << pin);
    }

    return 0;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    assert(dest);

    unsigned state = irq_disable();

    *dest = _gpio_conf[GPIO_PIN(GPIO_PORT_NUM(port), pin)];
    if (dest->state == GPIO_INPUT) {
        dest->initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }
    irq_restore(state);
}
