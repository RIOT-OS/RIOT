/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for SM_PWM_01C driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#include "sm_pwm_01c.h"
#include "sm_pwm_01c_params.h"

#include "progress_bar.h"

static progress_bar_t progress_bar_list[2];

int main(void)
{
    sm_pwm_01c_t dev;

    puts("sm_pwm_01c driver test application");
    if (sm_pwm_01c_init(&dev, &sm_pwm_01c_params[0]) != 0) {
        puts("init device [ERROR]");
        return -1;
    }

    puts("starting weighted average PM2.5 and PM10 measurements\n");
    sm_pwm_01c_start(&dev);
    progress_bar_prepare_multi(2);

    while (1) {
        ztimer_sleep(ZTIMER_USEC, 200 * US_PER_MS);
        sm_pwm_01c_data_t data;
        sm_pwm_01c_read_data(&dev, &data);
        sprintf(progress_bar_list[0].prefix,
                "%s %4d ug/m3", "PM2.5 level:",
                data.mc_pm_2p5);
        sprintf(progress_bar_list[1].prefix, "%s %4d ug/m3", "PM10  level:",
                data.mc_pm_10);
        progress_bar_list[0].value = data.mc_pm_2p5 >
                                     3000 ? 100 : (data.mc_pm_2p5 * 100) / 3000;
        progress_bar_list[1].value = data.mc_pm_10 >
                                     3000 ? 100 : (data.mc_pm_10 * 100) / 3000;
        progress_bar_update_multi(progress_bar_list, 2);
    }

    return 0;
}
