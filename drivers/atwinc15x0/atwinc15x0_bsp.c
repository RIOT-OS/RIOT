/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       RIOT BSP API implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>

#include "atwinc15x0_internal.h"
#include "mutex.h"
#include "periph/spi.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void atwinc15x0_isr(void *arg)
{
    (void)arg;
    if (atwinc15x0->bsp_isr != NULL && atwinc15x0->bsp_irq_enabled) {
        atwinc15x0->bsp_isr();
    }
    atwinc15x0_irq();
}

sint8 nm_bsp_init(void)
{
    assert(atwinc15x0);
    assert(gpio_is_valid(atwinc15x0->params.reset_pin));
    assert(gpio_is_valid(atwinc15x0->params.irq_pin));

    gpio_init(atwinc15x0->params.reset_pin, GPIO_OUT);
    gpio_set(atwinc15x0->params.reset_pin);

    gpio_init_int(atwinc15x0->params.irq_pin, GPIO_IN_PU, GPIO_FALLING,
                  atwinc15x0_isr, NULL);

    if (gpio_is_valid(atwinc15x0->params.chip_en_pin)) {
        gpio_init(atwinc15x0->params.chip_en_pin, GPIO_OUT);
        gpio_set(atwinc15x0->params.chip_en_pin);
    }

    if (gpio_is_valid(atwinc15x0->params.wake_pin)) {
        gpio_init(atwinc15x0->params.wake_pin, GPIO_OUT);
        gpio_set(atwinc15x0->params.wake_pin);
    }

    return 0;
}

sint8 nm_bsp_deinit(void)
{
    return 0;
}

void nm_bsp_reset(void)
{
    assert(atwinc15x0);
    gpio_clear(atwinc15x0->params.reset_pin);
    nm_bsp_sleep(100);
    gpio_set(atwinc15x0->params.reset_pin);
    nm_bsp_sleep(100);
}

void nm_bsp_sleep(uint32 u32TimeMsec)
{
    ztimer_sleep(ZTIMER_MSEC, u32TimeMsec);
}

void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
    assert(atwinc15x0);

    DEBUG("%s %p\n", __func__, (void *)(uintptr_t)pfIsr);

    atwinc15x0->bsp_isr = pfIsr;
}

void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
    assert(atwinc15x0);

    DEBUG("%s %u\n", __func__, u8Enable);

    atwinc15x0->bsp_irq_enabled = u8Enable;
}
