/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_gpio
 *
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#if GPIO_NUMOF

#ifndef PIN_MUX_FUNCTION_ANALOG
#define PIN_MUX_FUNCTION_ANALOG     0
#endif

#ifndef PIN_MUX_FUNCTION_GPIO
#define PIN_MUX_FUNCTION_GPIO       1
#endif

#ifndef PIN_INTERRUPT_RISING
#define PIN_INTERRUPT_RISING        0x9
#endif

#ifndef PIN_INTERRUPT_FALLING
#define PIN_INTERRUPT_FALLING       0xa
#endif

#ifndef PIN_INTERRUPT_EDGE
#define PIN_INTERRUPT_EDGE          0xb
#endif

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
    uint32_t irqc;      /**< remember interrupt configuration between disable/enable */
} gpio_state_t;

/**
 * @brief Unified IRQ handler shared by all interrupt routines
 *
 * @param[in] dev   the device that triggered the interrupt
 */
static inline void irq_handler(gpio_t dev);

/**
 * @brief Hold one callback function pointer for each gpio device
 */
static gpio_state_t config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    PORT_Type *port;
    GPIO_Type *gpio;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            gpio = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            gpio = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            gpio = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            gpio = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            gpio = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            gpio = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            gpio = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            gpio = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            gpio = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            gpio = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            gpio = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            gpio = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            gpio = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            gpio = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            gpio = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            gpio = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            GPIO_16_CLKEN();
            port = GPIO_16_PORT;
            gpio = GPIO_16_DEV;
            pin = GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            GPIO_17_CLKEN();
            port = GPIO_17_PORT;
            gpio = GPIO_17_DEV;
            pin = GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            GPIO_18_CLKEN();
            port = GPIO_18_PORT;
            gpio = GPIO_18_DEV;
            pin = GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            GPIO_19_CLKEN();
            port = GPIO_19_PORT;
            gpio = GPIO_19_DEV;
            pin = GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            GPIO_20_CLKEN();
            port = GPIO_20_PORT;
            gpio = GPIO_20_DEV;
            pin = GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            GPIO_21_CLKEN();
            port = GPIO_21_PORT;
            gpio = GPIO_21_DEV;
            pin = GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            GPIO_22_CLKEN();
            port = GPIO_22_PORT;
            gpio = GPIO_22_DEV;
            pin = GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            GPIO_23_CLKEN();
            port = GPIO_23_PORT;
            gpio = GPIO_23_DEV;
            pin = GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            GPIO_24_CLKEN();
            port = GPIO_24_PORT;
            gpio = GPIO_24_DEV;
            pin = GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            GPIO_25_CLKEN();
            port = GPIO_25_PORT;
            gpio = GPIO_25_DEV;
            pin = GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            GPIO_26_CLKEN();
            port = GPIO_26_PORT;
            gpio = GPIO_26_DEV;
            pin = GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            GPIO_27_CLKEN();
            port = GPIO_27_PORT;
            gpio = GPIO_27_DEV;
            pin = GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            GPIO_28_CLKEN();
            port = GPIO_28_PORT;
            gpio = GPIO_28_DEV;
            pin = GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            GPIO_29_CLKEN();
            port = GPIO_29_PORT;
            gpio = GPIO_29_DEV;
            pin = GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            GPIO_30_CLKEN();
            port = GPIO_30_PORT;
            gpio = GPIO_30_DEV;
            pin = GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            GPIO_31_CLKEN();
            port = GPIO_31_PORT;
            gpio = GPIO_31_DEV;
            pin = GPIO_31_PIN;
            break;
#endif

        default:
            return -1;
    }

    /* Clear interrupt config */
    config[dev].cb = NULL;
    config[dev].arg = NULL;
    config[dev].irqc = 0;

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PIN_MUX_FUNCTION_ANALOG);

    /* set to push-pull configuration */
    switch (pushpull) {
        case GPIO_PULLUP:
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Pull enable, pull up */
            break;

        case GPIO_PULLDOWN:
            port->PCR[pin] |= PORT_PCR_PE_MASK; /* Pull enable, !pull up */
            break;

        default:
            break;
    }

    gpio->PDDR |= GPIO_PDDR_PDD(1 << pin);     /* set pin to output mode */
    gpio->PCOR |= GPIO_PCOR_PTCO(1 << pin);    /* set output to low */
    /* Select GPIO function for the pin */
    port->PCR[pin] |= PORT_PCR_MUX(PIN_MUX_FUNCTION_GPIO);

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    PORT_Type *port;
    GPIO_Type *gpio;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            gpio = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            gpio = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            gpio = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            gpio = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            gpio = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            gpio = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            gpio = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            gpio = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            gpio = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            gpio = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            gpio = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            gpio = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            gpio = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            gpio = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            gpio = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            gpio = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            GPIO_16_CLKEN();
            port = GPIO_16_PORT;
            gpio = GPIO_16_DEV;
            pin = GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            GPIO_17_CLKEN();
            port = GPIO_17_PORT;
            gpio = GPIO_17_DEV;
            pin = GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            GPIO_18_CLKEN();
            port = GPIO_18_PORT;
            gpio = GPIO_18_DEV;
            pin = GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            GPIO_19_CLKEN();
            port = GPIO_19_PORT;
            gpio = GPIO_19_DEV;
            pin = GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            GPIO_20_CLKEN();
            port = GPIO_20_PORT;
            gpio = GPIO_20_DEV;
            pin = GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            GPIO_21_CLKEN();
            port = GPIO_21_PORT;
            gpio = GPIO_21_DEV;
            pin = GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            GPIO_22_CLKEN();
            port = GPIO_22_PORT;
            gpio = GPIO_22_DEV;
            pin = GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            GPIO_23_CLKEN();
            port = GPIO_23_PORT;
            gpio = GPIO_23_DEV;
            pin = GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            GPIO_24_CLKEN();
            port = GPIO_24_PORT;
            gpio = GPIO_24_DEV;
            pin = GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            GPIO_25_CLKEN();
            port = GPIO_25_PORT;
            gpio = GPIO_25_DEV;
            pin = GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            GPIO_26_CLKEN();
            port = GPIO_26_PORT;
            gpio = GPIO_26_DEV;
            pin = GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            GPIO_27_CLKEN();
            port = GPIO_27_PORT;
            gpio = GPIO_27_DEV;
            pin = GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            GPIO_28_CLKEN();
            port = GPIO_28_PORT;
            gpio = GPIO_28_DEV;
            pin = GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            GPIO_29_CLKEN();
            port = GPIO_29_PORT;
            gpio = GPIO_29_DEV;
            pin = GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            GPIO_30_CLKEN();
            port = GPIO_30_PORT;
            gpio = GPIO_30_DEV;
            pin = GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            GPIO_31_CLKEN();
            port = GPIO_31_PORT;
            gpio = GPIO_31_DEV;
            pin = GPIO_31_PIN;
            break;
#endif

        default:
            return -1;
    }

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PIN_MUX_FUNCTION_ANALOG);

    /* set to push-pull configuration */
    switch (pushpull) {
        case GPIO_NOPULL:
            break;

        case GPIO_PULLUP:
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Pull enable, pull up */
            break;

        case GPIO_PULLDOWN:
            port->PCR[pin] |= PORT_PCR_PE_MASK; /* Pull enable */
            break;

        default:
            break;
    }

    gpio->PDDR &= ~(GPIO_PDDR_PDD(1 << pin));     /* set pin to input mode */
    /* Select GPIO function for the pin */
    port->PCR[pin] |= PORT_PCR_MUX(PIN_MUX_FUNCTION_GPIO);

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    PORT_Type *port;
    int res;
    uint32_t pin = 0;

    res = gpio_init_in(dev, pushpull);

    if (res < 0) {
        return res;
    }

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_0_IRQ);
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            NVIC_SetPriority(GPIO_1_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_1_IRQ);
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            NVIC_SetPriority(GPIO_2_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_2_IRQ);
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            NVIC_SetPriority(GPIO_3_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_3_IRQ);
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            NVIC_SetPriority(GPIO_4_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_4_IRQ);
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            NVIC_SetPriority(GPIO_5_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_5_IRQ);
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            NVIC_SetPriority(GPIO_6_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_6_IRQ);
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            NVIC_SetPriority(GPIO_7_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_7_IRQ);
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            NVIC_SetPriority(GPIO_8_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_8_IRQ);
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            NVIC_SetPriority(GPIO_9_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_9_IRQ);
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            NVIC_SetPriority(GPIO_10_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_10_IRQ);
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            NVIC_SetPriority(GPIO_11_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_11_IRQ);
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            NVIC_SetPriority(GPIO_12_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_12_IRQ);
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            NVIC_SetPriority(GPIO_13_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_13_IRQ);
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            NVIC_SetPriority(GPIO_14_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_14_IRQ);
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            NVIC_SetPriority(GPIO_15_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_15_IRQ);
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            port = GPIO_16_PORT;
            pin = GPIO_16_PIN;
            NVIC_SetPriority(GPIO_16_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_16_IRQ);
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            port = GPIO_17_PORT;
            pin = GPIO_17_PIN;
            NVIC_SetPriority(GPIO_17_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_17_IRQ);
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            port = GPIO_18_PORT;
            pin = GPIO_18_PIN;
            NVIC_SetPriority(GPIO_18_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_18_IRQ);
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            port = GPIO_19_PORT;
            pin = GPIO_19_PIN;
            NVIC_SetPriority(GPIO_19_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_19_IRQ);
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            port = GPIO_20_PORT;
            pin = GPIO_20_PIN;
            NVIC_SetPriority(GPIO_20_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_20_IRQ);
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            port = GPIO_21_PORT;
            pin = GPIO_21_PIN;
            NVIC_SetPriority(GPIO_21_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_21_IRQ);
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            port = GPIO_22_PORT;
            pin = GPIO_22_PIN;
            NVIC_SetPriority(GPIO_22_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_22_IRQ);
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            port = GPIO_23_PORT;
            pin = GPIO_23_PIN;
            NVIC_SetPriority(GPIO_23_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_23_IRQ);
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            port = GPIO_24_PORT;
            pin = GPIO_24_PIN;
            NVIC_SetPriority(GPIO_24_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_24_IRQ);
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            port = GPIO_25_PORT;
            pin = GPIO_25_PIN;
            NVIC_SetPriority(GPIO_25_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_25_IRQ);
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            port = GPIO_26_PORT;
            pin = GPIO_26_PIN;
            NVIC_SetPriority(GPIO_26_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_26_IRQ);
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            port = GPIO_27_PORT;
            pin = GPIO_27_PIN;
            NVIC_SetPriority(GPIO_27_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_27_IRQ);
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            port = GPIO_28_PORT;
            pin = GPIO_28_PIN;
            NVIC_SetPriority(GPIO_28_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_28_IRQ);
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            port = GPIO_29_PORT;
            pin = GPIO_29_PIN;
            NVIC_SetPriority(GPIO_29_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_29_IRQ);
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            port = GPIO_30_PORT;
            pin = GPIO_30_PIN;
            NVIC_SetPriority(GPIO_30_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_30_IRQ);
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            port = GPIO_31_PORT;
            pin = GPIO_31_PIN;
            NVIC_SetPriority(GPIO_31_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_31_IRQ);
            break;
#endif

        default:
            return -1;
    }

    /* set callback */
    config[dev].cb = cb;
    config[dev].arg = arg;

    /* configure the active edges */
    switch (flank) {
        case GPIO_RISING:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_RISING); /* Enable interrupt */
            config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_RISING);
            break;

        case GPIO_FALLING:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_FALLING); /* Enable interrupt */
            config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_FALLING);
            break;

        case GPIO_BOTH:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_EDGE); /* Enable interrupt */
            config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_EDGE);
            break;

        default:
            /* Unknown setting */
            return -1;
    }

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    PORT_Type *port;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            port = GPIO_16_PORT;
            pin = GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            port = GPIO_17_PORT;
            pin = GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            port = GPIO_18_PORT;
            pin = GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            port = GPIO_19_PORT;
            pin = GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            port = GPIO_20_PORT;
            pin = GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            port = GPIO_21_PORT;
            pin = GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            port = GPIO_22_PORT;
            pin = GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            port = GPIO_23_PORT;
            pin = GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            port = GPIO_24_PORT;
            pin = GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            port = GPIO_25_PORT;
            pin = GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            port = GPIO_26_PORT;
            pin = GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            port = GPIO_27_PORT;
            pin = GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            port = GPIO_28_PORT;
            pin = GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            port = GPIO_29_PORT;
            pin = GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            port = GPIO_30_PORT;
            pin = GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            port = GPIO_31_PORT;
            pin = GPIO_31_PIN;
            break;
#endif

        default:
            return;
    }

    /* Restore saved state */
    port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK);
    port->PCR[pin] |= PORT_PCR_IRQC_MASK & config[dev].irqc;
}

void gpio_irq_disable(gpio_t dev)
{
    PORT_Type *port;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            port = GPIO_16_PORT;
            pin = GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            port = GPIO_17_PORT;
            pin = GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            port = GPIO_18_PORT;
            pin = GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            port = GPIO_19_PORT;
            pin = GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            port = GPIO_20_PORT;
            pin = GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            port = GPIO_21_PORT;
            pin = GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            port = GPIO_22_PORT;
            pin = GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            port = GPIO_23_PORT;
            pin = GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            port = GPIO_24_PORT;
            pin = GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            port = GPIO_25_PORT;
            pin = GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            port = GPIO_26_PORT;
            pin = GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            port = GPIO_27_PORT;
            pin = GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            port = GPIO_28_PORT;
            pin = GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            port = GPIO_29_PORT;
            pin = GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            port = GPIO_30_PORT;
            pin = GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            port = GPIO_31_PORT;
            pin = GPIO_31_PIN;
            break;
#endif

        default:
            return;
    }

    /* Save irqc state before disabling to allow enabling with the same trigger settings later. */
    config[dev].irqc = PORT_PCR_IRQC_MASK & port->PCR[pin];
    port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK);
}

int gpio_read(gpio_t dev)
{
    GPIO_Type *gpio;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            gpio = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            gpio = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            gpio = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            gpio = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            gpio = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            gpio = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            gpio = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            gpio = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            gpio = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            gpio = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            gpio = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            gpio = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            gpio = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            gpio = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            gpio = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            gpio = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            gpio = GPIO_16_DEV;
            pin = GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            gpio = GPIO_17_DEV;
            pin = GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            gpio = GPIO_18_DEV;
            pin = GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            gpio = GPIO_19_DEV;
            pin = GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            gpio = GPIO_20_DEV;
            pin = GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            gpio = GPIO_21_DEV;
            pin = GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            gpio = GPIO_22_DEV;
            pin = GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            gpio = GPIO_23_DEV;
            pin = GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            gpio = GPIO_24_DEV;
            pin = GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            gpio = GPIO_25_DEV;
            pin = GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            gpio = GPIO_26_DEV;
            pin = GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            gpio = GPIO_27_DEV;
            pin = GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            gpio = GPIO_28_DEV;
            pin = GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            gpio = GPIO_29_DEV;
            pin = GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            gpio = GPIO_30_DEV;
            pin = GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            gpio = GPIO_31_DEV;
            pin = GPIO_31_PIN;
            break;
#endif

        default:
            return -1;
    }

    if (gpio->PDDR & GPIO_PDDR_PDD(1 << pin)) {      /* if configured as output */
        return gpio->PDOR & GPIO_PDOR_PDO(1 << pin); /* read output data register */
    }
    else {
        return gpio->PDIR & GPIO_PDIR_PDI(1 << pin); /* else read input data register */
    }
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            GPIO_0_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            GPIO_1_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            GPIO_2_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            GPIO_3_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            GPIO_4_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            GPIO_5_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            GPIO_6_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            GPIO_7_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            GPIO_8_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            GPIO_9_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            GPIO_10_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            GPIO_11_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            GPIO_12_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            GPIO_13_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            GPIO_14_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            GPIO_15_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_15_PIN);
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            GPIO_16_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_16_PIN);
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            GPIO_17_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_17_PIN);
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            GPIO_18_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_18_PIN);
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            GPIO_19_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_19_PIN);
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            GPIO_20_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_20_PIN);
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            GPIO_21_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_21_PIN);
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            GPIO_22_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_22_PIN);
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            GPIO_23_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_23_PIN);
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            GPIO_24_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_24_PIN);
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            GPIO_25_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_25_PIN);
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            GPIO_26_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_26_PIN);
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            GPIO_27_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_27_PIN);
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            GPIO_28_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_28_PIN);
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            GPIO_29_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_29_PIN);
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            GPIO_30_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_30_PIN);
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            GPIO_31_DEV->PSOR |= GPIO_PSOR_PTSO(1 << GPIO_31_PIN);
            break;
#endif
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            GPIO_0_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            GPIO_1_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            GPIO_2_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            GPIO_3_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            GPIO_4_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            GPIO_5_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            GPIO_6_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            GPIO_7_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            GPIO_8_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            GPIO_9_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            GPIO_10_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            GPIO_11_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            GPIO_12_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            GPIO_13_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            GPIO_14_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            GPIO_15_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_15_PIN);
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            GPIO_16_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_16_PIN);
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            GPIO_17_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_17_PIN);
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            GPIO_18_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_18_PIN);
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            GPIO_19_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_19_PIN);
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            GPIO_20_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_20_PIN);
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            GPIO_21_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_21_PIN);
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            GPIO_22_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_22_PIN);
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            GPIO_23_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_23_PIN);
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            GPIO_24_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_24_PIN);
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            GPIO_25_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_25_PIN);
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            GPIO_26_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_26_PIN);
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            GPIO_27_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_27_PIN);
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            GPIO_28_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_28_PIN);
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            GPIO_29_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_29_PIN);
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            GPIO_30_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_30_PIN);
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            GPIO_31_DEV->PCOR |= GPIO_PCOR_PTCO(1 << GPIO_31_PIN);
            break;
#endif
    }
}

void gpio_toggle(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            GPIO_0_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            GPIO_1_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            GPIO_2_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            GPIO_3_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            GPIO_4_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            GPIO_5_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            GPIO_6_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            GPIO_7_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            GPIO_8_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            GPIO_9_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            GPIO_10_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            GPIO_11_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            GPIO_12_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            GPIO_13_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            GPIO_14_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            GPIO_15_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_15_PIN);
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            GPIO_16_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_16_PIN);
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            GPIO_17_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_17_PIN);
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            GPIO_18_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_18_PIN);
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            GPIO_19_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_19_PIN);
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            GPIO_20_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_20_PIN);
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            GPIO_21_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_21_PIN);
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            GPIO_22_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_22_PIN);
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            GPIO_23_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_23_PIN);
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            GPIO_24_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_24_PIN);
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            GPIO_25_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_25_PIN);
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            GPIO_26_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_26_PIN);
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            GPIO_27_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_27_PIN);
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            GPIO_28_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_28_PIN);
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            GPIO_29_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_29_PIN);
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            GPIO_30_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_30_PIN);
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            GPIO_31_DEV->PTOR |= GPIO_PTOR_PTTO(1 << GPIO_31_PIN);
            break;
#endif
    }
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    }
    else {
        gpio_clear(dev);
    }
}

static inline void irq_handler(gpio_t dev)
{
    config[dev].cb(config[dev].arg);

    if (sched_context_switch_request) {
        thread_yield();
    }
}


/* the following interrupt handlers are quite ugly, the preprocessor is used to
 * insert only the relevant checks in each isr, however, in the source all of
 * the ISRs contain all GPIO checks...
 */
#define GPIO_ISR_TEMPLATE(gpio) \
    if (gpio##_PORT->ISFR & PORT_ISFR_ISF(1 << gpio##_PIN)) { \
        irq_handler(gpio); \
        /* clear status bit by writing a 1 to it */ \
        gpio##_PORT->ISFR = PORT_ISFR_ISF(1 << gpio##_PIN); \
    }

/* Generate the below part with: (in bash or zsh)
for p in $(seq 8 | tr 123456789 abcdefghi); do echo -n "
#if PORT$(echo $p | tr '[:lower:]' '[:upper:]')_BASE
void ISR_PORT_${p}(void)
{
"; for f in $(seq 0 31); do echo -n "
#if GPIO_${f}_EN && (GPIO_${f}_PORT_BASE == PORT$(echo $p | tr '[:lower:]' '[:upper:]')_BASE)
    GPIO_ISR_TEMPLATE(GPIO_${f})
#endif
"; done;
echo -n "
}
#endif // PORT$(echo $p | tr '[:lower:]' '[:upper:]')_BASE
"; done > gpio-isr.c
*/

/* Script generated code below. */
/* -------------------------------------------------------------------------- */

#if PORTA_BASE
void ISR_PORT_A(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTA_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTA_BASE */

#if PORTB_BASE
void ISR_PORT_B(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTB_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTB_BASE */

#if PORTC_BASE
void ISR_PORT_C(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTC_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTC_BASE */

#if PORTD_BASE
void ISR_PORT_D(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTD_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTD_BASE */

#if PORTE_BASE
void ISR_PORT_E(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTE_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTE_BASE */

#if PORTF_BASE
void ISR_PORT_F(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTF_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTF_BASE */

#if PORTG_BASE
void ISR_PORT_G(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTG_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTG_BASE */

#if PORTH_BASE
void ISR_PORT_H(void)
{

#if GPIO_0_EN && (GPIO_0_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_0)
#endif

#if GPIO_1_EN && (GPIO_1_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_1)
#endif

#if GPIO_2_EN && (GPIO_2_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_2)
#endif

#if GPIO_3_EN && (GPIO_3_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_3)
#endif

#if GPIO_4_EN && (GPIO_4_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_4)
#endif

#if GPIO_5_EN && (GPIO_5_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_5)
#endif

#if GPIO_6_EN && (GPIO_6_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_6)
#endif

#if GPIO_7_EN && (GPIO_7_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_7)
#endif

#if GPIO_8_EN && (GPIO_8_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_8)
#endif

#if GPIO_9_EN && (GPIO_9_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_9)
#endif

#if GPIO_10_EN && (GPIO_10_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_10)
#endif

#if GPIO_11_EN && (GPIO_11_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_11)
#endif

#if GPIO_12_EN && (GPIO_12_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_12)
#endif

#if GPIO_13_EN && (GPIO_13_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_13)
#endif

#if GPIO_14_EN && (GPIO_14_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_14)
#endif

#if GPIO_15_EN && (GPIO_15_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_15)
#endif

#if GPIO_16_EN && (GPIO_16_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_16)
#endif

#if GPIO_17_EN && (GPIO_17_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_17)
#endif

#if GPIO_18_EN && (GPIO_18_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_18)
#endif

#if GPIO_19_EN && (GPIO_19_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_19)
#endif

#if GPIO_20_EN && (GPIO_20_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_20)
#endif

#if GPIO_21_EN && (GPIO_21_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_21)
#endif

#if GPIO_22_EN && (GPIO_22_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_22)
#endif

#if GPIO_23_EN && (GPIO_23_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_23)
#endif

#if GPIO_24_EN && (GPIO_24_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_24)
#endif

#if GPIO_25_EN && (GPIO_25_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_25)
#endif

#if GPIO_26_EN && (GPIO_26_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_26)
#endif

#if GPIO_27_EN && (GPIO_27_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_27)
#endif

#if GPIO_28_EN && (GPIO_28_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_28)
#endif

#if GPIO_29_EN && (GPIO_29_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_29)
#endif

#if GPIO_30_EN && (GPIO_30_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_30)
#endif

#if GPIO_31_EN && (GPIO_31_PORT_BASE == PORTH_BASE)
    GPIO_ISR_TEMPLATE(GPIO_31)
#endif

}
#endif /* PORTH_BASE */

#endif /* GPIO_NUMOF */
