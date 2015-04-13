/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_sensor
 * @{
 *
 * @file        sense.c
 * @brief       CeBIT 2014 demo application - sensor node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <math.h>

#include "thread.h"
#include "smb380-board.h"
#include "vtimer.h"
#include "kernel.h"
#include "board.h"

#include "sense.h"
#include "evt_handler.h"


#define THREAD_PRIO         (10U)

#define AXIS_THRESHOLD      (230)
#define SAMPLING_PERIOD     (50000U)
#define REP_LIMIT           (3U)

#define STATE_NORMAL        (0U)
#define STATE_DOWN1         (2U)
#define STATE_DOWN2         (1U)

#define STACKSIZE           KERNEL_CONF_STACKSIZE_MAIN

static char stack[STACKSIZE];
static int sensepid;


static int16_t acc_data[6];
static int state = -1;
static int rep_count = 0;


void sensethread(void);
void check_state(void);
int math_modulus(int16_t *v, int dim);


void sensethread(void)
{
    timex_t delay = timex_set(0, SAMPLING_PERIOD);
    for (;;) {
        SMB380_getAcceleration(SMB380_X_AXIS, &acc_data[SMB380_X_AXIS], &acc_data[SMB380_X_AXIS + 3]);
        SMB380_getAcceleration(SMB380_Y_AXIS, &acc_data[SMB380_Y_AXIS], &acc_data[SMB380_Y_AXIS + 3]);
        SMB380_getAcceleration(SMB380_Z_AXIS, &acc_data[SMB380_Z_AXIS], &acc_data[SMB380_Z_AXIS + 3]);
        check_state();
        vtimer_sleep(delay);
    }
}

void check_state(void)
{
    int norm = math_modulus(acc_data, 3);
    // printf("--- norm: %d, x: %d, y: %d, z: %d ---\n", norm, acc_data[0], acc_data[1], acc_data[2]);

    if (norm > 235 && norm < 275) {
        // puts("NORM OK");
        for (int i = 0; i < 3; i++) {
            if (acc_data[i] > AXIS_THRESHOLD || acc_data[i] < -AXIS_THRESHOLD) {
                // printf("axis %d;  %d > %d", i, acc_data[i], AXIS_THRESHOLD);
                if (state == i) {
                    if (rep_count < REP_LIMIT) {
                        ++rep_count;
                    }
                    // printf("inc %i\n", i);
                } else {
                    state = i;
                    rep_count = 0;
                    // printf("new dir: %i\n", i);
                }
            }
        }
    }
    
    if (rep_count == REP_LIMIT) {
        rep_count = REP_LIMIT + 1;
        switch (state) {
            case STATE_NORMAL:
                evt_handler_ok();
                break;
            case STATE_DOWN1:
                evt_handler_warn();
                break;
            case STATE_DOWN2:
                evt_handler_alarm();
                break;
        }
    }
}


void sense_init(void)
{
    // initialize the SMB380 acceleration sensor
    SMB380_init_simple(100, SMB380_BAND_WIDTH_375HZ, SMB380_RANGE_2G);
    puts("SMB380 initialized.");

    // setup and start sense thread
    sensepid = thread_create(stack, STACKSIZE, THREAD_PRIO, CREATE_STACKTEST, sensethread, "sense");
    puts("Sense thread created.");
}


int math_modulus(int16_t *v, int dim)
{
    float mod = 0.0f;
    for (int i = 0; i < dim; i++) {
        mod += powf(v[i], 2.0f);
    }
    return (int)sqrtf(mod);
}
