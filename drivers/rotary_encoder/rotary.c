/*
 * Copyright (C) 2016 Marc Poulhiès
 * Copyright (C) 2011 Ben Buxton <bb@cactii.net>
 *
 * Licensed under the GNU GPL Version 3
 */

/**
 * @ingroup     drivers_rotary_encoder
 * @{
 *
 * @file
 * @brief       Low-level rotary encoder implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#include "rotary.h"

/*
 * Code mainly based on http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 */

#define ENABLE_DEBUG (0)

#include "debug.h"

#define R_START 0x0

#ifndef HALF_STEP
#define HALF_STEP 0
#endif

#if HALF_STEP

/* Use the half-step state table (emits a code at 00 and 11) */
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5

static const unsigned char ttable[6][4] = {
    /* R_START (00) */
    { R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START },
    /* R_CCW_BEGIN */
    { R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START },
    /* R_CW_BEGIN */
    { R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START },
    /* R_START_M (11) */
    { R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START },
    /* R_CW_BEGIN_M */
    { R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW },
    /* R_CCW_BEGIN_M */
    { R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW },
};
#else

/* Use the full-step state table (emits a code at 00 only) */
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

static const unsigned char ttable[7][4] = {
    /* R_START */
    { R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START },
    /* R_CW_FINAL */
    { R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW },
    /* R_CW_BEGIN */
    { R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START },
    /* R_CW_NEXT */
    { R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START },
    /* R_CCW_BEGIN */
    { R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START },
    /* R_CCW_FINAL */
    { R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW },
    /* R_CCW_NEXT */
    { R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START },
};
#endif

static void rotary_cb(void *rot_dev)
{
    rotary_t *dev = (rotary_t *) rot_dev;
    const unsigned int b1_v = gpio_read(dev->pin1);
    const unsigned int b2_v = gpio_read(dev->pin2);
    const unsigned int pinstate = (b1_v ? 2 : 0) | (b2_v ? 1 : 0);
    unsigned dir = 0;

    dev->state = ttable[dev->state & 0xf][pinstate];

    switch (dev->state & 0x30) {
        case DIR_CCW:
        case DIR_CW:
            dir = dev->state & 0x30;
            break;
        case DIR_NONE:
        default:
            break;
    }

    if (dev->listener != KERNEL_PID_UNDEF && dir != 0) {
        msg_t m;
        m.type = ROTARY_MSG_EVENT;
        m.content.value = dir;
        DEBUG("Dispatching event : %d to pid %d\n", dir, dev->listener);
        if (msg_send_int(&m, dev->listener) != 1) {
            DEBUG("Not sent, queue full ?\n");
        }
        ;
    }
}

int rotary_init(rotary_t *dev, gpio_t pin1, gpio_t pin2)
{
    gpio_init_int(pin1, GPIO_IN_PU, GPIO_BOTH, rotary_cb, dev);
    gpio_init_int(pin2, GPIO_IN_PU, GPIO_BOTH, rotary_cb, dev);

    dev->pin1 = pin1;
    dev->pin2 = pin2;

    dev->state = R_START;
    dev->listener = KERNEL_PID_UNDEF;

    return 0;
}

void rotary_register(rotary_t *dev, kernel_pid_t pid)
{
    dev->listener = pid;
}
