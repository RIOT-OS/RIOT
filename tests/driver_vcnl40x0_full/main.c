/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the VCNL40X0 driver. It uses a
 * VCNL4010, VCNL4020, or VCNL3020 sensor connected to I2C_DEV(0) with
 * following configuration parameters:
 *
 * - Ambient light measurement with
 *      - a rate of 4 per second (#VCNL40X0_ALS_RATE_4) and
 *      - averaging over 32 conversions (#VCNL40X0_ALS_AVG_32)
 *
 * - Proximity measurement with
 *      - a rate of 1.95 per seconds (#VCNL40X0_PRX_RATE_1_95) and
 *      - an IR LED current of 20 mA.
 *
 * The application uses data ready interrupts to fetch the data. Furthermore,
 * it configures and enables threshold interrupts that are triggered when
 * ambient light exceed the thresholds.
 */

#ifndef VCNL40X0_PARAM_ALS_RATE
#define VCNL40X0_PARAM_ALS_RATE     (VCNL40X0_ALS_RATE_10)
#endif
#ifndef VCNL40X0_PARAM_PRX_RATE
#define VCNL40X0_PARAM_PRX_RATE     (VCNL40X0_PRX_RATE_1_95)
#endif

#include <stdio.h>

#include "mbox.h"
#include "thread.h"
#include "xtimer.h"

#include "vcnl40x0.h"
#include "vcnl40x0_params.h"

#define SLEEP   (100 * US_PER_MS)

kernel_pid_t p_main;

static msg_t  ie_queue[4]; /* message queue for up to 4 interrupt envents */
static mbox_t ie_mbox;     /* mail box for interrupt events */

/*
 * Interrupt service routine that handles all interrupts and informs the main
 * thread about the type of interrupt.
 */
static void vcnl40x0_isr (void *arg)
{
    /* send a message to trigger main thread to handle the interrupt */
    msg_t ie = { .type = (vcnl40x0_int_t)arg };
    mbox_try_put(&ie_mbox, &ie);
}

int main(void)
{
    vcnl40x0_t dev;

    p_main = sched_active_pid;
    mbox_init (&ie_mbox, ie_queue, sizeof(ie_queue) / sizeof(ie_queue[0]));

    puts("VCNL40X0 proximity and ambient light sensor driver test application\n");
    puts("Initializing VCNL40X0 sensor");

    /* initialize the sensor with default configuration parameters */
    if (vcnl40x0_init(&dev, &vcnl40x0_params[0]) == VCNL40X0_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* enable the the DRDY interrupts */
    vcnl40x0_enable_int(&dev, VCNL40X0_INT_ALS_DRDY, vcnl40x0_isr, (void*)VCNL40X0_INT_ALS_DRDY);
    vcnl40x0_enable_int(&dev, VCNL40X0_INT_PRX_DRDY, vcnl40x0_isr, (void*)VCNL40X0_INT_PRX_DRDY);

    /* configure and enable threshold interrupts */
    vcnl40x0_set_int_thresh(&dev, VCNL40X0_INT_THS_ALS, 100, 500, VCNL40X0_INT_THS_4);
    vcnl40x0_enable_int(&dev, VCNL40X0_INT_THS_LO, vcnl40x0_isr, (void*)VCNL40X0_INT_THS_LO);
    vcnl40x0_enable_int(&dev, VCNL40X0_INT_THS_HI, vcnl40x0_isr, (void*)VCNL40X0_INT_THS_HI);

    uint16_t min_prox =  UINT16_MAX;    /* proximity offset value */

    while (1) {
        uint16_t prox;
        uint16_t als;

        /* wait for any interrupt event */
        msg_t ie;
        mbox_get (&ie_mbox, &ie);

        /* in case of any data ready interrupts fetch and print the data */
        if (ie.type == VCNL40X0_INT_PRX_DRDY &&
            vcnl40x0_read_prox(&dev, &prox) == VCNL40X0_OK) {

            /*
             * Simple proximity offset correction:
             * A number of disturbing effects such as DC noise, sensor
             * coverage, or surrounding objects cause an offset in the
             * measured proximity values. Find the minimum value ever
             * measured and subtract it from the current reading. The
             * minimum value is assumed to be 0 (no proximity).
             */
            min_prox = (prox < min_prox) ? prox : min_prox;
            prox -= min_prox;

            printf("Proximity [cnts]: %5" PRIu16 "\n", prox);
        }
        if (ie.type == VCNL40X0_INT_ALS_DRDY &&
            vcnl40x0_read_als(&dev, &als) == VCNL40X0_OK) {
            printf("ALS [lux] %5" PRIu16 ".%02" PRIu16 "\n", als/4, (als % 4) * 25);
        }

        /* in case of any data threshold interrupts */
        if (ie.type == VCNL40X0_INT_THS_LO) {
            printf("ALS low threshold exceeded\n");
        }
        if (ie.type == VCNL40X0_INT_THS_HI) {
            printf("ALS high threshold exceeded\n");
        }
    }
    return 0;
}
