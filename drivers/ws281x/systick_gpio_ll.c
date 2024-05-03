/*
 * Copyright 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of the WS281x abstraction based on GPIO_LL and timers
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "clk.h"
#include "cpu.h"
#include "irq.h"
#include "macros/math.h"
#include "periph/gpio_ll.h"
#include "time_units.h"

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _systick_start(uint32_t ticks)
{
    /* disable SysTick, clear value */
    SysTick->CTRL = 0;
    SysTick->VAL = 0;
    /* prepare value in re-load register */
    SysTick->LOAD = ticks;
    /* start and wait for the load value to be applied */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (SysTick->VAL == 0) { /* Wait for SysTick to start and spin */ }
}

static void _systick_wait(void)
{
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) { /* busy wait */ }
}

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);

    /* the high time for one can be as high as 5 seconds in practise, so
     * rather be on the high side by adding a few CPU cycles. */
    const uint32_t ticks_one = DIV_ROUND_UP((uint64_t)WS281X_T_DATA_ONE_NS * (uint64_t)coreclk(), NS_PER_SEC) + 16;
    /* the low time should rather be on the short side, so rounding down */
    const uint32_t ticks_zero = (uint64_t)(WS281X_T_DATA_ZERO_NS - 50) * (uint64_t)coreclk() / NS_PER_SEC;
    /* the remaining time doesn't matter to much, should only be enough for the
     * LEDs to detect the low phase. And not way to much to be detected as
     * reset */
    const uint32_t ticks_bit = DIV_ROUND((uint64_t)WS281X_T_DATA_NS * (uint64_t)coreclk(), NS_PER_SEC);

    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;

    gpio_port_t port = gpio_get_port(dev->params.pin);
    uword_t mask = 1U << gpio_get_pin_num(dev->params.pin);

    unsigned irq_state = irq_disable();
    while (pos < end) {
        uint8_t data = *pos;
        for (uint8_t cnt = 8; cnt > 0; cnt--) {
            uint32_t ticks_high = (data & 0x80) ? ticks_one : ticks_zero;
            uint32_t ticks_low = ticks_bit - ticks_high;
            _systick_start(ticks_high);
            gpio_ll_set(port, mask);
            _systick_wait();
            gpio_ll_clear(port, mask);
            _systick_start(ticks_low);
            _systick_wait();
            data <<= 1;
        }
        pos++;
    }

    irq_restore(irq_state);
}

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    int err;

    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    gpio_port_t port = gpio_get_port(dev->params.pin);
    uint8_t pin = gpio_get_pin_num(dev->params.pin);

    err = gpio_ll_init(port, pin, gpio_ll_out);
    DEBUG("Initializing port %x pin %d (originally %x): %d\n",
            port, pin, (unsigned)params->pin, err);
    if (err != 0) {
        return -EIO;
    }

    return 0;
}
