/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <stdbool.h>

#include "log.h"
#include "periph/gpio_ll_irq.h"

#include "driver/gpio.h"
#include "esp/common_macros.h"
#include "esp_intr_alloc.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "rom/ets_sys.h"

#if __xtensa__
#include "xtensa/xtensa_api.h"
#endif

#include "irq_arch.h"

#if MODULE_PERIPH_GPIO_IRQ
/* variables that have to be used together with periph/gpio_irq */
extern bool gpio_int_enabled_table[];
extern bool gpio_isr_service_installed;
#else
bool gpio_int_enabled_table[GPIO_PIN_NUMOF] = { };
bool gpio_isr_service_installed = false;
#endif

extern void IRAM gpio_int_handler(void* arg);

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig,
                gpio_ll_cb_t cb, void *arg)
{
    assert(is_gpio_port_num_valid(port));
    assert(arg);
    assert(pin < GPIO_PORT_PIN_NUMOF(port));

    unsigned state = irq_disable();

    gpio_t gpio = GPIO_PIN(gpio_port_num(port), pin);

    DEBUG("%s gpio=%u port=%u pin=%u trig=%d cb=%p arg=%p\n",
          __func__, gpio, (unsigned)gpio_port_num(port), pin, trig, cb, arg);

    /* install GPIO ISR of ESP-IDF if not yet done */
    if (!gpio_isr_service_installed &&
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1) != ESP_OK) {
        return -1;
    }
    gpio_isr_service_installed = true;

    /* set the interrupt type for the pin */
    gpio_int_type_t type;
    switch (trig) {
    case GPIO_TRIGGER_EDGE_FALLING:
        type = GPIO_INTR_NEGEDGE;
        break;
    case GPIO_TRIGGER_EDGE_RISING:
        type = GPIO_INTR_POSEDGE;
        break;
    case GPIO_TRIGGER_EDGE_BOTH:
        type = GPIO_INTR_ANYEDGE;
        break;
    case GPIO_TRIGGER_LEVEL_HIGH:
        type = GPIO_INTR_HIGH_LEVEL;
        break;
    case GPIO_TRIGGER_LEVEL_LOW:
        type = GPIO_INTR_LOW_LEVEL;
        break;
    default:
        type = GPIO_INTR_DISABLE;
    }
    if (gpio_set_intr_type(gpio, type) != ESP_OK) {
        return -1;
    }

    /* add interrupt handler for the pin */
    if (gpio_isr_handler_add(gpio, cb, arg) != ESP_OK) {
        return -1;
    }

    /* unmask and clear pending interrupts for the pin */
    gpio_ll_irq_unmask_and_clear(port, pin);

    irq_restore(state);

    return 0;
}

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    gpio_t gpio = GPIO_PIN(gpio_port_num(port), pin);

    DEBUG("%s gpio=%u port=%u pin=%u\n",
          __func__, gpio, (unsigned)gpio_port_num(port), pin);

    if (gpio_intr_disable(gpio) == ESP_OK) {
        gpio_int_enabled_table[gpio] = false;
    }
}

void gpio_ll_irq_unmask(gpio_port_t port, uint8_t pin)
{
    gpio_t gpio = GPIO_PIN(gpio_port_num(port), pin);

    DEBUG("%s gpio=%u port=%u pin=%u\n",
          __func__, gpio, port, pin);

    if (gpio_intr_enable(gpio) == ESP_OK) {
        gpio_int_enabled_table[gpio] = true;
    }
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    gpio_t gpio = GPIO_PIN(gpio_port_num(port), pin);

    DEBUG("%s gpio=%u port=%u pin=%u\n",
          __func__, gpio, (unsigned)gpio_port_num(port), pin);

    volatile uint32_t *status_w1tc = (uint32_t *)GPIO_STATUS_W1TC_REG;
#if GPIO_PORT_NUMOF > 1
    if (gpio_port_num(port) != 0) {
        status_w1tc = (uint32_t *)GPIO_STATUS1_W1TC_REG;
    }
#endif

    *status_w1tc = BIT(pin);

    if (gpio_intr_enable(gpio) == ESP_OK) {
        gpio_int_enabled_table[gpio] = true;
    }
}
