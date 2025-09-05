/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_servo_timer
 * @{
 *
 * @file
 * @brief       Servo motor driver implementation using periph_timer_periodic
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "atomic_utils.h"
#include "irq.h"
#include "kernel_defines.h"
#include "macros/math.h"
#include "periph/gpio.h"
#include "periph/timer.h"
#include "servo.h"
#include "test_utils/expect.h"
#include "time_units.h"

#define ENABLE_DEBUG 0
#include "debug.h"

servo_timer_ctx_t servo_timer_default_ctx;

static unsigned ticks_from_us(uint64_t duration, uint64_t freq)
{
    return DIV_ROUND(duration * freq, US_PER_SEC);
}

/*
 * Timer channel 0 is always used for the rising flank of all servos driven
 * by the same timer. The channels 1 till n are used for the falling flanks of
 * servos 0 till n-1. E.g. as shown in this diagram:
 *
 * Servo_0 ______/""""\___________
 * Servo_1 ______/""""""""\_______
 * ...
 * Servo_n ______/""""""""""""\___
 *
 *               ^    ^   ^   ^
 *               |    |   |   |
 * timer chan 0 -+    |   |   |
 * timer chan 1 ------+   |   |
 * timer chan 2 ----------+   |
 * ...                        |
 * timer chan n+1 ------------+
 *
 * Channel 0 is set to the period of one PWM control cycle and due to flag
 * `TIM_FLAG_RESET_ON_MATCH` will end the period and start the new period. As
 * a result, n+1 channels are needed to control n servos.
 */
static void timer_cb(void *arg, int chan)
{
    servo_timer_ctx_t *ctx = arg;
    if (chan == 0) {
        /* end of period, set the control pin of all controlled servos */
        for (unsigned i = 0; i < ARRAY_SIZE(ctx->servo_map); i++) {
            servo_t *servo = ctx->servo_map[i];
            if (servo) {
                gpio_set(servo->params->servo_pin);
            }
        }
    }
    else {
        /* end of duty cycle of a servo, clear the control pin of the servo
         * for which the timer fired */
        assert((unsigned)chan <= ARRAY_SIZE(ctx->servo_map));
        servo_t *servo = ctx->servo_map[chan - 1];
        assert(servo);
        gpio_clear(servo->params->servo_pin);
    }
}

int servo_init(servo_t *dev, const servo_params_t *params)
{
    memset(dev, 0, sizeof(*dev));
    assert(params->servo_pin != GPIO_UNDEF);
    assert((params->timer_chan > 0)
           && (params->timer_chan <= SERVO_TIMER_MAX_CHAN));
    DEBUG("[servo] init %p for GPIO pin %x\n", (void *)dev,
          (unsigned)params->servo_pin);

    const servo_timer_params_t *timer_params = params->timer;

    gpio_init(params->servo_pin, GPIO_OUT);

    /* Note: This may initialize the timer dev over and over again if multiple
     *       servos are connected to the same timer . But other than wasting CPU
     *       cycles, this does no harm. And it greatly simplifies the API, so
     *       we willfully accept this inefficiency here.
     */
    int retval = timer_init(timer_params->timer, timer_params->timer_freq,
                            timer_cb, timer_params->ctx);
    DEBUG("[servo] timer_init(0x%x, %" PRIu32", timer_cb, ctx)) returned %i\n",
          (unsigned)timer_params->timer, timer_params->timer_freq, retval);

    assert(retval == 0);
    if (retval != 0) {
        return -EINVAL;
    }

    uint32_t servo_period_us = US_PER_SEC / timer_params->servo_freq;
    unsigned ticks = ticks_from_us(servo_period_us, timer_params->timer_freq);
    retval = timer_set_periodic(timer_params->timer, 0, ticks,
                                TIM_FLAG_RESET_ON_MATCH);
    DEBUG("[servo] timer_set_periodic(0x%x, 0, %u) returned %i\n",
          (unsigned)timer_params->timer, ticks, retval);

    assert(retval == 0);
    if (retval != 0) {
        return -ENOTSUP;
    }

    dev->params = params;
    dev->min = ticks_from_us(params->min_us, timer_params->timer_freq);
    dev->max = ticks_from_us(params->max_us, timer_params->timer_freq);

    unsigned irq_state = irq_disable();
    timer_params->ctx->servo_map[params->timer_chan - 1] = dev;
    irq_restore(irq_state);

    servo_set(dev, 127);

    return 0;
}

void servo_set(servo_t *dev, uint8_t pos)
{
    uint32_t target = dev->max - dev->min;
    target *= pos;
    target >>= 8;
    target += dev->min;
    DEBUG("[servo] setting %p to %u (%u / 255)\n",
          (void *)dev, (unsigned)target, (unsigned)pos);

    /* Update duty cycle */
    const servo_params_t *params = dev->params;
    tim_t tim = params->timer->timer;
    int retval = timer_set_periodic(tim, params->timer_chan, target, 0);

    assert(retval == 0);
    DEBUG("[servo] timer_set_periodic(0x%x, %u, %u) returned %i\n",
          (unsigned)tim, (unsigned)params->timer_chan,
          (unsigned)dev->min, retval);
}
