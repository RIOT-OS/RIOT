/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the LIS2DH12 accelerometer driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "fmt.h"
#include "xtimer.h"
#include "mutex.h"
#include "lis2dh12.h"
#include "lis2dh12_params.h"

 /* delay between sensor data reads */
#define DELAY       (100UL * US_PER_MS)

/* allocate some memory to hold one formatted string for each sensor output, so
 * one string for the X, Y, and Z reading, respectively */
static char str_out[3][8];

/* allocate device descriptor */
static lis2dh12_t dev;

#ifdef MODULE_LIS2DH12_INT
/* control interrupt */
typedef struct {
    uint8_t line;
    mutex_t *lock;
    uint8_t *flags;
} lis_ctx;

/* timer lock */
static uint8_t isr_flags;
static mutex_t isr_mtx = MUTEX_INIT_LOCKED;
static lis_ctx ctx[2] = {
    {
        .line  = 1,
        .lock  = &isr_mtx,
        .flags = &isr_flags,
    }, {
        .line  = 2,
        .lock  = &isr_mtx,
        .flags = &isr_flags,
    }
};

/* interrupt callback function. */
static void lis2dh12_int_cb(void* _ctx) {
    lis_ctx *control = _ctx;

    *control->flags |= control->line;

    mutex_unlock(control->lock);
}

/* print interrupt register */
static void lis2dh12_int_reg_content(lis2dh12_t *dev, uint8_t pin){

    assert(pin == LIS2DH12_INT1 || pin == LIS2DH12_INT2);

    uint8_t buffer;
    lis2dh12_read_int_src(dev, &buffer, pin);

    printf("content SRC_Reg_%d: 0x%02x\n", pin, buffer);
    printf("\t XL %d\n", !!(buffer & LIS2DH12_INT_SRC_XL));
    printf("\t XH %d\n", !!(buffer & LIS2DH12_INT_SRC_XH));
    printf("\t YL %d\n", !!(buffer & LIS2DH12_INT_SRC_YL));
    printf("\t YH %d\n", !!(buffer & LIS2DH12_INT_SRC_YH));
    printf("\t ZL %d\n", !!(buffer & LIS2DH12_INT_SRC_ZL));
    printf("\t ZH %d\n", !!(buffer & LIS2DH12_INT_SRC_ZH));
    printf("\t IA %d\n", !!(buffer & LIS2DH12_INT_SRC_IA));
}
#endif

int main(void)
{

#ifdef MODULE_LIS2DH12_INT
    uint8_t flags = 0;
#endif

    puts("LIS2DH12 accelerometer driver test application\n");

    puts("Initializing LIS2DH12 sensor... ");
    if (lis2dh12_init(&dev, &lis2dh12_params[0]) == LIS2DH12_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

#ifdef MODULE_LIS2DH12_INT
    /* enable interrupt Pins */
    if (gpio_is_valid(lis2dh12_params[0].int1_pin)) {
        /* create and set the interrupt params */
        lis2dh12_int_params_t params_int1 = {
            .int_type = LIS2DH12_INT_TYPE_I1_IA1,
            .int_config = LIS2DH12_INT_CFG_XLIE,
            .int_threshold = 31,
            .int_duration = 1,
            .cb = lis2dh12_int_cb,
            .arg = &ctx[0],
        };
        lis2dh12_set_int(&dev, &params_int1, LIS2DH12_INT1);
    }

    /* create and set the interrupt params */
    if (gpio_is_valid(lis2dh12_params[0].int2_pin)) {
        lis2dh12_int_params_t params_int2 = {
            .int_type = LIS2DH12_INT_TYPE_I2_IA2,
            .int_config = LIS2DH12_INT_CFG_YLIE,
            .int_threshold = 31,
            .int_duration = 1,
            .cb = lis2dh12_int_cb,
            .arg = &ctx[1],
        };
        lis2dh12_set_int(&dev, &params_int2, LIS2DH12_INT2);
    }
#endif

    while (1) {

#ifdef MODULE_LIS2DH12_INT
        if (xtimer_mutex_lock_timeout(&isr_mtx, DELAY) == 0) {
            flags = isr_flags;
            isr_flags = 0;
        }

        /* check interrupt 1 and read register */
        if (flags & 0x1) {
            printf("reads interrupt %d\n", LIS2DH12_INT1);
            lis2dh12_int_reg_content(&dev, LIS2DH12_INT1);
            flags &= ~(0x1);
        }
        /* check interrupt 2 and read register */
        if (flags & 0x2) {
            printf("reads interrupt %d\n", LIS2DH12_INT2);
            lis2dh12_int_reg_content(&dev, LIS2DH12_INT2);
            flags &= ~(0x2);
        }
#else
        xtimer_usleep(DELAY);
#endif

        /* read sensor data */
        int16_t data[3];
        if (lis2dh12_read(&dev, data) != LIS2DH12_OK) {
            puts("error: unable to retrieve data from sensor");
            continue;
        }

        /* format data */
        for (int i = 0; i < 3; i++) {
            size_t len = fmt_s16_dfp(str_out[i], data[i], -3);
            str_out[i][len] = '\0';
        }

        /* print data to STDIO */
        printf("X: %8s Y: %8s Z: %8s\n", str_out[0], str_out[1], str_out[2]);
    }

    return 0;
}
