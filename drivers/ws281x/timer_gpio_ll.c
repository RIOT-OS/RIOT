/*
 * SPDX-FileCopyrightText: 2023 Christian Amsüss
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of the WS281x abstraction based on GPIO_LL and timers
 *
 * When this is used, the whole @ref ws281x_write_buffer operation is conducted
 * in a single interrupts-disabled run. As it takes about 1us per bit (eg.
 * 0.3ms on a 100-LED strip), the use of this module needs to be carefully
 * evaluated against other real-time requirements.
 *
 * (Letting the trailing pause run with interrupts enabled, or even enabling
 * interrupts between bits, is an option that is being considered for future
 * expansion).
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "irq.h"
#include "time_units.h"
#include "periph/timer.h"
#include "periph/gpio_ll.h"

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* (+ NS_PER_SEC - 1): Rounding up, as T1H is the time that needs to distinctly
 * longer than T0H.
 *
 * Then adding +1 extra, because the spin loop adds another layer of jitter. A
 * more correct version would be to add the spin loop time before rounding (and
 * then rounding up), but as that time is not available, spending one more
 * cycle is the next best thing to do. */
const int ticks_one = ((uint64_t)WS281X_T_DATA_ONE_NS * WS281X_TIMER_FREQ + NS_PER_SEC - 1)
                    / NS_PER_SEC + 1;
/* Rounding down, zeros are better shorter */
const int ticks_zero = (uint64_t)WS281X_T_DATA_ZERO_NS * (uint64_t)WS281X_TIMER_FREQ / NS_PER_SEC;
/* No particular known requirements, but we're taking longer than that anyway
 * because we don't clock the times between bits. */
const int ticks_data = (uint64_t)WS281X_T_DATA_NS * (uint64_t)WS281X_TIMER_FREQ / NS_PER_SEC;

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);

    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;

    /* The logical sequence for a cycle would be to set high, sleep, set low,
     * sleep. But the critical time is the high time, so we do something
     * different: We go low, set up the timer, and then go high and back low
     * both as controlled by the timer. That means that our logical cycles are
     * the previous bit's low time followed by the current bit's high time -- so we store the previous bit here.
     *
     * It is initialized to 1 because the 1 has the longer high time, so the
     * lower low time -- we just spend 325 instead of 600ns before actually
     * sending the message. (This wait-before-we-start time is also part of how
     * this implementation can get away without compensation for the time it
     * takes to set up timers: it sets them up and then waits for some time
     * that is known to be long enough, because that time also works
     * mid-transmission). */
    bool last_bit = 1;

    gpio_port_t port = gpio_get_port(dev->params.pin);
    uword_t mask = 1 << gpio_get_pin_num(dev->params.pin);

    /* If a too-slow channel is used, then the critical section could be done
     * away with: If an interrupt fires (or a higher priority thread runs;
     * really: runs long enough to matter, but that'd need very fast switching
     * and hundreds of MHz to be the case), the function will return with an
     * error (or ignore the error), and the worst thing that'd happen is that
     * if it hits the last bit of a LED, that might be flipped to a value 1.
     * */
    unsigned int irq_state = irq_disable();

    while (pos < end) {
        uint8_t data = *pos;
        for (uint8_t cnt = 8; cnt > 0; cnt--) {
            bool bit = data >> 7;
            data <<= 1;
            int time_to_set = last_bit ? (ticks_data - ticks_one) : (ticks_data - ticks_zero);
            int time_to_clear = time_to_set + (bit ? ticks_one : ticks_zero);

            /* I'd prefer to just zero the timer, but we don't have API for that */
            int now = timer_read(WS281X_TIMER_DEV);
            time_to_set = (time_to_set + now) & WS281X_TIMER_MAX_VALUE;
            time_to_clear = (time_to_clear + now) & WS281X_TIMER_MAX_VALUE;

            timer_set_absolute(WS281X_TIMER_DEV, 0, time_to_set);
            timer_set_absolute(WS281X_TIMER_DEV, 1, time_to_clear);
            timer_start(WS281X_TIMER_DEV);
            /* A
             * ~~~
             * while ((int)timer_read(WS281X_TIMER_DEV) < time_to_set) {};
             * ~~~
             * would be way too slow, plus it'd impose additional requirements
             * on the timer to avoid wrapping */
            while (!timer_poll_channel(WS281X_TIMER_DEV, 0)) {}
            gpio_ll_set(port, mask);
            while (!timer_poll_channel(WS281X_TIMER_DEV, 1)) {}
            gpio_ll_clear(port, mask);
            timer_stop(WS281X_TIMER_DEV);

            last_bit = bit;
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
            port, pin, params->pin, err);
    if (err != 0) {
        return -EIO;
    }

    err = timer_init(WS281X_TIMER_DEV, WS281X_TIMER_FREQ, NULL, NULL);
    DEBUG("Initialized timer to %d Hz: %d\n", WS281X_TIMER_FREQ, err);
    if (err != 0) {
        return -EIO;
    }
    timer_stop(WS281X_TIMER_DEV);

    /* We're not trying to make an assessment on whether that means we can
     * manage or not, for that also depends on the number of instructions in
     * the loop, which adds to the granularity. (Really the relevant time is
     * the duration converted back from ticks to time, rounded up or down
     * randomly to the duration of the loop). */
    DEBUG("At this speed, ones are %d ticks long and zeros %d ticks of a %d "
            "tick transmission.\n", ticks_one, ticks_zero, ticks_data);

    return 0;
}
