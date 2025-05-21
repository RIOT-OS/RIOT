/*
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_qdec
 * @{
 *
 * @file
 * @brief       Low-level QDEC driver implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <errno.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/qdec.h"
#include "periph/gpio.h"

#ifdef QDEC_NUMOF

/**
 * @brief   Interrupt context for each configured qdec
 */
static qdec_isr_ctx_t isr_ctx[QDEC_NUMOF];

static inline NRF_QDEC_Type* dev(qdec_t qdec)
{
    (void)qdec;
    return NRF_QDEC;
}

static inline const qdec_conf_t* conf(qdec_t qdec)
{
    return &qdec_config[qdec];
}

int32_t qdec_init(qdec_t qdec, qdec_mode_t mode, qdec_cb_t cb, void *arg)
{
    (void)mode;
    /* Verify parameters */
    assert((qdec < QDEC_NUMOF));

    /* The nrf5x peripheral counts all edges */
    if (mode != QDEC_X4) {
        return -EINVAL;
    }

    isr_ctx[qdec].cb = cb;
    isr_ctx[qdec].arg = arg;

    if (cb) {
        NVIC_EnableIRQ(QDEC_IRQn);
        dev(qdec)->INTENSET = QDEC_INTENSET_ACCOF_Msk;
    }
    else {
        NVIC_DisableIRQ(QDEC_IRQn);
        dev(qdec)->INTENCLR = QDEC_INTENCLR_ACCOF_Msk;
    }

    gpio_init(conf(qdec)->a_pin, GPIO_IN_PU);
    gpio_init(conf(qdec)->b_pin, GPIO_IN_PU);

    dev(qdec)->PSEL.A = conf(qdec)->a_pin;
    dev(qdec)->PSEL.B = conf(qdec)->b_pin;

    /** Optionally set or disable the LED */
    dev(qdec)->PSEL.LED = gpio_is_valid(conf(qdec)->led_pin)
        ? QDEC_PSEL_LED_CONNECT_Msk
        : conf(qdec)->led_pin;

    dev(qdec)->DBFEN = conf(qdec)->debounce_filter ? 1 : 0;

    /* Enable the peripheral */
    dev(qdec)->ENABLE = 1;
    dev(qdec)->TASKS_START = 1;
    return 0;
}

int32_t qdec_read_and_reset(qdec_t qdec)
{
    /* Read and clear ACC register */
    dev(qdec)->TASKS_RDCLRACC = 1;
    return dev(qdec)->ACCREAD;
}

int32_t qdec_read(qdec_t qdec)
{
    return dev(qdec)->ACC;
}

void qdec_start(qdec_t qdec)
{
    dev(qdec)->TASKS_START = 1;
}

void qdec_stop(qdec_t qdec)
{
    dev(qdec)->TASKS_STOP = 1;
    while (dev(qdec)->EVENTS_STOPPED == 0) {}
    dev(qdec)->EVENTS_STOPPED = 0;
}

void isr_qdec(void)
{
    qdec_t qdec = QDEC_DEV(0); /* only one available */
    dev(qdec)->EVENTS_ACCOF = 0;
    isr_ctx[qdec].cb(isr_ctx[qdec].arg);
}
#endif
