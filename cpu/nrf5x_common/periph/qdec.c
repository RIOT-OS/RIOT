/*
 * SPDX-FileCopyrightText: 2021 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Leandro Lanzieri <leandro.lanzieri@tuhh.de>
 *
 * @}
 */
#include <errno.h>

#include "cpu.h"
#include "irq.h"
#include "periph_conf.h"
#include "periph/qdec.h"
#include "periph/gpio.h"

#ifdef QDEC_NUMOF

/**
 * @brief   Interrupt context for each configured qdec
 */
static qdec_isr_ctx_t isr_ctx[QDEC_NUMOF];

/**
 * @brief   Software accumulator for each configured qdec
 */
static int32_t _acc[QDEC_NUMOF];

static inline NRF_QDEC_Type* dev(qdec_t qdec)
{
    (void)qdec;
    return NRF_QDEC;
}

static inline const qdec_conf_t* conf(qdec_t qdec)
{
    return &qdec_config[qdec];
}

/**
 * @brief   Read the hardware accumulator into the software one
 *
 * @param[in] qdec          the qdec device from which to read the hardware accumulator
 */
static void _read_hw_accumulator(qdec_t qdec)
{
    dev(qdec)->TASKS_RDCLRACC = 1;
    _acc[qdec] += (int32_t)dev(qdec)->ACCREAD;
}

int32_t qdec_init(qdec_t qdec, qdec_mode_t mode, qdec_cb_t cb, void *arg)
{
    (void)mode;
    /* Verify parameters */
    assert((qdec < QDEC_NUMOF));
    assert(conf(qdec)->sample_period <= QDEC_SAMPLEPER_SAMPLEPER_131ms);
    assert(conf(qdec)->callback_threshold <= INT32_MAX);

    /* The nrf5x peripheral counts all edges */
    if (mode != QDEC_X4) {
        return -EINVAL;
    }

    isr_ctx[qdec].cb = cb;
    isr_ctx[qdec].arg = arg;
    _acc[qdec] = 0;

    if (cb && (conf(qdec)->callback_threshold > 0)) {
        dev(qdec)->EVENTS_REPORTRDY = 0;
        dev(qdec)->INTENSET = QDEC_INTENSET_REPORTRDY_Msk;
        NVIC_EnableIRQ(QDEC_IRQn);
    }
    else {
        NVIC_DisableIRQ(QDEC_IRQn);
        dev(qdec)->INTENCLR = QDEC_INTENCLR_REPORTRDY_Msk;
    }

    gpio_init(conf(qdec)->a_pin, GPIO_IN_PU);
    gpio_init(conf(qdec)->b_pin, GPIO_IN_PU);

    dev(qdec)->PSEL.A = conf(qdec)->a_pin;
    dev(qdec)->PSEL.B = conf(qdec)->b_pin;

    /* Optionally set or disable the LED */
    if (gpio_is_valid(conf(qdec)->led_pin)) {
        dev(qdec)->PSEL.LED = conf(qdec)->led_pin;
        dev(qdec)->LEDPOL = conf(qdec)->led_active_state ? 1 : 0;
    }
    else {
        dev(qdec)->PSEL.LED = QDEC_PSEL_LED_CONNECT_Msk;
    }

    dev(qdec)->DBFEN = conf(qdec)->debounce_filter ? 1 : 0;
    dev(qdec)->SAMPLEPER = conf(qdec)->sample_period;
    dev(qdec)->REPORTPER = conf(qdec)->report_period;

    /* Enable the peripheral */
    dev(qdec)->ENABLE = 1;
    dev(qdec)->TASKS_START = 1;
    return 0;
}

int32_t qdec_read_and_reset(qdec_t qdec)
{
    unsigned state = irq_disable();

    _read_hw_accumulator(qdec);
    int32_t value = _acc[qdec];
    _acc[qdec] = 0;

    irq_restore(state);
    return value;
}

int32_t qdec_read(qdec_t qdec)
{
    unsigned state = irq_disable();

    _read_hw_accumulator(qdec);
    int32_t value = _acc[qdec];

    irq_restore(state);
    return value;
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

    if (dev(qdec)->EVENTS_REPORTRDY) {
        dev(qdec)->EVENTS_REPORTRDY = 0;

        int32_t prev = _acc[qdec];
        _read_hw_accumulator(qdec);

        /* Only call the callback when the counter crosses +/-callback_threshold */
        int32_t max = conf(qdec)->callback_threshold;
        if ((_acc[qdec] >= max && prev < max) || (_acc[qdec] <= -max && prev > -max)) {
            isr_ctx[qdec].cb(isr_ctx[qdec].arg);
        }
    }

    cortexm_isr_end();
}
#endif
